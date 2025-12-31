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

    struct LAB {
        double l = 0.0;  // L* (lightness) 0-100
        double a = 0.0;  // a* component (green-red) typically -128 to 127
        double b = 0.0;  // b* component (blue-yellow) typically -128 to 127
        int alpha = 255; // alpha channel 0-255

        LAB() = default;
        LAB(double l_, double a_, double b_, int alpha_ = 255) : l(l_), a(a_), b(b_), alpha(alpha_) {}

        // Convert from RGB using D65 illuminant (optimized with lookup tables)
        static LAB fromRGB(const RGB &rgb) {
            // Use lookup tables for gamma correction
            double r = lab_tables::fast_gamma_to_linear(rgb.r);
            double g = lab_tables::fast_gamma_to_linear(rgb.g);
            double b = lab_tables::fast_gamma_to_linear(rgb.b);

            // Convert to XYZ using sRGB matrix
            double x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
            double y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
            double z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;

            // Normalize to D65 illuminant
            x /= 0.95047;
            y /= 1.00000;
            z /= 1.08883;

            // Convert XYZ to LAB using lookup tables
            double fx = lab_tables::fast_lab_f(x);
            double fy = lab_tables::fast_lab_f(y);
            double fz = lab_tables::fast_lab_f(z);

            LAB lab;
            lab.l = 116.0 * fy - 16.0;
            lab.a = 500.0 * (fx - fy);
            lab.b = 200.0 * (fy - fz);
            lab.alpha = rgb.a;

            return lab;
        }

        // Convert to RGB (optimized with lookup tables)
        RGB to_rgb() const {
            // Convert LAB to XYZ
            double fy = (l + 16.0) / 116.0;
            double fx = a / 500.0 + fy;
            double fz = fy - b / 200.0;

            // Use lookup tables for f inverse function
            double x = lab_tables::fast_lab_f_inv(fx) * 0.95047;
            double y = lab_tables::fast_lab_f_inv(fy) * 1.00000;
            double z = lab_tables::fast_lab_f_inv(fz) * 1.08883;

            // Convert XYZ to RGB
            double r = x * 3.2404542 + y * -1.5371385 + z * -0.4985314;
            double g = x * -0.9692660 + y * 1.8760108 + z * 0.0415560;
            double b = x * 0.0556434 + y * -0.2040259 + z * 1.0572252;

            // Apply inverse gamma correction using lookup tables
            r = lab_tables::fast_linear_to_gamma(r);
            g = lab_tables::fast_linear_to_gamma(g);
            b = lab_tables::fast_linear_to_gamma(b);

            return RGB(std::clamp(static_cast<int>(std::round(r * 255)), 0, 255),
                       std::clamp(static_cast<int>(std::round(g * 255)), 0, 255),
                       std::clamp(static_cast<int>(std::round(b * 255)), 0, 255), alpha);
        }

        // Calculate Delta E (color difference) - CIE76 formula
        double delta_e(const LAB &other) const {
            double dl = l - other.l;
            double da = a - other.a;
            double db = b - other.b;
            return std::sqrt(dl * dl + da * da + db * db);
        }

        // More accurate Delta E 2000 calculation
        double delta_e_2000(const LAB &other) const {
            // Simplified implementation - full CIE Delta E 2000 is quite complex
            double dl = l - other.l;
            double da = a - other.a;
            double db = b - other.b;

            double c1 = std::sqrt(a * a + b * b);
            double c2 = std::sqrt(other.a * other.a + other.b * other.b);
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
        LAB adjust_lightness(double amount) const { return LAB(std::clamp(l + amount, 0.0, 100.0), a, b, alpha); }

        // Mix two LAB colors
        LAB mix(const LAB &other, double ratio = 0.5) const {
            ratio = std::clamp(ratio, 0.0, 1.0);
            return LAB(l * (1 - ratio) + other.l * ratio, a * (1 - ratio) + other.a * ratio,
                       b * (1 - ratio) + other.b * ratio, static_cast<int>(alpha * (1 - ratio) + other.alpha * ratio));
        }
    };

    // Implementation of RGB conversion constructor
    inline RGB::RGB(const LAB &lab) {
        RGB temp = lab.to_rgb();
        r = temp.r;
        g = temp.g;
        b = temp.b;
        a = temp.a;
    }

} // namespace pigment
