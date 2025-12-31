#pragma once

#include "types_basic.hpp"
#include <algorithm>
#include <cmath>

namespace pigment {

    struct XYZ {
        // X, Y, Z values (typically 0-95.047, 0-100, 0-108.883 for D65 illuminant)
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;

        XYZ() = default;
        XYZ(double X_, double Y_, double Z_) : X(X_), Y(Y_), Z(Z_) {}

        // Create XYZ from RGB (using sRGB color space with D65 illuminant)
        static XYZ fromRGB(const RGB &c) {
            // Convert RGB to linear RGB
            auto linearize = [](double val) {
                val = val / 255.0;
                if (val <= 0.04045) {
                    return val / 12.92;
                } else {
                    return std::pow((val + 0.055) / 1.055, 2.4);
                }
            };

            double r_linear = linearize(c.r);
            double g_linear = linearize(c.g);
            double b_linear = linearize(c.b);

            // Apply sRGB to XYZ transformation matrix (D65 illuminant)
            XYZ result;
            result.X = r_linear * 0.4124564 + g_linear * 0.3575761 + b_linear * 0.1804375;
            result.Y = r_linear * 0.2126729 + g_linear * 0.7151522 + b_linear * 0.0721750;
            result.Z = r_linear * 0.0193339 + g_linear * 0.1191920 + b_linear * 0.9503041;

            // Scale to standard illuminant D65 range
            result.X *= 95.047;
            result.Y *= 100.0;
            result.Z *= 108.883;

            return result;
        }

        // Convert XYZ to RGB
        RGB to_rgb() const {
            // Normalize to 0-1 range
            double x = X / 95.047;
            double y = Y / 100.0;
            double z = Z / 108.883;

            // Apply XYZ to sRGB transformation matrix
            double r_linear = x * 3.2404542 + y * -1.5371385 + z * -0.4985314;
            double g_linear = x * -0.9692660 + y * 1.8760108 + z * 0.0415560;
            double b_linear = x * 0.0556434 + y * -0.2040259 + z * 1.0572252;

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
        bool operator==(const XYZ &other) const {
            const double epsilon = 1e-6;
            return std::abs(X - other.X) < epsilon && std::abs(Y - other.Y) < epsilon &&
                   std::abs(Z - other.Z) < epsilon;
        }

        bool operator!=(const XYZ &other) const { return !(*this == other); }

        // Get luminance (Y component represents luminance)
        double luminance() const { return Y; }

        // Normalize values to prevent out-of-gamut issues
        void normalize() {
            X = std::max(0.0, X);
            Y = std::max(0.0, Y);
            Z = std::max(0.0, Z);
        }
    };

    // Implementation of RGB conversion constructor for XYZ
    inline RGB::RGB(const XYZ &xyz) {
        RGB temp = xyz.to_rgb();
        r = temp.r;
        g = temp.g;
        b = temp.b;
        a = temp.a;
    }

} // namespace pigment