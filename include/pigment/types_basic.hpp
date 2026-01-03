#pragma once

#include <datapod/matrix/vector.hpp>

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

    namespace dp = datapod;

    // Forward declarations
    struct HSL;
    struct HSV;
    struct LAB;
    struct MONO;
    struct XYZ;
    struct OKLAB;
    struct LCH;

    /**
     * @brief RGB color type built on datapod::mat::Vector<uint8_t, 4>
     *
     * Stores RGBA values as a 4-element vector with r, g, b, a components.
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct RGB : public dp::mat::Vector<uint8_t, 4> {
        using base_type = dp::mat::Vector<uint8_t, 4>;

        // Accessors for color components
        uint8_t &r() { return data_[0]; }
        uint8_t &g() { return data_[1]; }
        uint8_t &b() { return data_[2]; }
        uint8_t &a() { return data_[3]; }

        const uint8_t &r() const { return data_[0]; }
        const uint8_t &g() const { return data_[1]; }
        const uint8_t &b() const { return data_[2]; }
        const uint8_t &a() const { return data_[3]; }

        RGB() {
            data_[0] = 0;
            data_[1] = 0;
            data_[2] = 0;
            data_[3] = 255;
        }

        constexpr RGB(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) {
            data_[0] = r_;
            data_[1] = g_;
            data_[2] = b_;
            data_[3] = a_;
        }

        RGB(const std::string &color_str) {
            data_[3] = 255; // Default alpha

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
            data_[0] = std::stoi(h.substr(0, 2), nullptr, 16);
            data_[1] = std::stoi(h.substr(2, 2), nullptr, 16);
            data_[2] = std::stoi(h.substr(4, 2), nullptr, 16);
            data_[3] = std::stoi(h.substr(6, 2), nullptr, 16);
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

            data_[0] = static_cast<uint8_t>(std::clamp(std::stoi(parts[0]), 0, 255));
            data_[1] = static_cast<uint8_t>(std::clamp(std::stoi(parts[1]), 0, 255));
            data_[2] = static_cast<uint8_t>(std::clamp(std::stoi(parts[2]), 0, 255));
            data_[3] = parts.size() == 4
                           ? static_cast<uint8_t>(std::clamp(static_cast<int>(std::stod(parts[3]) * 255), 0, 255))
                           : 255;
        }

      public:
        RGB(const std::tuple<uint8_t, uint8_t, uint8_t> &rgb_tuple) {
            data_[0] = std::get<0>(rgb_tuple);
            data_[1] = std::get<1>(rgb_tuple);
            data_[2] = std::get<2>(rgb_tuple);
            data_[3] = 255;
        }

        // Implicit conversions from other color types
        RGB(const MONO &mono);   // Defined after MONO is complete
        RGB(const HSL &hsl);     // Defined after HSL is complete
        RGB(const HSV &hsv);     // Defined after HSV is complete
        RGB(const LAB &lab);     // Defined after LAB is complete
        RGB(const XYZ &xyz);     // Defined after XYZ is complete
        RGB(const OKLAB &oklab); // Defined after OKLAB is complete
        RGB(const LCH &lch);     // Defined after LCH is complete

        // Convert to hex string
        std::string to_hex(bool include_alpha = false) const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(r()) << std::setw(2)
               << static_cast<int>(g()) << std::setw(2) << static_cast<int>(b());
            if (include_alpha && a() != 255) {
                ss << std::setw(2) << static_cast<int>(a());
            }
            return ss.str();
        }

        // Arithmetic operations
        RGB operator+(const RGB &other) const {
            return RGB(std::clamp(static_cast<int>(r()) + other.r(), 0, 255),
                       std::clamp(static_cast<int>(g()) + other.g(), 0, 255),
                       std::clamp(static_cast<int>(b()) + other.b(), 0, 255),
                       std::clamp(static_cast<int>(a()) + other.a(), 0, 255));
        }

        RGB operator-(const RGB &other) const {
            return RGB(std::clamp(static_cast<int>(r()) - other.r(), 0, 255),
                       std::clamp(static_cast<int>(g()) - other.g(), 0, 255),
                       std::clamp(static_cast<int>(b()) - other.b(), 0, 255),
                       std::clamp(static_cast<int>(a()) - other.a(), 0, 255));
        }

        operator std::tuple<uint8_t, uint8_t, uint8_t>() const { return std::make_tuple(r(), g(), b()); }

        RGB operator*(double factor) const {
            return RGB(static_cast<uint8_t>(r() * factor > 255 ? 255 : (r() * factor < 0 ? 0 : r() * factor)),
                       static_cast<uint8_t>(g() * factor > 255 ? 255 : (g() * factor < 0 ? 0 : g() * factor)),
                       static_cast<uint8_t>(b() * factor > 255 ? 255 : (b() * factor < 0 ? 0 : b() * factor)), a());
        }

        RGB &operator+=(const RGB &other) {
            *this = *this + other;
            return *this;
        }

        RGB &operator*=(double factor) {
            *this = *this * factor;
            return *this;
        }

        bool operator==(const RGB &other) const {
            return r() == other.r() && g() == other.g() && b() == other.b() && a() == other.a();
        }

        bool operator!=(const RGB &other) const { return !(*this == other); }

        // Brightness adjustment
        RGB brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        RGB darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Color mixing
        RGB mix(const RGB &other, double ratio = 0.5) const {
            double clamped_ratio = ratio < 0.0 ? 0.0 : (ratio > 1.0 ? 1.0 : ratio);
            return RGB(static_cast<uint8_t>(r() * (1 - clamped_ratio) + other.r() * clamped_ratio),
                       static_cast<uint8_t>(g() * (1 - clamped_ratio) + other.g() * clamped_ratio),
                       static_cast<uint8_t>(b() * (1 - clamped_ratio) + other.b() * clamped_ratio),
                       static_cast<uint8_t>(a() * (1 - clamped_ratio) + other.a() * clamped_ratio));
        }

        // Blending modes
        RGB blend_add(const RGB &other) const {
            return RGB(r() + other.r() > 255 ? 255 : r() + other.r(), g() + other.g() > 255 ? 255 : g() + other.g(),
                       b() + other.b() > 255 ? 255 : b() + other.b(), a());
        }

        RGB blend_subtract(const RGB &other) const {
            return RGB(r() > other.r() ? r() - other.r() : 0, g() > other.g() ? g() - other.g() : 0,
                       b() > other.b() ? b() - other.b() : 0, a());
        }

        RGB blend_multiply(const RGB &other) const {
            return RGB(static_cast<uint8_t>((r() * other.r()) / 255), static_cast<uint8_t>((g() * other.g()) / 255),
                       static_cast<uint8_t>((b() * other.b()) / 255), a());
        }

        RGB blend_screen(const RGB &other) const {
            return RGB(static_cast<uint8_t>(255 - ((255 - r()) * (255 - other.r())) / 255),
                       static_cast<uint8_t>(255 - ((255 - g()) * (255 - other.g())) / 255),
                       static_cast<uint8_t>(255 - ((255 - b()) * (255 - other.b())) / 255), a());
        }

        RGB blend_overlay(const RGB &other) const {
            auto overlay_channel = [](uint8_t base, uint8_t blend) -> uint8_t {
                if (base < 128) {
                    return static_cast<uint8_t>((2 * base * blend) / 255);
                } else {
                    return static_cast<uint8_t>(255 - (2 * (255 - base) * (255 - blend)) / 255);
                }
            };
            return RGB(overlay_channel(r(), other.r()), overlay_channel(g(), other.g()),
                       overlay_channel(b(), other.b()), a());
        }

        // Alpha blending (proper alpha compositing)
        RGB alpha_blend(const RGB &background) const {
            if (a() == 255)
                return *this;
            if (a() == 0)
                return background;

            double alpha_fg = a() / 255.0;
            double alpha_bg = background.a() / 255.0;
            double alpha_out = alpha_fg + alpha_bg * (1.0 - alpha_fg);

            if (alpha_out == 0.0) {
                return RGB(0, 0, 0, 0);
            }

            uint8_t r_out =
                static_cast<uint8_t>((r() * alpha_fg + background.r() * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t g_out =
                static_cast<uint8_t>((g() * alpha_fg + background.g() * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t b_out =
                static_cast<uint8_t>((b() * alpha_fg + background.b() * alpha_bg * (1.0 - alpha_fg)) / alpha_out);
            uint8_t a_out = static_cast<uint8_t>(alpha_out * 255.0);

            return RGB(r_out, g_out, b_out, a_out);
        }

        // Simple alpha blend with background (assumes opaque background)
        RGB alpha_blend_simple(const RGB &background) const {
            if (a() == 255)
                return *this;
            if (a() == 0)
                return background;

            double alpha = a() / 255.0;
            double inv_alpha = 1.0 - alpha;

            return RGB(static_cast<uint8_t>(r() * alpha + background.r() * inv_alpha),
                       static_cast<uint8_t>(g() * alpha + background.g() * inv_alpha),
                       static_cast<uint8_t>(b() * alpha + background.b() * inv_alpha), 255);
        }

        // Set alpha channel
        RGB with_alpha(uint8_t new_alpha) const { return RGB(r(), g(), b(), new_alpha); }

        // Get transparency (inverse of alpha)
        double transparency() const { return 1.0 - (a() / 255.0); }

        // Check if color is transparent
        bool is_transparent() const { return a() < 255; }

        // Check if color is opaque
        bool is_opaque() const { return a() == 255; }

        // Luminance calculation (perceived brightness)
        double luminance() const { return 0.299 * r() + 0.587 * g() + 0.114 * b(); }

        bool is_dark() const { return luminance() < 128; }
        bool is_light() const { return luminance() >= 128; }

        // Color temperature adjustment
        RGB warm(double factor = 0.1) const {
            factor = std::clamp(factor, 0.0, 1.0);
            return RGB(std::clamp(static_cast<int>(r() + 255 * factor * 0.3), 0, 255),
                       std::clamp(static_cast<int>(g() + 255 * factor * 0.1), 0, 255), b(), a());
        }

        RGB cool(double factor = 0.1) const {
            factor = std::clamp(factor, 0.0, 1.0);
            return RGB(r(), std::clamp(static_cast<int>(g() + 255 * factor * 0.1), 0, 255),
                       std::clamp(static_cast<int>(b() + 255 * factor * 0.3), 0, 255), a());
        }

        // Grayscale conversion
        RGB to_grayscale() const {
            uint8_t gray = static_cast<uint8_t>(luminance());
            return RGB(gray, gray, gray, a());
        }

        // Invert color
        RGB invert() const { return RGB(255 - r(), 255 - g(), 255 - b(), a()); }

        // Gamma correction utilities
        RGB apply_gamma(double gamma = 2.2) const {
            auto gamma_correct = [gamma](uint8_t val) -> uint8_t {
                double normalized = val / 255.0;
                double corrected = std::pow(normalized, 1.0 / gamma);
                return static_cast<uint8_t>(std::clamp(corrected * 255.0, 0.0, 255.0));
            };
            return RGB(gamma_correct(r()), gamma_correct(g()), gamma_correct(b()), a());
        }

        RGB remove_gamma(double gamma = 2.2) const {
            auto gamma_remove = [gamma](uint8_t val) -> uint8_t {
                double normalized = val / 255.0;
                double linear = std::pow(normalized, gamma);
                return static_cast<uint8_t>(std::clamp(linear * 255.0, 0.0, 255.0));
            };
            return RGB(gamma_remove(r()), gamma_remove(g()), gamma_remove(b()), a());
        }

        // Contrast adjustment
        RGB adjust_contrast(double contrast) const {
            contrast = std::clamp(contrast, -1.0, 1.0);
            double factor = (259.0 * (contrast * 255.0 + 255.0)) / (255.0 * (259.0 - contrast * 255.0));

            return RGB(std::clamp(static_cast<int>(factor * (r() - 128) + 128), 0, 255),
                       std::clamp(static_cast<int>(factor * (g() - 128) + 128), 0, 255),
                       std::clamp(static_cast<int>(factor * (b() - 128) + 128), 0, 255), a());
        }

        static RGB random() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 255);
            return RGB(dist(gen), dist(gen), dist(gen), 255);
        }

        // Predefined colors
        static RGB black() { return RGB(0, 0, 0); }
        static RGB white() { return RGB(255, 255, 255); }
        static RGB red() { return RGB(255, 0, 0); }
        static RGB green() { return RGB(0, 255, 0); }
        static RGB blue() { return RGB(0, 0, 255); }
        static RGB yellow() { return RGB(255, 255, 0); }
        static RGB cyan() { return RGB(0, 255, 255); }
        static RGB magenta() { return RGB(255, 0, 255); }
        static RGB transparent() { return RGB(0, 0, 0, 0); }
    };

    /**
     * @brief Monochrome color type built on datapod::mat::Vector<uint8_t, 2>
     *
     * Stores grayscale value and alpha as a 2-element vector.
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct MONO : public dp::mat::Vector<uint8_t, 2> {
        using base_type = dp::mat::Vector<uint8_t, 2>;

        // Accessors for components
        uint8_t &v() { return data_[0]; }
        uint8_t &a() { return data_[1]; }

        const uint8_t &v() const { return data_[0]; }
        const uint8_t &a() const { return data_[1]; }

        MONO() {
            data_[0] = 0;
            data_[1] = 255;
        }

        constexpr MONO(uint8_t v_, uint8_t a_ = 255) {
            data_[0] = v_;
            data_[1] = a_;
        }

        // Convert from RGB using luminance
        MONO(const RGB &rgb) {
            data_[0] = static_cast<uint8_t>(rgb.luminance());
            data_[1] = rgb.a();
        }

        // Convert to RGB
        RGB to_rgb() const { return RGB(v(), v(), v(), a()); }

        // Arithmetic operations
        MONO operator+(const MONO &other) const {
            return MONO(std::clamp(static_cast<int>(v()) + other.v(), 0, 255), a());
        }

        MONO operator-(const MONO &other) const {
            return MONO(std::clamp(static_cast<int>(v()) - other.v(), 0, 255), a());
        }

        MONO operator*(double factor) const {
            return MONO(static_cast<uint8_t>(v() * factor > 255 ? 255 : (v() * factor < 0 ? 0 : v() * factor)), a());
        }

        bool operator==(const MONO &other) const { return v() == other.v() && a() == other.a(); }

        bool operator!=(const MONO &other) const { return !(*this == other); }

        bool operator<(const MONO &other) const { return v() < other.v(); }

        // Brightness adjustment
        MONO brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        MONO darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Invert
        MONO invert() const { return MONO(255 - v(), a()); }

        // Mix with another monochrome color
        MONO mix(const MONO &other, double ratio = 0.5) const {
            double clamped_ratio = ratio < 0.0 ? 0.0 : (ratio > 1.0 ? 1.0 : ratio);
            return MONO(static_cast<uint8_t>(v() * (1 - clamped_ratio) + other.v() * clamped_ratio),
                        static_cast<uint8_t>(a() * (1 - clamped_ratio) + other.a() * clamped_ratio));
        }

        // Convert to hex string
        std::string to_hex() const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(v());
            return ss.str();
        }

        static MONO random() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, 255);
            return MONO(dist(gen), 255);
        }

        // Predefined values
        static MONO black() { return MONO(0); }
        static MONO white() { return MONO(255); }
        static MONO gray() { return MONO(128); }
    };

    // Implementation of RGB conversion constructor for MONO
    inline RGB::RGB(const MONO &mono) {
        data_[0] = mono.v();
        data_[1] = mono.v();
        data_[2] = mono.v();
        data_[3] = mono.a();
    }

} // namespace pigment
