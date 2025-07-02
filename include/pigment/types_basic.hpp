#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace pigment {

    // Forward declarations
    struct HSL;
    struct HSV;
    struct LAB;
    struct MONO;
    struct XYZ;
    struct OKLAB;
    struct LCH;

    struct RGB {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        RGB() = default;
        constexpr RGB(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) : r(r_), g(g_), b(b_), a(a_) {}

        RGB(const std::string &color_str) {
            if (color_str.empty()) {
                throw std::invalid_argument("Empty color string");
            }
            
            // Check if it's a CSS rgb() or rgba() function
            if (color_str.substr(0, 4) == "rgb(" || color_str.substr(0, 5) == "rgba(") {
                parse_css_rgb(color_str);
                return;
            }
            
            // Otherwise treat as hex
            std::string h = color_str;
            if (!h.empty() && h[0] == '#') {
                h.erase(0, 1);
            }
            if (h.size() == 3) {
                std::string tmp;
                tmp.reserve(6);
                for (char c : h) {
                    tmp.push_back(c);
                    tmp.push_back(c);
                }
                h = tmp;
            }
            if (h.size() == 6) {
                h += "ff";
            }
            if (h.size() != 8) {
                throw std::invalid_argument("Invalid hex color: '" + color_str + "'");
            }
            r = std::stoi(h.substr(0, 2), nullptr, 16);
            g = std::stoi(h.substr(2, 2), nullptr, 16);
            b = std::stoi(h.substr(4, 2), nullptr, 16);
            a = std::stoi(h.substr(6, 2), nullptr, 16);
        }

    private:
        void parse_css_rgb(const std::string &css_str) {
            // Remove spaces and find the parentheses
            std::string clean = css_str;
            clean.erase(std::remove(clean.begin(), clean.end(), ' '), clean.end());
            
            size_t start = clean.find('(');
            size_t end = clean.find(')', start);
            
            if (start == std::string::npos || end == std::string::npos) {
                throw std::invalid_argument("Invalid CSS color format");
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
                throw std::invalid_argument("Invalid number of RGB components");
            }
            
            r = static_cast<uint8_t>(std::clamp(std::stoi(parts[0]), 0, 255));
            g = static_cast<uint8_t>(std::clamp(std::stoi(parts[1]), 0, 255));
            b = static_cast<uint8_t>(std::clamp(std::stoi(parts[2]), 0, 255));
            a = parts.size() == 4 ? static_cast<uint8_t>(std::clamp(static_cast<int>(std::stod(parts[3]) * 255), 0, 255)) : 255;
        }

    public:

        RGB(const std::tuple<uint8_t, uint8_t, uint8_t> &rgb_tuple)
            : r(std::get<0>(rgb_tuple)), g(std::get<1>(rgb_tuple)), b(std::get<2>(rgb_tuple)), a(255) {}

        // Implicit conversions from other color types
        RGB(const MONO& mono);  // Defined after MONO is complete
        RGB(const HSL& hsl);    // Defined after HSL is complete
        RGB(const HSV& hsv);    // Defined after HSV is complete  
        RGB(const LAB& lab);    // Defined after LAB is complete
        RGB(const XYZ& xyz);    // Defined after XYZ is complete
        RGB(const OKLAB& oklab); // Defined after OKLAB is complete
        RGB(const LCH& lch);    // Defined after LCH is complete

        // Convert to hex string
        std::string to_hex(bool include_alpha = false) const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') 
               << std::setw(2) << static_cast<int>(r) 
               << std::setw(2) << static_cast<int>(g) 
               << std::setw(2) << static_cast<int>(b);
            if (include_alpha && a != 255) {
                ss << std::setw(2) << static_cast<int>(a);
            }
            return ss.str();
        }

        // Arithmetic operations
        RGB operator+(const RGB &other) const {
            return RGB(std::clamp(static_cast<int>(r) + other.r, 0, 255), 
                       std::clamp(static_cast<int>(g) + other.g, 0, 255),
                       std::clamp(static_cast<int>(b) + other.b, 0, 255), 
                       std::clamp(static_cast<int>(a) + other.a, 0, 255));
        }

        RGB operator-(const RGB &other) const {
            return RGB(std::clamp(static_cast<int>(r) - other.r, 0, 255), 
                       std::clamp(static_cast<int>(g) - other.g, 0, 255),
                       std::clamp(static_cast<int>(b) - other.b, 0, 255), 
                       std::clamp(static_cast<int>(a) - other.a, 0, 255));
        }

        operator std::tuple<uint8_t, uint8_t, uint8_t>() const { return std::make_tuple(r, g, b); }

        constexpr RGB operator*(double factor) const {
            return RGB(static_cast<uint8_t>(r * factor > 255 ? 255 : (r * factor < 0 ? 0 : r * factor)),
                       static_cast<uint8_t>(g * factor > 255 ? 255 : (g * factor < 0 ? 0 : g * factor)),
                       static_cast<uint8_t>(b * factor > 255 ? 255 : (b * factor < 0 ? 0 : b * factor)), a);
        }

        RGB &operator+=(const RGB &other) {
            *this = *this + other;
            return *this;
        }

        RGB &operator*=(double factor) {
            *this = *this * factor;
            return *this;
        }

        bool operator==(const RGB &other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }

        bool operator!=(const RGB &other) const { return !(*this == other); }

        // Brightness adjustment
        constexpr RGB brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        constexpr RGB darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Color mixing
        constexpr RGB mix(const RGB &other, double ratio = 0.5) const {
            double clamped_ratio = ratio < 0.0 ? 0.0 : (ratio > 1.0 ? 1.0 : ratio);
            return RGB(static_cast<uint8_t>(r * (1 - clamped_ratio) + other.r * clamped_ratio),
                       static_cast<uint8_t>(g * (1 - clamped_ratio) + other.g * clamped_ratio),
                       static_cast<uint8_t>(b * (1 - clamped_ratio) + other.b * clamped_ratio),
                       static_cast<uint8_t>(a * (1 - clamped_ratio) + other.a * clamped_ratio));
        }

        // Blending modes
        constexpr RGB blend_add(const RGB &other) const {
            return RGB(r + other.r > 255 ? 255 : r + other.r,
                       g + other.g > 255 ? 255 : g + other.g,
                       b + other.b > 255 ? 255 : b + other.b, a);
        }

        constexpr RGB blend_subtract(const RGB &other) const {
            return RGB(r > other.r ? r - other.r : 0,
                       g > other.g ? g - other.g : 0,
                       b > other.b ? b - other.b : 0, a);
        }

        constexpr RGB blend_multiply(const RGB &other) const {
            return RGB(static_cast<uint8_t>((r * other.r) / 255),
                       static_cast<uint8_t>((g * other.g) / 255),
                       static_cast<uint8_t>((b * other.b) / 255), a);
        }

        constexpr RGB blend_screen(const RGB &other) const {
            return RGB(static_cast<uint8_t>(255 - ((255 - r) * (255 - other.r)) / 255),
                       static_cast<uint8_t>(255 - ((255 - g) * (255 - other.g)) / 255),
                       static_cast<uint8_t>(255 - ((255 - b) * (255 - other.b)) / 255), a);
        }

        constexpr RGB blend_overlay(const RGB &other) const {
            auto overlay_channel = [](uint8_t base, uint8_t blend) -> uint8_t {
                if (base < 128) {
                    return static_cast<uint8_t>((2 * base * blend) / 255);
                } else {
                    return static_cast<uint8_t>(255 - (2 * (255 - base) * (255 - blend)) / 255);
                }
            };
            return RGB(overlay_channel(r, other.r),
                       overlay_channel(g, other.g),
                       overlay_channel(b, other.b), a);
        }

        // Alpha blending (proper alpha compositing)
        RGB alpha_blend(const RGB &background) const {
            if (a == 255) return *this;
            if (a == 0) return background;
            
            double alpha_fg = a / 255.0;
            double alpha_bg = background.a / 255.0;
            double alpha_out = alpha_fg + alpha_bg * (1.0 - alpha_fg);
            
            if (alpha_out == 0.0) {
                return RGB(0, 0, 0, 0);
            }
            
            uint8_t r_out = static_cast<uint8_t>((r * alpha_fg + background.r * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t g_out = static_cast<uint8_t>((g * alpha_fg + background.g * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t b_out = static_cast<uint8_t>((b * alpha_fg + background.b * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t a_out = static_cast<uint8_t>(alpha_out * 255.0);
            
            return RGB(r_out, g_out, b_out, a_out);
        }

        // Simple alpha blend with background (assumes opaque background)
        RGB alpha_blend_simple(const RGB &background) const {
            if (a == 255) return *this;
            if (a == 0) return background;
            
            double alpha = a / 255.0;
            double inv_alpha = 1.0 - alpha;
            
            return RGB(static_cast<uint8_t>(r * alpha + background.r * inv_alpha),
                       static_cast<uint8_t>(g * alpha + background.g * inv_alpha),
                       static_cast<uint8_t>(b * alpha + background.b * inv_alpha),
                       255);
        }

        // Set alpha channel
        constexpr RGB with_alpha(uint8_t new_alpha) const {
            return RGB(r, g, b, new_alpha);
        }

        // Get transparency (inverse of alpha)
        constexpr double transparency() const {
            return 1.0 - (a / 255.0);
        }

        // Check if color is transparent
        constexpr bool is_transparent() const {
            return a < 255;
        }

        // Check if color is opaque
        constexpr bool is_opaque() const {
            return a == 255;
        }

        // Luminance calculation (perceived brightness)
        constexpr double luminance() const { return 0.299 * r + 0.587 * g + 0.114 * b; }

        constexpr bool is_dark() const { return luminance() < 128; }
        constexpr bool is_light() const { return luminance() >= 128; }

        // Color temperature adjustment
        RGB warm(double factor = 0.1) const {
            factor = std::clamp(factor, 0.0, 1.0);
            return RGB(std::clamp(static_cast<int>(r + 255 * factor * 0.3), 0, 255),
                       std::clamp(static_cast<int>(g + 255 * factor * 0.1), 0, 255), b, a);
        }

        RGB cool(double factor = 0.1) const {
            factor = std::clamp(factor, 0.0, 1.0);
            return RGB(r, std::clamp(static_cast<int>(g + 255 * factor * 0.1), 0, 255),
                       std::clamp(static_cast<int>(b + 255 * factor * 0.3), 0, 255), a);
        }

        // Grayscale conversion
        RGB to_grayscale() const {
            uint8_t gray = static_cast<uint8_t>(luminance());
            return RGB(gray, gray, gray, a);
        }

        // Invert color
        constexpr RGB invert() const { return RGB(255 - r, 255 - g, 255 - b, a); }

        // Gamma correction utilities
        RGB apply_gamma(double gamma = 2.2) const {
            auto gamma_correct = [gamma](uint8_t val) -> uint8_t {
                double normalized = val / 255.0;
                double corrected = std::pow(normalized, 1.0 / gamma);
                return static_cast<uint8_t>(std::clamp(corrected * 255.0, 0.0, 255.0));
            };
            return RGB(gamma_correct(r), gamma_correct(g), gamma_correct(b), a);
        }

        RGB remove_gamma(double gamma = 2.2) const {
            auto gamma_remove = [gamma](uint8_t val) -> uint8_t {
                double normalized = val / 255.0;
                double linear = std::pow(normalized, gamma);
                return static_cast<uint8_t>(std::clamp(linear * 255.0, 0.0, 255.0));
            };
            return RGB(gamma_remove(r), gamma_remove(g), gamma_remove(b), a);
        }

        // Contrast adjustment
        RGB adjust_contrast(double contrast) const {
            contrast = std::clamp(contrast, -1.0, 1.0);
            double factor = (259.0 * (contrast * 255.0 + 255.0)) / (255.0 * (259.0 - contrast * 255.0));

            return RGB(std::clamp(static_cast<int>(factor * (r - 128) + 128), 0, 255),
                       std::clamp(static_cast<int>(factor * (g - 128) + 128), 0, 255),
                       std::clamp(static_cast<int>(factor * (b - 128) + 128), 0, 255), a);
        }

        static RGB random() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 255);
            return RGB(dist(gen), dist(gen), dist(gen), 255);
        }

        // Predefined colors
        static constexpr RGB black() { return RGB(0, 0, 0); }
        static constexpr RGB white() { return RGB(255, 255, 255); }
        static constexpr RGB red() { return RGB(255, 0, 0); }
        static constexpr RGB green() { return RGB(0, 255, 0); }
        static constexpr RGB blue() { return RGB(0, 0, 255); }
        static constexpr RGB yellow() { return RGB(255, 255, 0); }
        static constexpr RGB cyan() { return RGB(0, 255, 255); }
        static constexpr RGB magenta() { return RGB(255, 0, 255); }
        static constexpr RGB transparent() { return RGB(0, 0, 0, 0); }
    };

    struct MONO {
        uint8_t v = 0;
        uint8_t a = 255;

        MONO() = default;
        constexpr MONO(uint8_t v_, uint8_t a_ = 255) : v(v_), a(a_) {}

        // Convert from RGB using luminance
        MONO(const RGB &rgb) : v(static_cast<uint8_t>(rgb.luminance())), a(rgb.a) {}

        // Convert to RGB
        RGB to_rgb() const { return RGB(v, v, v, a); }

        // Arithmetic operations
        MONO operator+(const MONO &other) const { return MONO(std::clamp(static_cast<int>(v) + other.v, 0, 255), a); }

        MONO operator-(const MONO &other) const { return MONO(std::clamp(static_cast<int>(v) - other.v, 0, 255), a); }

        constexpr MONO operator*(double factor) const { 
            return MONO(static_cast<uint8_t>(v * factor > 255 ? 255 : (v * factor < 0 ? 0 : v * factor)), a); 
        }

        bool operator==(const MONO &other) const { return v == other.v && a == other.a; }

        bool operator!=(const MONO &other) const { return !(*this == other); }

        bool operator<(const MONO &other) const { return v < other.v; }

        // Brightness adjustment
        constexpr MONO brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        constexpr MONO darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Invert
        constexpr MONO invert() const { return MONO(255 - v, a); }

        // Mix with another monochrome color
        constexpr MONO mix(const MONO &other, double ratio = 0.5) const {
            double clamped_ratio = ratio < 0.0 ? 0.0 : (ratio > 1.0 ? 1.0 : ratio);
            return MONO(static_cast<uint8_t>(v * (1 - clamped_ratio) + other.v * clamped_ratio),
                        static_cast<uint8_t>(a * (1 - clamped_ratio) + other.a * clamped_ratio));
        }

        // Convert to hex string
        std::string to_hex() const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(v);
            return ss.str();
        }

        static MONO random() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 255);
            return MONO(dist(gen), 255);
        }

        // Predefined values
        static constexpr MONO black() { return MONO(0); }
        static constexpr MONO white() { return MONO(255); }
        static constexpr MONO gray() { return MONO(128); }
    };

    // Implementation of RGB conversion constructor for MONO
    inline RGB::RGB(const MONO& mono) : r(mono.v), g(mono.v), b(mono.v), a(mono.a) {}

} // namespace pigment
