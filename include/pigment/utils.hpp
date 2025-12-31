#pragma once

#include "types_basic.hpp"
#include "types_hsl.hpp"
#include "types_lab.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace pigment {
    namespace utils {

        // Color blindness simulation
        struct ColorBlindness {
            enum Type {
                PROTANOPIA,    // Red blind
                DEUTERANOPIA,  // Green blind
                TRITANOPIA,    // Blue blind
                PROTANOMALY,   // Red weak
                DEUTERANOMALY, // Green weak
                TRITANOMALY    // Blue weak
            };

            static RGB simulate(const RGB &color, Type type) {
                // Simplified simulation using transformation matrices
                double r = color.r / 255.0;
                double g = color.g / 255.0;
                double b = color.b / 255.0;

                double nr, ng, nb;

                switch (type) {
                case PROTANOPIA:
                    nr = 0.567 * r + 0.433 * g;
                    ng = 0.558 * r + 0.442 * g;
                    nb = 0.242 * g + 0.758 * b;
                    break;
                case DEUTERANOPIA:
                    nr = 0.625 * r + 0.375 * g;
                    ng = 0.7 * r + 0.3 * g;
                    nb = 0.3 * g + 0.7 * b;
                    break;
                case TRITANOPIA:
                    nr = 0.95 * r + 0.05 * g;
                    ng = 0.433 * g + 0.567 * b;
                    nb = 0.475 * g + 0.525 * b;
                    break;
                default: // For anomalies, use a lighter version of the respective type
                    return simulate(color, static_cast<Type>(type - 3)).mix(color, 0.5);
                }

                return RGB(std::clamp(static_cast<int>(nr * 255), 0, 255),
                           std::clamp(static_cast<int>(ng * 255), 0, 255),
                           std::clamp(static_cast<int>(nb * 255), 0, 255), color.a);
            }
        };

        // Contrast calculation
        inline double contrast_ratio(const RGB &color1, const RGB &color2) {
            double lum1 = color1.luminance() / 255.0;
            double lum2 = color2.luminance() / 255.0;

            // Ensure lum1 is the lighter color
            if (lum1 < lum2)
                std::swap(lum1, lum2);

            return (lum1 + 0.05) / (lum2 + 0.05);
        }

        // Check WCAG accessibility compliance
        struct AccessibilityLevel {
            enum Level {
                FAIL,
                AA_NORMAL,  // 4.5:1
                AA_LARGE,   // 3:1 for large text
                AAA_NORMAL, // 7:1
                AAA_LARGE   // 4.5:1 for large text
            };
        };

        inline AccessibilityLevel::Level check_accessibility(const RGB &foreground, const RGB &background,
                                                             bool large_text = false) {
            double ratio = contrast_ratio(foreground, background);

            if (ratio >= 7.0)
                return AccessibilityLevel::AAA_NORMAL;
            if (ratio >= 4.5) {
                return large_text ? AccessibilityLevel::AAA_LARGE : AccessibilityLevel::AA_NORMAL;
            }
            if (ratio >= 3.0 && large_text)
                return AccessibilityLevel::AA_LARGE;

            return AccessibilityLevel::FAIL;
        }

        // Find the best contrasting color (black or white)
        inline RGB best_contrast_color(const RGB &background) {
            double contrast_with_white = contrast_ratio(RGB::white(), background);
            double contrast_with_black = contrast_ratio(RGB::black(), background);

            return (contrast_with_white > contrast_with_black) ? RGB::white() : RGB::black();
        }

        // Color temperature estimation (in Kelvin)
        inline double color_temperature(const RGB &color) {
            // Simplified calculation based on chromaticity
            double r = color.r / 255.0;
            double g = color.g / 255.0;
            double b = color.b / 255.0;

            // Convert to XYZ first (simplified)
            double x = r * 0.4124 + g * 0.3576 + b * 0.1805;
            double y = r * 0.2126 + g * 0.7152 + b * 0.0722;
            double z = r * 0.0193 + g * 0.1192 + b * 0.9505;

            double total = x + y + z;
            if (total == 0)
                return 6500; // Default daylight

            double cx = x / total;
            double cy = y / total;

            // McCamy's approximation
            double n = (cx - 0.3320) / (0.1858 - cy);
            return 449.0 * n * n * n + 3525.0 * n * n + 6823.3 * n + 5520.33;
        }

        // Check if color is warm or cool
        inline bool is_warm_color(const RGB &color) {
            return color_temperature(color) < 5000; // Below daylight temperature
        }

        // Generate monochromatic color variations
        inline std::vector<RGB> generate_monochromatic(const RGB &base, int count = 5) {
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors;
            colors.reserve(count);

            // Generate variations by adjusting lightness and saturation
            for (int i = 0; i < count; ++i) {

                // Create variations with different lightness levels
                HSL variation = hsl;
                if (i == count / 2) {
                    // Keep original color in the middle
                    colors.push_back(base);
                } else if (i < count / 2) {
                    // Darker variations
                    variation = hsl.darken(0.1 * (count / 2 - i));
                } else {
                    // Lighter variations
                    variation = hsl.lighten(0.1 * (i - count / 2));
                }

                if (i != count / 2) {
                    colors.push_back(variation.to_rgb());
                }
            }

            return colors;
        }

        // Generate enhanced split-complementary with custom angles
        inline std::vector<RGB> generate_split_complementary(const RGB &base, double angle = 30.0) {
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors = {base};

            // Generate split-complementary with custom angle
            colors.push_back(hsl.adjust_hue(180.0 - angle).to_rgb());
            colors.push_back(hsl.adjust_hue(180.0 + angle).to_rgb());

            return colors;
        }

        // Generate colors based on golden ratio (137.5°)
        inline std::vector<RGB> generate_golden_ratio_scheme(const RGB &base, int count = 5) {
            constexpr double GOLDEN_ANGLE = 137.507764050; // Golden angle in degrees
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors = {base};

            for (int i = 1; i < count; ++i) {
                HSL variation = hsl.adjust_hue(GOLDEN_ANGLE * i);
                colors.push_back(variation.to_rgb());
            }

            return colors;
        }

        // Generate a harmonious color scheme
        inline std::vector<RGB> generate_harmony(const RGB &base, const std::string &scheme = "complementary") {
            HSL hsl = HSL::fromRGB(base);
            std::vector<RGB> colors = {base};

            if (scheme == "complementary") {
                colors.push_back(hsl.complement().to_rgb());
            } else if (scheme == "triadic") {
                auto triadic = hsl.triadic();
                for (size_t i = 1; i < triadic.size(); ++i) {
                    colors.push_back(triadic[i].to_rgb());
                }
            } else if (scheme == "split_complementary") {
                auto split = hsl.split_complementary();
                for (size_t i = 1; i < split.size(); ++i) {
                    colors.push_back(split[i].to_rgb());
                }
            } else if (scheme == "analogous") {
                auto analogous = hsl.analogous();
                for (size_t i = 0; i < analogous.size(); ++i) {
                    if (i != 1) { // Skip the base color (middle one)
                        colors.push_back(analogous[i].to_rgb());
                    }
                }
            } else if (scheme == "tetradic") {
                colors.push_back(hsl.adjust_hue(90).to_rgb());
                colors.push_back(hsl.adjust_hue(180).to_rgb());
                colors.push_back(hsl.adjust_hue(270).to_rgb());
            } else if (scheme == "monochromatic") {
                return generate_monochromatic(base, 5);
            } else if (scheme == "golden_ratio") {
                return generate_golden_ratio_scheme(base, 5);
            }

            return colors;
        }

        // Color sorting functions
        inline void sort_by_hue(std::vector<RGB> &colors) {
            std::sort(colors.begin(), colors.end(), [](const RGB &a, const RGB &b) {
                HSL hsl_a = HSL::fromRGB(a);
                HSL hsl_b = HSL::fromRGB(b);
                return hsl_a.h < hsl_b.h;
            });
        }

        inline void sort_by_brightness(std::vector<RGB> &colors) {
            std::sort(colors.begin(), colors.end(),
                      [](const RGB &a, const RGB &b) { return a.luminance() < b.luminance(); });
        }

        inline void sort_by_saturation(std::vector<RGB> &colors) {
            std::sort(colors.begin(), colors.end(), [](const RGB &a, const RGB &b) {
                HSL hsl_a = HSL::fromRGB(a);
                HSL hsl_b = HSL::fromRGB(b);
                return hsl_a.s < hsl_b.s;
            });
        }

        // Color distance calculation (LAB-based perceptual distance)
        inline double color_distance(const RGB &color1, const RGB &color2) {
            LAB lab1 = LAB::fromRGB(color1);
            LAB lab2 = LAB::fromRGB(color2);
            return lab1.delta_e(lab2);
        }

        // Simple RGB Euclidean distance
        inline double rgb_distance(const RGB &color1, const RGB &color2) {
            double dr = color1.r - color2.r;
            double dg = color1.g - color2.g;
            double db = color1.b - color2.b;
            return std::sqrt(dr * dr + dg * dg + db * db);
        }

        // Brightness difference (perceived luminance)
        inline double brightness_difference(const RGB &color1, const RGB &color2) {
            return std::abs(color1.luminance() - color2.luminance());
        }

        // Hue difference in degrees (0-180, shortest angular distance)
        inline double hue_difference(const RGB &color1, const RGB &color2) {
            HSL hsl1 = HSL::fromRGB(color1);
            HSL hsl2 = HSL::fromRGB(color2);

            double h1 = hsl1.get_h();
            double h2 = hsl2.get_h();

            // Calculate shortest angular distance
            double diff = std::abs(h1 - h2);
            if (diff > 180.0) {
                diff = 360.0 - diff;
            }

            return diff;
        }

        // Saturation difference
        inline double saturation_difference(const RGB &color1, const RGB &color2) {
            HSL hsl1 = HSL::fromRGB(color1);
            HSL hsl2 = HSL::fromRGB(color2);
            return std::abs(hsl1.get_s() - hsl2.get_s());
        }

        // Lightness difference
        inline double lightness_difference(const RGB &color1, const RGB &color2) {
            HSL hsl1 = HSL::fromRGB(color1);
            HSL hsl2 = HSL::fromRGB(color2);
            return std::abs(hsl1.get_l() - hsl2.get_l());
        }

        // Check if two colors are similar (multiple criteria)
        inline bool colors_similar(const RGB &color1, const RGB &color2, double rgb_threshold = 30.0,
                                   double brightness_threshold = 20.0, double hue_threshold = 15.0) {
            return rgb_distance(color1, color2) < rgb_threshold &&
                   brightness_difference(color1, color2) < brightness_threshold &&
                   hue_difference(color1, color2) < hue_threshold;
        }

        // Find the closest color in a palette
        inline RGB find_closest_color(const RGB &target, const std::vector<RGB> &palette) {
            if (palette.empty())
                return target;

            RGB closest = palette[0];
            double min_distance = color_distance(target, closest);

            for (const auto &color : palette) {
                double distance = color_distance(target, color);
                if (distance < min_distance) {
                    min_distance = distance;
                    closest = color;
                }
            }

            return closest;
        }

        // Quantize colors to a palette
        inline std::vector<RGB> quantize_to_palette(const std::vector<RGB> &colors, const std::vector<RGB> &palette) {
            std::vector<RGB> quantized;
            quantized.reserve(colors.size());

            for (const auto &color : colors) {
                quantized.push_back(find_closest_color(color, palette));
            }

            return quantized;
        }

        // Color validation functions
        inline bool is_valid_rgb(int r, int g, int b, int a = 255) {
            return r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255 && a >= 0 && a <= 255;
        }

        inline bool is_valid_hsl(double h, double s, double l) {
            return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && l >= 0.0 && l <= 1.0;
        }

        inline bool is_valid_hsv(double h, double s, double v) {
            return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && v >= 0.0 && v <= 1.0;
        }

        inline bool is_valid_lab(double l, double a, double b) {
            return l >= 0.0 && l <= 100.0 && a >= -128.0 && a <= 127.0 && b >= -128.0 && b <= 127.0;
        }

        // String validation functions
        inline bool is_valid_hex_color(const std::string &hex) {
            if (hex.empty())
                return false;

            std::string h = hex;
            if (h[0] == '#')
                h.erase(0, 1);

            if (h.size() != 3 && h.size() != 6 && h.size() != 8)
                return false;

            for (char c : h) {
                if (!std::isxdigit(c))
                    return false;
            }

            return true;
        }

        inline bool is_valid_css_rgb(const std::string &css) {
            if (css.empty())
                return false;
            return css.substr(0, 4) == "rgb(" || css.substr(0, 5) == "rgba(";
        }

        inline bool is_valid_css_hsl(const std::string &css) {
            if (css.empty())
                return false;
            return css.substr(0, 4) == "hsl(" || css.substr(0, 5) == "hsla(";
        }

        // Sanitization functions - clamp values to valid ranges
        inline RGB sanitize_rgb(int r, int g, int b, int a = 255) {
            return RGB(std::clamp(r, 0, 255), std::clamp(g, 0, 255), std::clamp(b, 0, 255), std::clamp(a, 0, 255));
        }

        inline HSL sanitize_hsl(double h, double s, double l) {
            // Normalize hue to [0, 360)
            h = std::fmod(h, 360.0);
            if (h < 0)
                h += 360.0;

            return HSL(h, std::clamp(s, 0.0, 1.0), std::clamp(l, 0.0, 1.0));
        }

        // Temperature to RGB conversion (Kelvin to RGB)
        inline RGB temperature_to_rgb(double kelvin) {
            // Clamp temperature to reasonable range
            kelvin = std::clamp(kelvin, 1000.0, 40000.0);

            double temp = kelvin / 100.0;
            double red, green, blue;

            // Calculate red
            if (temp <= 66.0) {
                red = 255.0;
            } else {
                red = temp - 60.0;
                red = 329.698727446 * std::pow(red, -0.1332047592);
                red = std::clamp(red, 0.0, 255.0);
            }

            // Calculate green
            if (temp <= 66.0) {
                green = temp;
                green = 99.4708025861 * std::log(green) - 161.1195681661;
                green = std::clamp(green, 0.0, 255.0);
            } else {
                green = temp - 60.0;
                green = 288.1221695283 * std::pow(green, -0.0755148492);
                green = std::clamp(green, 0.0, 255.0);
            }

            // Calculate blue
            if (temp >= 66.0) {
                blue = 255.0;
            } else if (temp <= 19.0) {
                blue = 0.0;
            } else {
                blue = temp - 10.0;
                blue = 138.5177312231 * std::log(blue) - 305.0447927307;
                blue = std::clamp(blue, 0.0, 255.0);
            }

            return RGB(static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue));
        }

        // Grayscale conversion variants
        inline RGB to_grayscale_average(const RGB &color) {
            uint8_t gray = static_cast<uint8_t>((color.r + color.g + color.b) / 3);
            return RGB(gray, gray, gray, color.a);
        }

        inline RGB to_grayscale_luminance(const RGB &color) {
            uint8_t gray = static_cast<uint8_t>(color.luminance());
            return RGB(gray, gray, gray, color.a);
        }

        inline RGB to_grayscale_lightness(const RGB &color) {
            uint8_t gray = static_cast<uint8_t>(
                (std::max({color.r, color.g, color.b}) + std::min({color.r, color.g, color.b})) / 2);
            return RGB(gray, gray, gray, color.a);
        }

        inline RGB to_grayscale_desaturate(const RGB &color) {
            HSL hsl = HSL::fromRGB(color);
            return HSL(hsl.get_h(), 0.0, hsl.get_l()).to_rgb();
        }

        // Sepia tone conversion
        inline RGB to_sepia(const RGB &color) {
            double r = color.r;
            double g = color.g;
            double b = color.b;

            uint8_t sepia_r = static_cast<uint8_t>(std::clamp((r * 0.393) + (g * 0.769) + (b * 0.189), 0.0, 255.0));
            uint8_t sepia_g = static_cast<uint8_t>(std::clamp((r * 0.349) + (g * 0.686) + (b * 0.168), 0.0, 255.0));
            uint8_t sepia_b = static_cast<uint8_t>(std::clamp((r * 0.272) + (g * 0.534) + (b * 0.131), 0.0, 255.0));

            return RGB(sepia_r, sepia_g, sepia_b, color.a);
        }

        // Remove duplicate colors from palette
        inline std::vector<RGB> remove_duplicates(const std::vector<RGB> &palette, double threshold = 5.0) {
            std::vector<RGB> unique_colors;

            for (const auto &color : palette) {
                bool is_duplicate = false;
                for (const auto &unique : unique_colors) {
                    if (rgb_distance(color, unique) < threshold) {
                        is_duplicate = true;
                        break;
                    }
                }
                if (!is_duplicate) {
                    unique_colors.push_back(color);
                }
            }

            return unique_colors;
        }

        // Extract dominant colors from a color array (simple approach)
        inline std::vector<RGB> extract_dominant_colors(const std::vector<RGB> &colors, int count = 5) {
            if (colors.empty())
                return {};

            // Simple approach: cluster colors by similarity and pick representatives
            std::vector<RGB> dominant;
            std::vector<RGB> remaining = colors;

            while (dominant.size() < static_cast<size_t>(count) && !remaining.empty()) {
                // Find the color that's most different from already selected colors
                RGB best_color = remaining[0];
                double best_distance = 0.0;
                size_t best_index = 0;

                for (size_t i = 0; i < remaining.size(); ++i) {
                    double min_distance = std::numeric_limits<double>::max();

                    for (const auto &selected : dominant) {
                        double dist = rgb_distance(remaining[i], selected);
                        min_distance = std::min(min_distance, dist);
                    }

                    if (min_distance > best_distance) {
                        best_distance = min_distance;
                        best_color = remaining[i];
                        best_index = i;
                    }
                }

                dominant.push_back(best_color);
                remaining.erase(remaining.begin() + best_index);
            }

            return dominant;
        }

    } // namespace utils
} // namespace pigment
