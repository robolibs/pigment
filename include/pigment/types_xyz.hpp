#pragma once

#include "types_basic.hpp"

#include <algorithm>
#include <cmath>

namespace pigment {

    /**
     * @brief XYZ color type built on datapod::mat::vector<double, 3>
     *
     * Stores X, Y, Z values (typically 0-95.047, 0-100, 0-108.883 for D65 illuminant).
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct XYZ : public datapod::mat::vector<double, 3> {
        using base_type = datapod::mat::vector<double, 3>;

        // Accessors for color components
        double &x() { return data_[0]; }
        double &y() { return data_[1]; }
        double &z() { return data_[2]; }

        const double &x() const { return data_[0]; }
        const double &y() const { return data_[1]; }
        const double &z() const { return data_[2]; }

        XYZ() {
            data_[0] = 0.0;
            data_[1] = 0.0;
            data_[2] = 0.0;
        }

        XYZ(double x_, double y_, double z_) {
            data_[0] = x_;
            data_[1] = y_;
            data_[2] = z_;
        }

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

            double r_linear = linearize(c.r());
            double g_linear = linearize(c.g());
            double b_linear = linearize(c.b());

            // Apply sRGB to XYZ transformation matrix (D65 illuminant)
            XYZ result;
            result.data_[0] = r_linear * 0.4124564 + g_linear * 0.3575761 + b_linear * 0.1804375;
            result.data_[1] = r_linear * 0.2126729 + g_linear * 0.7151522 + b_linear * 0.0721750;
            result.data_[2] = r_linear * 0.0193339 + g_linear * 0.1191920 + b_linear * 0.9503041;

            // Scale to standard illuminant D65 range
            result.data_[0] *= 95.047;
            result.data_[1] *= 100.0;
            result.data_[2] *= 108.883;

            return result;
        }

        // Convert XYZ to RGB
        RGB to_rgb() const {
            // Normalize to 0-1 range
            double x_val = x() / 95.047;
            double y_val = y() / 100.0;
            double z_val = z() / 108.883;

            // Apply XYZ to sRGB transformation matrix
            double r_linear = x_val * 3.2404542 + y_val * -1.5371385 + z_val * -0.4985314;
            double g_linear = x_val * -0.9692660 + y_val * 1.8760108 + z_val * 0.0415560;
            double b_linear = x_val * 0.0556434 + y_val * -0.2040259 + z_val * 1.0572252;

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
        bool operator==(const XYZ &other) const {
            const double epsilon = 1e-6;
            return std::abs(x() - other.x()) < epsilon && std::abs(y() - other.y()) < epsilon &&
                   std::abs(z() - other.z()) < epsilon;
        }

        bool operator!=(const XYZ &other) const { return !(*this == other); }

        // Get luminance (Y component represents luminance)
        double luminance() const { return y(); }

        // Normalize values to prevent out-of-gamut issues
        void normalize() {
            data_[0] = std::max(0.0, x());
            data_[1] = std::max(0.0, y());
            data_[2] = std::max(0.0, z());
        }
    };

    // Implementation of RGB conversion constructor for XYZ
    inline RGB::RGB(const XYZ &xyz) {
        RGB temp = xyz.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
