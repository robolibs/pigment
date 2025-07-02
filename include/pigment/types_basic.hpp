#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace pigment {

    struct RGB {
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;

        RGB() = default;
        RGB(int r_, int g_, int b_, int a_ = 255) : r(r_), g(g_), b(b_), a(a_) {}

        RGB(const std::string &hex) {
            std::string h = hex;
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
                throw std::invalid_argument("Invalid hex color: '" + hex + "'");
            }
            r = std::stoi(h.substr(0, 2), nullptr, 16);
            g = std::stoi(h.substr(2, 2), nullptr, 16);
            b = std::stoi(h.substr(4, 2), nullptr, 16);
            a = std::stoi(h.substr(6, 2), nullptr, 16);
        }

        RGB(const std::tuple<int, int, int> &rgb_tuple)
            : r(std::get<0>(rgb_tuple)), g(std::get<1>(rgb_tuple)), b(std::get<2>(rgb_tuple)), a(255) {}

        // Convert to hex string
        std::string to_hex(bool include_alpha = false) const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
            if (include_alpha && a != 255) {
                ss << std::setw(2) << a;
            }
            return ss.str();
        }

        // Arithmetic operations
        RGB operator+(const RGB &other) const {
            return RGB(std::clamp(r + other.r, 0, 255), std::clamp(g + other.g, 0, 255),
                       std::clamp(b + other.b, 0, 255), std::clamp(a + other.a, 0, 255));
        }

        RGB operator-(const RGB &other) const {
            return RGB(std::clamp(r - other.r, 0, 255), std::clamp(g - other.g, 0, 255),
                       std::clamp(b - other.b, 0, 255), std::clamp(a - other.a, 0, 255));
        }

        operator std::tuple<int, int, int>() const { return std::make_tuple(r, g, b); }

        RGB operator*(double factor) const {
            return RGB(std::clamp(static_cast<int>(r * factor), 0, 255),
                       std::clamp(static_cast<int>(g * factor), 0, 255),
                       std::clamp(static_cast<int>(b * factor), 0, 255), a);
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
        RGB brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        RGB darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Color mixing
        RGB mix(const RGB &other, double ratio = 0.5) const {
            ratio = std::clamp(ratio, 0.0, 1.0);
            return RGB(static_cast<int>(r * (1 - ratio) + other.r * ratio),
                       static_cast<int>(g * (1 - ratio) + other.g * ratio),
                       static_cast<int>(b * (1 - ratio) + other.b * ratio),
                       static_cast<int>(a * (1 - ratio) + other.a * ratio));
        }

        // Luminance calculation (perceived brightness)
        double luminance() const { return 0.299 * r + 0.587 * g + 0.114 * b; }

        bool is_dark() const { return luminance() < 128; }
        bool is_light() const { return luminance() >= 128; }

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
            int gray = static_cast<int>(luminance());
            return RGB(gray, gray, gray, a);
        }

        // Invert color
        RGB invert() const { return RGB(255 - r, 255 - g, 255 - b, a); }

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

    struct MONO {
        int v = 0;
        int a = 255;

        MONO() = default;
        MONO(int v_, int a_ = 255) : v(std::clamp(v_, 0, 255)), a(std::clamp(a_, 0, 255)) {}

        // Convert from RGB using luminance
        MONO(const RGB &rgb) : v(static_cast<int>(rgb.luminance())), a(rgb.a) {}

        // Convert to RGB
        RGB to_rgb() const { return RGB(v, v, v, a); }

        // Arithmetic operations
        MONO operator+(const MONO &other) const { return MONO(std::clamp(v + other.v, 0, 255), a); }

        MONO operator-(const MONO &other) const { return MONO(std::clamp(v - other.v, 0, 255), a); }

        MONO operator*(double factor) const { return MONO(std::clamp(static_cast<int>(v * factor), 0, 255), a); }

        bool operator==(const MONO &other) const { return v == other.v && a == other.a; }

        bool operator!=(const MONO &other) const { return !(*this == other); }

        bool operator<(const MONO &other) const { return v < other.v; }

        // Brightness adjustment
        MONO brighten(double factor = 0.2) const { return *this * (1.0 + factor); }

        MONO darken(double factor = 0.2) const { return *this * (1.0 - factor); }

        // Invert
        MONO invert() const { return MONO(255 - v, a); }

        // Mix with another monochrome color
        MONO mix(const MONO &other, double ratio = 0.5) const {
            ratio = std::clamp(ratio, 0.0, 1.0);
            return MONO(static_cast<int>(v * (1 - ratio) + other.v * ratio),
                        static_cast<int>(a * (1 - ratio) + other.a * ratio));
        }

        // Convert to hex string
        std::string to_hex() const {
            std::stringstream ss;
            ss << "#" << std::hex << std::setfill('0') << std::setw(2) << v;
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
} // namespace pigment
