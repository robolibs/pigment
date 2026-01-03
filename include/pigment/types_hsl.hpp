#pragma once

#include "types_basic.hpp"

#include <datapod/matrix/vector.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

namespace pigment {

    /**
     * @brief HSL color type built on datapod::mat::Vector
     *
     * Stores Hue (0-36000, representing 0.0-360.0 degrees * 100),
     * Saturation (0-255), Lightness (0-255), and Alpha (0-255).
     *
     * Uses a mixed storage approach:
     * - hue as uint16_t for precision
     * - s, l, alpha as uint8_t for compact storage
     */
    struct HSL {
        uint16_t h = 0;      // 0-36000 (representing 0.0-360.0 degrees * 100)
        uint8_t s = 0;       // 0-255 (representing 0.0-1.0 saturation)
        uint8_t l = 0;       // 0-255 (representing 0.0-1.0 lightness)
        uint8_t alpha = 255; // 0-255 alpha

        // Serialization support
        auto members() noexcept { return std::tie(h, s, l, alpha); }
        auto members() const noexcept { return std::tie(h, s, l, alpha); }

        HSL() = default;
        HSL(double h_, double s_, double l_, uint8_t a_ = 255)
            : s(static_cast<uint8_t>(std::clamp(s_, 0.0, 1.0) * 255)),
              l(static_cast<uint8_t>(std::clamp(l_, 0.0, 1.0) * 255)), alpha(a_) {
            // Normalize hue to [0, 360) before converting to integer
            double normalized_h = std::fmod(h_, 360.0);
            if (normalized_h < 0)
                normalized_h += 360.0;
            h = static_cast<uint16_t>(normalized_h * 100);
            normalize();
        }

        // CSS HSL string constructor
        HSL(const std::string &hsl_str) {
            if (hsl_str.empty()) {
                throw std::invalid_argument("Empty HSL string");
            }

            // Check if it's a CSS hsl() or hsla() function
            if (hsl_str.substr(0, 4) == "hsl(" || hsl_str.substr(0, 5) == "hsla(") {
                parse_css_hsl(hsl_str);
            } else {
                throw std::invalid_argument("Invalid HSL format. Use hsl(h,s%,l%) or hsla(h,s%,l%,a)");
            }
        }

      private:
        void parse_css_hsl(const std::string &css_str) {
            // Remove spaces and find the parentheses
            std::string clean = css_str;
            clean.erase(std::remove(clean.begin(), clean.end(), ' '), clean.end());

            size_t start = clean.find('(');
            size_t end = clean.find(')', start);

            if (start == std::string::npos || end == std::string::npos) {
                throw std::invalid_argument("Invalid CSS HSL format");
            }

            std::string values = clean.substr(start + 1, end - start - 1);

            // Split by commas
            std::vector<std::string> parts;
            size_t pos = 0;
            while (pos < values.length()) {
                size_t comma = values.find(',', pos);
                if (comma == std::string::npos) {
                    parts.push_back(values.substr(pos));
                    break;
                }
                parts.push_back(values.substr(pos, comma - pos));
                pos = comma + 1;
            }

            if (parts.size() < 3 || parts.size() > 4) {
                throw std::invalid_argument("Invalid number of HSL components");
            }

            // Parse hue (0-360)
            double hue = std::stod(parts[0]);
            double normalized_h = std::fmod(hue, 360.0);
            if (normalized_h < 0)
                normalized_h += 360.0;
            h = static_cast<uint16_t>(normalized_h * 100);

            // Parse saturation (remove % if present)
            std::string sat_str = parts[1];
            if (!sat_str.empty() && sat_str.back() == '%') {
                sat_str.pop_back();
            }
            double saturation = std::stod(sat_str) / 100.0;
            s = static_cast<uint8_t>(std::clamp(saturation, 0.0, 1.0) * 255);

            // Parse lightness (remove % if present)
            std::string light_str = parts[2];
            if (!light_str.empty() && light_str.back() == '%') {
                light_str.pop_back();
            }
            double lightness = std::stod(light_str) / 100.0;
            l = static_cast<uint8_t>(std::clamp(lightness, 0.0, 1.0) * 255);

            // Parse alpha if present
            alpha = parts.size() == 4 ? static_cast<uint8_t>(std::clamp(std::stod(parts[3]), 0.0, 1.0) * 255) : 255;

            normalize();
        }

      public:
        // Getters for compatibility with tests expecting double values
        double get_h() const { return h / 100.0; }
        double get_s() const { return s / 255.0; }
        double get_l() const { return l / 255.0; }

        // Comparison operators for backward compatibility
        bool operator==(double hue) const { return std::abs(get_h() - hue) < 0.1; }
        friend bool operator==(double val, const HSL &hsl) { return hsl == val; }

        void normalize() {
            // Wrap hue to [0, 36000) (0.0-360.0 degrees * 100)
            // Handle negative values properly
            h = ((h % 36000) + 36000) % 36000;

            // Saturation and lightness are already clamped by uint8_t range
            s = std::clamp(static_cast<int>(s), 0, 255);
            l = std::clamp(static_cast<int>(l), 0, 255);
            alpha = std::clamp(static_cast<int>(alpha), 0, 255);
        }

        // Convert from RGB
        static HSL fromRGB(const RGB &rgb) {
            double r_val = rgb.r() / 255.0;
            double g_val = rgb.g() / 255.0;
            double b_val = rgb.b() / 255.0;

            double max_val = std::max({r_val, g_val, b_val});
            double min_val = std::min({r_val, g_val, b_val});
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
                if (max_val == r_val) {
                    hue = (g_val - b_val) / delta + (g_val < b_val ? 6 : 0);
                } else if (max_val == g_val) {
                    hue = (b_val - r_val) / delta + 2;
                } else {
                    hue = (r_val - g_val) / delta + 4;
                }
                hue /= 6;
                hue *= 360;
                hsl.h = static_cast<uint16_t>(hue * 100);
            }
            hsl.alpha = rgb.a();
            hsl.normalize();

            return hsl;
        }

        // Convert to RGB
        RGB to_rgb() const {
            double l_norm = l / 255.0;
            double s_norm = s / 255.0;

            if (s == 0) {
                return RGB(l, l, l, alpha);
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

            double r_val = hue_to_rgb(p, q, h_norm + 1.0 / 3);
            double g_val = hue_to_rgb(p, q, h_norm);
            double b_val = hue_to_rgb(p, q, h_norm - 1.0 / 3);

            return RGB(static_cast<uint8_t>(std::round(r_val * 255)), static_cast<uint8_t>(std::round(g_val * 255)),
                       static_cast<uint8_t>(std::round(b_val * 255)), alpha);
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
            result.alpha = 255;
            return result;
        }
    };

    // Implementation of RGB conversion constructor
    inline RGB::RGB(const HSL &hsl) {
        RGB temp = hsl.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
