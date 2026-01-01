#pragma once

#include "types_basic.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace pigment {

    namespace lab_tables {
        // Lookup table for gamma correction (sRGB -> linear)
        constexpr size_t GAMMA_TABLE_SIZE = 256;
        constexpr std::array<double, GAMMA_TABLE_SIZE> create_gamma_to_linear_table() {
            std::array<double, GAMMA_TABLE_SIZE> table{};
            for (size_t i = 0; i < GAMMA_TABLE_SIZE; ++i) {
                double val = i / 255.0;
                table[i] = (val > 0.04045) ? std::pow((val + 0.055) / 1.055, 2.4) : val / 12.92;
            }
            return table;
        }

        // Lookup table for inverse gamma correction (linear -> sRGB)
        constexpr size_t LINEAR_TABLE_SIZE = 4096;
        constexpr std::array<double, LINEAR_TABLE_SIZE> create_linear_to_gamma_table() {
            std::array<double, LINEAR_TABLE_SIZE> table{};
            for (size_t i = 0; i < LINEAR_TABLE_SIZE; ++i) {
                double val = i / double(LINEAR_TABLE_SIZE - 1);
                table[i] = (val > 0.0031308) ? 1.055 * std::pow(val, 1.0 / 2.4) - 0.055 : 12.92 * val;
            }
            return table;
        }

        // Lookup table for LAB f function
        constexpr size_t LAB_F_TABLE_SIZE = 4096;
        constexpr std::array<double, LAB_F_TABLE_SIZE> create_lab_f_table() {
            std::array<double, LAB_F_TABLE_SIZE> table{};
            for (size_t i = 0; i < LAB_F_TABLE_SIZE; ++i) {
                double t = i / double(LAB_F_TABLE_SIZE - 1) * 2.0; // Scale to [0, 2]
                table[i] = (t > 0.008856) ? std::pow(t, 1.0 / 3.0) : (7.787 * t + 16.0 / 116.0);
            }
            return table;
        }

        // Lookup table for LAB f inverse function
        constexpr std::array<double, LAB_F_TABLE_SIZE> create_lab_f_inv_table() {
            std::array<double, LAB_F_TABLE_SIZE> table{};
            for (size_t i = 0; i < LAB_F_TABLE_SIZE; ++i) {
                double t = i / double(LAB_F_TABLE_SIZE - 1) * 2.0; // Scale to [0, 2]
                double t3 = t * t * t;
                table[i] = (t3 > 0.008856) ? t3 : (t - 16.0 / 116.0) / 7.787;
            }
            return table;
        }

        // Create the lookup tables
        static const auto gamma_to_linear = create_gamma_to_linear_table();
        static const auto linear_to_gamma = create_linear_to_gamma_table();
        static const auto lab_f = create_lab_f_table();
        static const auto lab_f_inv = create_lab_f_inv_table();

        // Fast lookup functions
        inline double fast_gamma_to_linear(uint8_t val) { return gamma_to_linear[val]; }

        inline double fast_linear_to_gamma(double val) {
            val = std::clamp(val, 0.0, 1.0);
            size_t index = static_cast<size_t>(val * (LINEAR_TABLE_SIZE - 1));
            return linear_to_gamma[index];
        }

        inline double fast_lab_f(double t) {
            t = std::clamp(t / 2.0, 0.0, 1.0); // Normalize to [0, 1]
            size_t index = static_cast<size_t>(t * (LAB_F_TABLE_SIZE - 1));
            return lab_f[index];
        }

        inline double fast_lab_f_inv(double t) {
            t = std::clamp(t / 2.0, 0.0, 1.0); // Normalize to [0, 1]
            size_t index = static_cast<size_t>(t * (LAB_F_TABLE_SIZE - 1));
            return lab_f_inv[index];
        }
    } // namespace lab_tables

    /**
     * @brief LAB color type built on datapod::mat::vector<double, 4>
     *
     * Stores L* (lightness 0-100), a* (green-red), b* (blue-yellow), and alpha.
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct LAB : public datapod::mat::vector<double, 4> {
        using base_type = datapod::mat::vector<double, 4>;

        // Accessors for color components
        double &l() { return data_[0]; }
        double &a() { return data_[1]; }
        double &b() { return data_[2]; }
        double &alpha() { return data_[3]; }

        const double &l() const { return data_[0]; }
        const double &a() const { return data_[1]; }
        const double &b() const { return data_[2]; }
        const double &alpha() const { return data_[3]; }

        LAB() {
            data_[0] = 0.0;
            data_[1] = 0.0;
            data_[2] = 0.0;
            data_[3] = 255.0;
        }

        LAB(double l_, double a_, double b_, double alpha_ = 255.0) {
            data_[0] = l_;
            data_[1] = a_;
            data_[2] = b_;
            data_[3] = alpha_;
        }

        // Convert from RGB using D65 illuminant (optimized with lookup tables)
        static LAB fromRGB(const RGB &rgb) {
            // Use lookup tables for gamma correction
            double r_val = lab_tables::fast_gamma_to_linear(rgb.r());
            double g_val = lab_tables::fast_gamma_to_linear(rgb.g());
            double b_val = lab_tables::fast_gamma_to_linear(rgb.b());

            // Convert to XYZ using sRGB matrix
            double x = r_val * 0.4124564 + g_val * 0.3575761 + b_val * 0.1804375;
            double y = r_val * 0.2126729 + g_val * 0.7151522 + b_val * 0.0721750;
            double z = r_val * 0.0193339 + g_val * 0.1191920 + b_val * 0.9503041;

            // Normalize to D65 illuminant
            x /= 0.95047;
            y /= 1.00000;
            z /= 1.08883;

            // Convert XYZ to LAB using lookup tables
            double fx = lab_tables::fast_lab_f(x);
            double fy = lab_tables::fast_lab_f(y);
            double fz = lab_tables::fast_lab_f(z);

            LAB lab;
            lab.data_[0] = 116.0 * fy - 16.0;
            lab.data_[1] = 500.0 * (fx - fy);
            lab.data_[2] = 200.0 * (fy - fz);
            lab.data_[3] = static_cast<double>(rgb.a());

            return lab;
        }

        // Convert to RGB (optimized with lookup tables)
        RGB to_rgb() const {
            // Convert LAB to XYZ
            double fy = (l() + 16.0) / 116.0;
            double fx = a() / 500.0 + fy;
            double fz = fy - b() / 200.0;

            // Use lookup tables for f inverse function
            double x = lab_tables::fast_lab_f_inv(fx) * 0.95047;
            double y = lab_tables::fast_lab_f_inv(fy) * 1.00000;
            double z = lab_tables::fast_lab_f_inv(fz) * 1.08883;

            // Convert XYZ to RGB
            double r_val = x * 3.2404542 + y * -1.5371385 + z * -0.4985314;
            double g_val = x * -0.9692660 + y * 1.8760108 + z * 0.0415560;
            double b_val = x * 0.0556434 + y * -0.2040259 + z * 1.0572252;

            // Apply inverse gamma correction using lookup tables
            r_val = lab_tables::fast_linear_to_gamma(r_val);
            g_val = lab_tables::fast_linear_to_gamma(g_val);
            b_val = lab_tables::fast_linear_to_gamma(b_val);

            return RGB(std::clamp(static_cast<int>(std::round(r_val * 255)), 0, 255),
                       std::clamp(static_cast<int>(std::round(g_val * 255)), 0, 255),
                       std::clamp(static_cast<int>(std::round(b_val * 255)), 0, 255), static_cast<uint8_t>(alpha()));
        }

        // Calculate Delta E (color difference) - CIE76 formula
        double delta_e(const LAB &other) const {
            double dl = l() - other.l();
            double da = a() - other.a();
            double db = b() - other.b();
            return std::sqrt(dl * dl + da * da + db * db);
        }

        // More accurate Delta E 2000 calculation
        double delta_e_2000(const LAB &other) const {
            // Simplified implementation - full CIE Delta E 2000 is quite complex
            double dl = l() - other.l();
            double da = a() - other.a();
            double db = b() - other.b();

            double c1 = std::sqrt(a() * a() + b() * b());
            double c2 = std::sqrt(other.a() * other.a() + other.b() * other.b());
            double dc = c1 - c2;

            double dh = std::sqrt(da * da + db * db - dc * dc);

            double sl = 1.0;
            double sc = 1.0 + 0.045 * c1;
            double sh = 1.0 + 0.015 * c1;

            return std::sqrt((dl / sl) * (dl / sl) + (dc / sc) * (dc / sc) + (dh / sh) * (dh / sh));
        }

        // Check if two colors are perceptually similar
        bool is_similar(const LAB &other, double threshold = 2.3) const { return delta_e(other) < threshold; }

        // Adjust lightness
        LAB adjust_lightness(double amount) const {
            return LAB(std::clamp(l() + amount, 0.0, 100.0), a(), b(), alpha());
        }

        // Mix two LAB colors
        LAB mix(const LAB &other, double ratio = 0.5) const {
            ratio = std::clamp(ratio, 0.0, 1.0);
            return LAB(l() * (1 - ratio) + other.l() * ratio, a() * (1 - ratio) + other.a() * ratio,
                       b() * (1 - ratio) + other.b() * ratio, alpha() * (1 - ratio) + other.alpha() * ratio);
        }
    };

    // Implementation of RGB conversion constructor
    inline RGB::RGB(const LAB &lab) {
        RGB temp = lab.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
