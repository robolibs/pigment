#pragma once

#include "types_basic.hpp"

#include <algorithm>
#include <cmath>

namespace pigment {

    /**
     * @brief OKLAB color type built on datapod::mat::vector<double, 3>
     *
     * Stores L (lightness 0-1), a and b (green-red and blue-yellow, ~-0.4 to 0.4).
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct OKLAB : public datapod::mat::vector<double, 3> {
        using base_type = datapod::mat::vector<double, 3>;

        // Accessors for color components
        double &l() { return data_[0]; }
        double &a() { return data_[1]; }
        double &b() { return data_[2]; }

        const double &l() const { return data_[0]; }
        const double &a() const { return data_[1]; }
        const double &b() const { return data_[2]; }

        OKLAB() {
            data_[0] = 0.0;
            data_[1] = 0.0;
            data_[2] = 0.0;
        }

        OKLAB(double l_, double a_, double b_) {
            data_[0] = l_;
            data_[1] = a_;
            data_[2] = b_;
        }

        // Create OKLAB from RGB using the improved Oklab color space
        static OKLAB fromRGB(const RGB &c) {
            // First convert RGB to linear RGB
            auto linearize = [](double val) {
                val = val / 255.0;
                if (val <= 0.04045) {
                    return val / 12.92;
                } else {
                    return std::pow((val + 0.055) / 1.055, 2.4);
                }
            };

            double r_val = linearize(c.r());
            double g_val = linearize(c.g());
            double b_val = linearize(c.b());

            // Convert linear RGB to LMS (cone response)
            double lms_l = 0.4122214708 * r_val + 0.5363325363 * g_val + 0.0514459929 * b_val;
            double lms_m = 0.2119034982 * r_val + 0.6806995451 * g_val + 0.1073969566 * b_val;
            double lms_s = 0.0883024619 * r_val + 0.2817188376 * g_val + 0.6299787005 * b_val;

            // Apply cube root
            lms_l = std::cbrt(lms_l);
            lms_m = std::cbrt(lms_m);
            lms_s = std::cbrt(lms_s);

            // Convert to Oklab
            OKLAB result;
            result.data_[0] = 0.2104542553 * lms_l + 0.7936177850 * lms_m - 0.0040720468 * lms_s;
            result.data_[1] = 1.9779984951 * lms_l - 2.4285922050 * lms_m + 0.4505937099 * lms_s;
            result.data_[2] = 0.0259040371 * lms_l + 0.7827717662 * lms_m - 0.8086757660 * lms_s;

            return result;
        }

        // Convert OKLAB to RGB
        RGB to_rgb() const {
            // Convert Oklab to LMS
            double lms_l = l() + 0.3963377774 * a() + 0.2158037573 * b();
            double lms_m = l() - 0.1055613458 * a() - 0.0638541728 * b();
            double lms_s = l() - 0.0894841775 * a() - 1.2914855480 * b();

            // Apply cube (inverse of cube root)
            lms_l = lms_l * lms_l * lms_l;
            lms_m = lms_m * lms_m * lms_m;
            lms_s = lms_s * lms_s * lms_s;

            // Convert LMS to linear RGB
            double r_linear = +4.0767416621 * lms_l - 3.3077115913 * lms_m + 0.2309699292 * lms_s;
            double g_linear = -1.2684380046 * lms_l + 2.6097574011 * lms_m - 0.3413193965 * lms_s;
            double b_linear = -0.0041960863 * lms_l - 0.7034186147 * lms_m + 1.7076147010 * lms_s;

            // Apply gamma correction
            auto gamma_correct = [](double val) {
                if (val <= 0.0031308) {
                    return val * 12.92;
                } else {
                    return 1.055 * std::pow(val, 1.0 / 2.4) - 0.055;
                }
            };

            double r_val = gamma_correct(r_linear);
            double g_val = gamma_correct(g_linear);
            double b_val = gamma_correct(b_linear);

            // Convert to 0-255 range and clamp
            return RGB(static_cast<uint8_t>(std::clamp(std::round(r_val * 255.0), 0.0, 255.0)),
                       static_cast<uint8_t>(std::clamp(std::round(g_val * 255.0), 0.0, 255.0)),
                       static_cast<uint8_t>(std::clamp(std::round(b_val * 255.0), 0.0, 255.0)), 255);
        }

        // Equality operators
        bool operator==(const OKLAB &other) const {
            const double epsilon = 1e-6;
            return std::abs(l() - other.l()) < epsilon && std::abs(a() - other.a()) < epsilon &&
                   std::abs(b() - other.b()) < epsilon;
        }

        bool operator!=(const OKLAB &other) const { return !(*this == other); }

        // Get lightness
        double lightness() const { return l(); }

        // Get chroma (colorfulness)
        double chroma() const { return std::sqrt(a() * a() + b() * b()); }

        // Get hue angle in radians
        double hue_radians() const { return std::atan2(b(), a()); }

        // Get hue angle in degrees
        double hue_degrees() const {
            double h = hue_radians() * 180.0 / M_PI;
            return h < 0 ? h + 360.0 : h;
        }

        // Adjust lightness
        OKLAB adjust_lightness(double delta) const { return OKLAB(std::clamp(l() + delta, 0.0, 1.0), a(), b()); }

        // Adjust chroma (saturation)
        OKLAB adjust_chroma(double factor) const { return OKLAB(l(), a() * factor, b() * factor); }

        // Rotate hue by angle in degrees
        OKLAB rotate_hue(double degrees) const {
            double radians = degrees * M_PI / 180.0;
            double cos_h = std::cos(radians);
            double sin_h = std::sin(radians);

            return OKLAB(l(), a() * cos_h - b() * sin_h, a() * sin_h + b() * cos_h);
        }

        // Calculate perceptual distance to another color
        double distance(const OKLAB &other) const {
            double dl = l() - other.l();
            double da = a() - other.a();
            double db = b() - other.b();
            return std::sqrt(dl * dl + da * da + db * db);
        }
    };

    // Implementation of RGB conversion constructor for OKLAB
    inline RGB::RGB(const OKLAB &oklab) {
        RGB temp = oklab.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
