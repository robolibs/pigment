#pragma once

#include "types_basic.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

namespace pigment {

    struct HSL {
        uint16_t h = 0;  // 0-36000 (representing 0.0-360.0 degrees * 100)
        uint8_t s = 0;   // 0-255 (representing 0.0-1.0 saturation)
        uint8_t l = 0;   // 0-255 (representing 0.0-1.0 lightness)
        uint8_t a = 255; // 0-255 alpha

        HSL() = default;
        HSL(double h_, double s_, double l_, uint8_t a_ = 255)
            : s(static_cast<uint8_t>(std::clamp(s_, 0.0, 1.0) * 255)), 
              l(static_cast<uint8_t>(std::clamp(l_, 0.0, 1.0) * 255)),
              a(a_) {
            // Normalize hue to [0, 360) before converting to integer
            double normalized_h = std::fmod(h_, 360.0);
            if (normalized_h < 0) normalized_h += 360.0;
            h = static_cast<uint16_t>(normalized_h * 100);
            normalize();
        }

        // Getters for compatibility with tests expecting double values
        double get_h() const { return h / 100.0; }
        double get_s() const { return s / 255.0; }
        double get_l() const { return l / 255.0; }
        
        // Comparison operators for backward compatibility
        bool operator==(double hue) const { return std::abs(get_h() - hue) < 0.1; }
        friend bool operator==(double val, const HSL& hsl) { return hsl == val; }

        void normalize() {
            // Wrap hue to [0, 36000) (0.0-360.0 degrees * 100)
            // Handle negative values properly
            h = ((h % 36000) + 36000) % 36000;
            
            // Saturation and lightness are already clamped by uint8_t range
            s = std::clamp(static_cast<int>(s), 0, 255);
            l = std::clamp(static_cast<int>(l), 0, 255);
            a = std::clamp(static_cast<int>(a), 0, 255);
        }

        // Convert from RGB
        static HSL fromRGB(const RGB &rgb) {
            double r = rgb.r / 255.0;
            double g = rgb.g / 255.0;
            double b = rgb.b / 255.0;

            double max_val = std::max({r, g, b});
            double min_val = std::min({r, g, b});
            double delta = max_val - min_val;

            HSL hsl;

            // Lightness (0-255)
            double lightness = (max_val + min_val) / 2.0;
            hsl.l = static_cast<uint8_t>(lightness * 255);

            if (delta == 0) {
                hsl.h = 0;
                hsl.s = 0; // achromatic
            } else {
                // Saturation (0-255)
                double saturation = lightness > 0.5 ? delta / (2.0 - max_val - min_val) : delta / (max_val + min_val);
                hsl.s = static_cast<uint8_t>(saturation * 255);

                // Hue (0-35999, representing 0.0-359.99 degrees)
                double hue;
                if (max_val == r) {
                    hue = (g - b) / delta + (g < b ? 6 : 0);
                } else if (max_val == g) {
                    hue = (b - r) / delta + 2;
                } else {
                    hue = (r - g) / delta + 4;
                }
                hue /= 6;
                hue *= 360;
                hsl.h = static_cast<uint16_t>(hue * 100);
            }
            hsl.a = rgb.a;
            hsl.normalize();

            return hsl;
        }

        // Convert to RGB
        RGB to_rgb() const {
            double l_norm = l / 255.0;
            double s_norm = s / 255.0;
            
            if (s == 0) {
                return RGB(l, l, l, a);
            }

            auto hue_to_rgb = [](double p, double q, double t) {
                if (t < 0)
                    t += 1;
                if (t > 1)
                    t -= 1;
                if (t < 1.0 / 6)
                    return p + (q - p) * 6 * t;
                if (t < 1.0 / 2)
                    return q;
                if (t < 2.0 / 3)
                    return p + (q - p) * (2.0 / 3 - t) * 6;
                return p;
            };

            double q = l_norm < 0.5 ? l_norm * (1 + s_norm) : l_norm + s_norm - l_norm * s_norm;
            double p = 2 * l_norm - q;
            double h_norm = (h / 100.0) / 360.0;

            double r = hue_to_rgb(p, q, h_norm + 1.0 / 3);
            double g = hue_to_rgb(p, q, h_norm);
            double b = hue_to_rgb(p, q, h_norm - 1.0 / 3);

            return RGB(static_cast<uint8_t>(std::round(r * 255)), 
                       static_cast<uint8_t>(std::round(g * 255)),
                       static_cast<uint8_t>(std::round(b * 255)), a);
        }

        // Color adjustments  
        HSL adjust_hue(double degrees) const { 
            HSL result = *this;
            // Fix integer overflow by properly handling negative values
            int new_hue = h + static_cast<int>(degrees * 100);
            result.h = static_cast<uint16_t>(((new_hue % 36000) + 36000) % 36000);
            return result;
        }

        HSL adjust_saturation(double factor) const { 
            HSL result = *this;
            result.s = static_cast<uint8_t>(std::clamp(s * factor, 0.0, 255.0));
            return result;
        }

        HSL adjust_lightness(double factor) const { 
            HSL result = *this;
            result.l = static_cast<uint8_t>(std::clamp(l * factor, 0.0, 255.0));
            return result;
        }

        HSL saturate(double amount = 0.1) const { 
            HSL result = *this;
            result.s = std::clamp(static_cast<int>(s + amount * 255), 0, 255);
            return result;
        }

        HSL desaturate(double amount = 0.1) const { 
            HSL result = *this;
            result.s = std::clamp(static_cast<int>(s - amount * 255), 0, 255);
            return result;
        }

        HSL lighten(double amount = 0.1) const { 
            HSL result = *this;
            result.l = std::clamp(static_cast<int>(l + amount * 255), 0, 255);
            return result;
        }

        HSL darken(double amount = 0.1) const { 
            HSL result = *this;
            result.l = std::clamp(static_cast<int>(l - amount * 255), 0, 255);
            return result;
        }

        // Complementary color
        HSL complement() const { return adjust_hue(180.0); }

        // Triadic colors
        std::vector<HSL> triadic() const { return {*this, adjust_hue(120.0), adjust_hue(240.0)}; }

        // Analogous colors
        std::vector<HSL> analogous(double angle = 30.0) const { return {adjust_hue(-angle), *this, adjust_hue(angle)}; }

        // Split complementary
        std::vector<HSL> split_complementary(double angle = 30.0) const {
            return {*this, adjust_hue(180.0 - angle), adjust_hue(180.0 + angle)};
        }

        static HSL random() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<uint16_t> hue_dist(0, 35999);
            std::uniform_int_distribution<uint8_t> sat_dist(0, 255);
            std::uniform_int_distribution<uint8_t> light_dist(0, 255);

            HSL result;
            result.h = hue_dist(gen);
            result.s = sat_dist(gen);
            result.l = light_dist(gen);
            result.a = 255;
            return result;
        }
    };

    // Implementation of RGB conversion constructor
    inline RGB::RGB(const HSL& hsl) {
        RGB temp = hsl.to_rgb();
        r = temp.r;
        g = temp.g; 
        b = temp.b;
        a = temp.a;
    }

} // namespace pigment