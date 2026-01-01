#pragma once

#include "types_basic.hpp"
#include "types_hsl.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

namespace pigment {

    class Palette {
      private:
        std::vector<RGB> colors_;

      public:
        Palette() = default;
        Palette(const std::vector<RGB> &colors) : colors_(colors) {}
        Palette(std::initializer_list<RGB> colors) : colors_(colors) {}

        // Add colors
        void add(const RGB &color) { colors_.push_back(color); }

        void add(const std::vector<RGB> &colors) { colors_.insert(colors_.end(), colors.begin(), colors.end()); }

        // Access colors
        RGB &operator[](size_t index) { return colors_[index % colors_.size()]; }

        const RGB &operator[](size_t index) const { return colors_[index % colors_.size()]; }

        size_t size() const { return colors_.size(); }

        bool empty() const { return colors_.empty(); }

        void clear() { colors_.clear(); }

        // Iterator support
        auto begin() { return colors_.begin(); }
        auto end() { return colors_.end(); }
        auto begin() const { return colors_.begin(); }
        auto end() const { return colors_.end(); }

        // Get random color from palette
        RGB random() const {
            if (colors_.empty())
                return RGB::black();

            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> dist(0, colors_.size() - 1);
            return colors_[dist(gen)];
        }

        // Create gradient between two colors
        static Palette gradient(const RGB &start, const RGB &end, size_t steps) {
            std::vector<RGB> colors;
            colors.reserve(steps);

            for (size_t i = 0; i < steps; ++i) {
                double ratio = static_cast<double>(i) / (steps - 1);
                colors.push_back(start.mix(end, ratio));
            }

            return Palette(colors);
        }

        // Create multi-color gradient
        static Palette gradient(const std::vector<RGB> &colors, size_t steps_per_segment) {
            if (colors.size() < 2)
                return Palette();

            Palette result;
            for (size_t i = 0; i < colors.size() - 1; ++i) {
                auto segment = gradient(colors[i], colors[i + 1], steps_per_segment);
                result.add(segment.colors_);
            }

            return result;
        }

        // Predefined palettes
        static Palette material_design() {
            return Palette({
                RGB("#F44336"), // Red
                RGB("#E91E63"), // Pink
                RGB("#9C27B0"), // Purple
                RGB("#673AB7"), // Deep Purple
                RGB("#3F51B5"), // Indigo
                RGB("#2196F3"), // Blue
                RGB("#03A9F4"), // Light Blue
                RGB("#00BCD4"), // Cyan
                RGB("#009688"), // Teal
                RGB("#4CAF50"), // Green
                RGB("#8BC34A"), // Light Green
                RGB("#CDDC39"), // Lime
                RGB("#FFEB3B"), // Yellow
                RGB("#FFC107"), // Amber
                RGB("#FF9800"), // Orange
                RGB("#FF5722")  // Deep Orange
            });
        }

        static Palette warm() {
            return Palette({
                RGB("#FF6B6B"), // Red
                RGB("#FFE66D"), // Yellow
                RGB("#FF8E53"), // Orange
                RGB("#FF6F91"), // Pink
                RGB("#C44569")  // Deep Pink
            });
        }

        static Palette cool() {
            return Palette({
                RGB("#4ECDC4"), // Teal
                RGB("#45B7D1"), // Blue
                RGB("#96CEB4"), // Green
                RGB("#FFEAA7"), // Light Yellow
                RGB("#DDA0DD")  // Plum
            });
        }

        static Palette monochromatic(const RGB &base, size_t count = 5) {
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors;

            for (size_t i = 0; i < count; ++i) {
                double lightness = 0.2 + (0.6 * i / (count - 1));
                colors.push_back(HSL(hsl.get_h(), hsl.get_s(), lightness, hsl.alpha).to_rgb());
            }

            return Palette(colors);
        }

        static Palette analogous(const RGB &base, size_t count = 5, double range = 60.0) {
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors;

            double step = range / (count - 1);
            double start_hue = hsl.get_h() - range / 2.0;

            for (size_t i = 0; i < count; ++i) {
                double hue = start_hue + (step * i);
                colors.push_back(HSL(hue, hsl.get_s(), hsl.get_l(), hsl.alpha).to_rgb());
            }

            return Palette(colors);
        }

        static Palette complementary(const RGB &base) {
            HSL hsl = HSL::fromRGB(base);
            return Palette({base, hsl.complement().to_rgb()});
        }

        static Palette triadic(const RGB &base) {
            HSL hsl = HSL::fromRGB(base);
            auto triadic_colors = hsl.triadic();
            std::vector<RGB> colors;

            for (const auto &color : triadic_colors) {
                colors.push_back(color.to_rgb());
            }

            return Palette(colors);
        }

        static Palette pastel(size_t count = 8) {
            std::vector<RGB> colors;
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<double> hue_dist(0.0, 360.0);

            for (size_t i = 0; i < count; ++i) {
                HSL hsl(hue_dist(gen), 0.3, 0.8); // Low saturation, high lightness
                colors.push_back(hsl.to_rgb());
            }

            return Palette(colors);
        }

        static Palette vibrant(size_t count = 8) {
            std::vector<RGB> colors;
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<double> hue_dist(0.0, 360.0);

            for (size_t i = 0; i < count; ++i) {
                HSL hsl(hue_dist(gen), 0.8, 0.5); // High saturation, medium lightness
                colors.push_back(hsl.to_rgb());
            }

            return Palette(colors);
        }

        // Export to hex strings
        std::vector<std::string> to_hex() const {
            std::vector<std::string> hex_colors;
            hex_colors.reserve(colors_.size());

            for (const auto &color : colors_) {
                hex_colors.push_back(color.to_hex());
            }

            return hex_colors;
        }
    };

} // namespace pigment
