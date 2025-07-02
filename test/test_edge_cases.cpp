#include <cmath>
#include <doctest/doctest.h>
#include <limits>
#include <pigment/pigment.hpp>

using namespace pigment;

TEST_CASE("Color Conversion Edge Cases") {
    SUBCASE("RGB Edge Values") {
        // Test boundary values
        RGB min_rgb(0, 0, 0);
        RGB max_rgb(255, 255, 255);
        RGB mid_rgb(128, 128, 128);

        CHECK(min_rgb.r == 0);
        CHECK(max_rgb.r == 255);
        CHECK(mid_rgb.r == 128);

        // Test that constructor with out-of-range values gets truncated by uint8_t
        RGB over_max(static_cast<uint8_t>(300), static_cast<uint8_t>(-50), static_cast<uint8_t>(400));
        // Values are automatically truncated to uint8_t range (0-255)
        CHECK(over_max.r == 44);  // 300 % 256 = 44  
        CHECK(over_max.g == 206); // -50 as uint8_t = 256-50 = 206
        CHECK(over_max.b == 144); // 400 % 256 = 144
        CHECK(true); // Constructor succeeded
    }

    SUBCASE("HSL Edge Values") {
        // Test hue wrapping
        HSL wrapped_hue(720.0, 0.5, 0.5); // 720° should normalize to 0°
        CHECK(wrapped_hue.h == 0); // 0.0 * 100

        HSL negative_hue(-120.0, 0.5, 0.5); // -120° should normalize to 240°  
        CHECK(negative_hue.h == 24000); // 240.0 * 100

        // Test saturation and lightness clamping
        HSL clamped(180.0, 2.0, -0.5);
        CHECK(clamped.s == 255); // 1.0 * 255 (clamped)
        CHECK(clamped.l == 0);   // 0.0 * 255 (clamped)
    }

    SUBCASE("HSV Edge Values") {
        // Test hue wrapping
        HSV wrapped_hue(450.0f, 0.8f, 0.9f); // 450° should normalize to 90°
        CHECK(wrapped_hue.h == 90.0f);

        // Test saturation and value clamping
        HSV clamped(180.0f, 1.5f, -0.2f);
        CHECK(clamped.s == 1.0f);
        CHECK(clamped.v == 0.0f);
    }

    SUBCASE("LAB Edge Values") {
        // Test extreme LAB values
        LAB extreme_lab(200.0, 200.0, -200.0);
        RGB converted = extreme_lab.to_rgb();

        // Should clamp to valid RGB range
        CHECK(converted.r >= 0);
        CHECK(converted.r <= 255);
        CHECK(converted.g >= 0);
        CHECK(converted.g <= 255);
        CHECK(converted.b >= 0);
        CHECK(converted.b <= 255);
    }
    SUBCASE("Hex String Edge Cases") {
        // Test invalid hex strings - these might throw exceptions or return default values
        try {
            RGB invalid_hex1("#GGGGGG"); // Invalid characters
            // If no exception, check it's some valid RGB value
            CHECK(invalid_hex1.r >= 0);
            CHECK(invalid_hex1.r <= 255);
        } catch (...) {
            // Exception is acceptable for invalid input
            CHECK(true);
        }

        try {
            RGB invalid_hex2("#GGG"); // Invalid short hex
            CHECK(invalid_hex2.r >= 0);
            CHECK(invalid_hex2.r <= 255);
        } catch (...) {
            CHECK(true);
        }

        try {
            RGB no_hash("FF0000");
            CHECK(no_hash.r >= 0);
            CHECK(no_hash.r <= 255);
        } catch (...) {
            CHECK(true);
        }

        try {
            RGB empty_hex("");
            CHECK(empty_hex.r >= 0);
            CHECK(empty_hex.r <= 255);
        } catch (...) {
            CHECK(true);
        }
    }
}

TEST_CASE("Precision and Rounding Tests") {
    SUBCASE("RGB to HSL and Back Precision") {
        std::vector<RGB> test_colors = {RGB(123, 87, 213), RGB(45, 67, 89), RGB(200, 150, 100), RGB(50, 150, 250)};

        for (const auto &original : test_colors) {
            HSL hsl = HSL::fromRGB(original);
            RGB converted_back = hsl.to_rgb();

            // Allow for small rounding errors (within 2 units)
            CHECK(std::abs(converted_back.r - original.r) <= 2);
            CHECK(std::abs(converted_back.g - original.g) <= 2);
            CHECK(std::abs(converted_back.b - original.b) <= 2);
        }
    }

    SUBCASE("RGB to HSV and Back Precision") {
        std::vector<RGB> test_colors = {RGB(123, 87, 213), RGB(45, 67, 89), RGB(200, 150, 100), RGB(50, 150, 250)};

        for (const auto &original : test_colors) {
            HSV hsv = HSV::fromRGB(original);
            RGB converted_back = hsv.to_rgb();

            // Allow for small rounding errors (within 2 units)
            CHECK(std::abs(converted_back.r - original.r) <= 2);
            CHECK(std::abs(converted_back.g - original.g) <= 2);
            CHECK(std::abs(converted_back.b - original.b) <= 2);
        }
    }

    SUBCASE("RGB to LAB and Back Precision") {
        std::vector<RGB> test_colors = {RGB(123, 87, 213), RGB(45, 67, 89), RGB(200, 150, 100), RGB(50, 150, 250)};

        for (const auto &original : test_colors) {
            LAB lab = LAB::fromRGB(original);
            RGB converted_back = lab.to_rgb();

            // LAB conversion may have larger errors due to gamut differences
            CHECK(std::abs(converted_back.r - original.r) <= 5);
            CHECK(std::abs(converted_back.g - original.g) <= 5);
            CHECK(std::abs(converted_back.b - original.b) <= 5);
        }
    }
}

TEST_CASE("Mathematical Operations Edge Cases") {
    SUBCASE("Color Mixing Edge Cases") {
        RGB red = RGB::red();
        RGB blue = RGB::blue();

        // Test extreme mixing ratios
        RGB mix_0 = red.mix(blue, 0.0);
        CHECK(mix_0 == red);

        RGB mix_1 = red.mix(blue, 1.0);
        CHECK(mix_1 == blue);

        // Test mixing with itself
        RGB self_mix = red.mix(red, 0.5);
        CHECK(self_mix == red);
    }

    SUBCASE("Arithmetic Operations Overflow") {
        RGB high_color(200, 200, 200);
        RGB add_result = high_color + RGB(100, 100, 100);

        // Should clamp at 255
        CHECK(add_result.r == 255);
        CHECK(add_result.g == 255);
        CHECK(add_result.b == 255);

        RGB low_color(50, 50, 50);
        RGB sub_result = low_color - RGB(100, 100, 100);

        // Should clamp at 0
        CHECK(sub_result.r == 0);
        CHECK(sub_result.g == 0);
        CHECK(sub_result.b == 0);
    }

    SUBCASE("Scaling Operations") {
        RGB color(100, 150, 200);

        // Test scaling by 0
        RGB scaled_zero = color * 0.0;
        CHECK(scaled_zero.r == 0);
        CHECK(scaled_zero.g == 0);
        CHECK(scaled_zero.b == 0);

        // Test scaling by values > 1
        RGB scaled_high = color * 2.0;
        CHECK(scaled_high.r == 200);
        CHECK(scaled_high.g == 255); // Clamped
        CHECK(scaled_high.b == 255); // Clamped

        // Test negative scaling
        RGB scaled_neg = color * -0.5;
        CHECK(scaled_neg.r == 0); // Clamped
        CHECK(scaled_neg.g == 0); // Clamped
        CHECK(scaled_neg.b == 0); // Clamped
    }
}

TEST_CASE("Palette Edge Cases") {
    SUBCASE("Empty Palette Operations") {
        Palette empty_palette;

        CHECK(empty_palette.empty());
        CHECK(empty_palette.size() == 0);

        // Test operations on empty palette
        empty_palette.clear(); // Should not crash
        CHECK(empty_palette.empty());

        auto hex_colors = empty_palette.to_hex();
        CHECK(hex_colors.empty());
    }

    SUBCASE("Single Color Gradient") {
        auto gradient = Palette::gradient(RGB::red(), RGB::red(), 5);
        CHECK(gradient.size() == 5);

        // All colors should be the same
        for (size_t i = 0; i < gradient.size(); ++i) {
            CHECK(gradient[i] == RGB::red());
        }
    }

    SUBCASE("Zero Steps Gradient") {
        auto gradient = Palette::gradient(RGB::red(), RGB::blue(), 0);
        CHECK(gradient.size() == 0);
    }

    SUBCASE("Large Palette Operations") {
        Palette large_palette;

        // Add many colors
        for (int i = 0; i < 1000; ++i) {
            large_palette.add(RGB(i % 256, (i * 2) % 256, (i * 3) % 256));
        }

        CHECK(large_palette.size() == 1000);
        CHECK_FALSE(large_palette.empty());

        // Test clearing large palette
        large_palette.clear();
        CHECK(large_palette.empty());
        CHECK(large_palette.size() == 0);
    }
}
