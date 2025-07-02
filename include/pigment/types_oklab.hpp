#pragma once

#include "types_basic.hpp"
#include "types_xyz.hpp"
#include <algorithm>
#include <cmath>

namespace pigment {

    struct OKLAB {
        // L (lightness): 0-1, a and b (green-red and blue-yellow): ~-0.4 to 0.4
        double L = 0.0;
        double a = 0.0;
        double b = 0.0;

        OKLAB() = default;
        OKLAB(double L_, double a_, double b_) : L(L_), a(a_), b(b_) {}

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

            double r = linearize(c.r);
            double g = linearize(c.g);
            double b = linearize(c.b);

            // Convert linear RGB to LMS (cone response)
            double l = 0.4122214708 * r + 0.5363325363 * g + 0.0514459929 * b;
            double m = 0.2119034982 * r + 0.6806995451 * g + 0.1073969566 * b;
            double s = 0.0883024619 * r + 0.2817188376 * g + 0.6299787005 * b;

            // Apply cube root
            l = std::cbrt(l);
            m = std::cbrt(m);
            s = std::cbrt(s);

            // Convert to Oklab
            OKLAB result;
            result.L = 0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s;
            result.a = 1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s;
            result.b = 0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s;

            return result;
        }

        // Convert OKLAB to RGB
        RGB to_rgb() const {
            // Convert Oklab to LMS
            double l = L + 0.3963377774 * a + 0.2158037573 * b;
            double m = L - 0.1055613458 * a - 0.0638541728 * b;
            double s = L - 0.0894841775 * a - 1.2914855480 * b;

            // Apply cube (inverse of cube root)
            l = l * l * l;
            m = m * m * m;
            s = s * s * s;

            // Convert LMS to linear RGB
            double r_linear = +4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s;
            double g_linear = -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s;
            double b_linear = -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s;

            // Apply gamma correction
            auto gamma_correct = [](double val) {
                if (val <= 0.0031308) {
                    return val * 12.92;
                } else {
                    return 1.055 * std::pow(val, 1.0 / 2.4) - 0.055;
                }
            };

            double r = gamma_correct(r_linear);
            double g = gamma_correct(g_linear);
            double b = gamma_correct(b_linear);

            // Convert to 0-255 range and clamp
            RGB result;
            result.r = static_cast<uint8_t>(std::clamp(std::round(r * 255.0), 0.0, 255.0));
            result.g = static_cast<uint8_t>(std::clamp(std::round(g * 255.0), 0.0, 255.0));
            result.b = static_cast<uint8_t>(std::clamp(std::round(b * 255.0), 0.0, 255.0));
            result.a = 255;

            return result;
        }

        // Equality operators
        bool operator==(const OKLAB &other) const {
            const double epsilon = 1e-6;
            return std::abs(L - other.L) < epsilon && 
                   std::abs(a - other.a) < epsilon && 
                   std::abs(b - other.b) < epsilon;
        }

        bool operator!=(const OKLAB &other) const {
            return !(*this == other);
        }

        // Get lightness
        double lightness() const {
            return L;
        }

        // Get chroma (colorfulness)
        double chroma() const {
            return std::sqrt(a * a + b * b);
        }

        // Get hue angle in radians
        double hue_radians() const {
            return std::atan2(b, a);
        }

        // Get hue angle in degrees
        double hue_degrees() const {
            double h = hue_radians() * 180.0 / M_PI;
            return h < 0 ? h + 360.0 : h;
        }

        // Adjust lightness
        OKLAB adjust_lightness(double delta) const {
            return OKLAB(std::clamp(L + delta, 0.0, 1.0), a, b);
        }

        // Adjust chroma (saturation)
        OKLAB adjust_chroma(double factor) const {
            return OKLAB(L, a * factor, b * factor);
        }

        // Rotate hue by angle in degrees  
        OKLAB rotate_hue(double degrees) const {
            double radians = degrees * M_PI / 180.0;
            double cos_h = std::cos(radians);
            double sin_h = std::sin(radians);
            
            return OKLAB(L, 
                        a * cos_h - b * sin_h,
                        a * sin_h + b * cos_h);
        }

        // Calculate perceptual distance to another color
        double distance(const OKLAB &other) const {
            double dL = L - other.L;
            double da = a - other.a;
            double db = b - other.b;
            return std::sqrt(dL * dL + da * da + db * db);
        }
    };

    // Implementation of RGB conversion constructor for OKLAB
    inline RGB::RGB(const OKLAB& oklab) {
        RGB temp = oklab.to_rgb();
        r = temp.r;
        g = temp.g;
        b = temp.b;
        a = temp.a;
    }

} // namespace pigment