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

        // Color distance calculation
        inline double color_distance(const RGB &color1, const RGB &color2) {
            LAB lab1 = LAB::fromRGB(color1);
            LAB lab2 = LAB::fromRGB(color2);
            return lab1.delta_e(lab2);
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

    } // namespace utils
} // namespace pigment
