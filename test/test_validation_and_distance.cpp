#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;
using namespace pigment::utils;

TEST_CASE("Color Validation Functions") {
    SUBCASE("RGB Validation") {
        CHECK(is_valid_rgb(255, 128, 64));
        CHECK(is_valid_rgb(0, 0, 0));
        CHECK(is_valid_rgb(255, 255, 255, 255));
        
        CHECK_FALSE(is_valid_rgb(-1, 128, 64));
        CHECK_FALSE(is_valid_rgb(256, 128, 64));
        CHECK_FALSE(is_valid_rgb(128, -1, 64));
        CHECK_FALSE(is_valid_rgb(128, 256, 64));
        CHECK_FALSE(is_valid_rgb(128, 128, -1));
        CHECK_FALSE(is_valid_rgb(128, 128, 256));
        CHECK_FALSE(is_valid_rgb(128, 128, 128, -1));
        CHECK_FALSE(is_valid_rgb(128, 128, 128, 256));
    }

    SUBCASE("HSL Validation") {
        CHECK(is_valid_hsl(180.0, 0.5, 0.75));
        CHECK(is_valid_hsl(0.0, 0.0, 0.0));
        CHECK(is_valid_hsl(359.9, 1.0, 1.0));
        
        CHECK_FALSE(is_valid_hsl(-1.0, 0.5, 0.75));
        CHECK_FALSE(is_valid_hsl(360.0, 0.5, 0.75));
        CHECK_FALSE(is_valid_hsl(180.0, -0.1, 0.75));
        CHECK_FALSE(is_valid_hsl(180.0, 1.1, 0.75));
        CHECK_FALSE(is_valid_hsl(180.0, 0.5, -0.1));
        CHECK_FALSE(is_valid_hsl(180.0, 0.5, 1.1));
    }

    SUBCASE("HSV Validation") {
        CHECK(is_valid_hsv(240.0, 0.8, 0.9));
        CHECK(is_valid_hsv(0.0, 0.0, 0.0));
        CHECK(is_valid_hsv(359.9, 1.0, 1.0));
        
        CHECK_FALSE(is_valid_hsv(-1.0, 0.8, 0.9));
        CHECK_FALSE(is_valid_hsv(360.0, 0.8, 0.9));
        CHECK_FALSE(is_valid_hsv(240.0, -0.1, 0.9));
        CHECK_FALSE(is_valid_hsv(240.0, 1.1, 0.9));
        CHECK_FALSE(is_valid_hsv(240.0, 0.8, -0.1));
        CHECK_FALSE(is_valid_hsv(240.0, 0.8, 1.1));
    }

    SUBCASE("LAB Validation") {
        CHECK(is_valid_lab(50.0, 20.0, -30.0));
        CHECK(is_valid_lab(0.0, 0.0, 0.0));
        CHECK(is_valid_lab(100.0, 127.0, 127.0));
        CHECK(is_valid_lab(100.0, -128.0, -128.0));
        
        CHECK_FALSE(is_valid_lab(-1.0, 20.0, -30.0));
        CHECK_FALSE(is_valid_lab(101.0, 20.0, -30.0));
        CHECK_FALSE(is_valid_lab(50.0, 128.0, -30.0));
        CHECK_FALSE(is_valid_lab(50.0, -129.0, -30.0));
        CHECK_FALSE(is_valid_lab(50.0, 20.0, 128.0));
        CHECK_FALSE(is_valid_lab(50.0, 20.0, -129.0));
    }

    SUBCASE("Hex Color Validation") {
        CHECK(is_valid_hex_color("#ff0000"));
        CHECK(is_valid_hex_color("#f00"));
        CHECK(is_valid_hex_color("ff0000"));
        CHECK(is_valid_hex_color("f00"));
        CHECK(is_valid_hex_color("#ff0000ff"));
        CHECK(is_valid_hex_color("FF0000FF"));
        
        CHECK_FALSE(is_valid_hex_color(""));
        CHECK_FALSE(is_valid_hex_color("#"));
        CHECK_FALSE(is_valid_hex_color("#ff"));
        CHECK_FALSE(is_valid_hex_color("#ff00"));
        CHECK_FALSE(is_valid_hex_color("#ff000"));
        CHECK_FALSE(is_valid_hex_color("#ff0000f"));
        CHECK_FALSE(is_valid_hex_color("#gg0000"));
        CHECK_FALSE(is_valid_hex_color("#ff000z"));
    }

    SUBCASE("CSS Color String Validation") {
        CHECK(is_valid_css_rgb("rgb(255, 0, 0)"));
        CHECK(is_valid_css_rgb("rgba(255, 0, 0, 0.5)"));
        CHECK_FALSE(is_valid_css_rgb("hsl(120, 50%, 50%)"));
        CHECK_FALSE(is_valid_css_rgb("#ff0000"));
        CHECK_FALSE(is_valid_css_rgb(""));
        
        CHECK(is_valid_css_hsl("hsl(120, 50%, 50%)"));
        CHECK(is_valid_css_hsl("hsla(120, 50%, 50%, 0.8)"));
        CHECK_FALSE(is_valid_css_hsl("rgb(255, 0, 0)"));
        CHECK_FALSE(is_valid_css_hsl("#ff0000"));
        CHECK_FALSE(is_valid_css_hsl(""));
    }

    SUBCASE("Color Sanitization") {
        RGB sanitized_rgb = sanitize_rgb(300, -50, 400, 300);
        CHECK(sanitized_rgb.r == 255);
        CHECK(sanitized_rgb.g == 0);
        CHECK(sanitized_rgb.b == 255);
        CHECK(sanitized_rgb.a == 255);
        
        HSL sanitized_hsl = sanitize_hsl(420.0, 1.5, -0.2);
        CHECK(std::abs(sanitized_hsl.get_h() - 60.0) < 1.0); // 420 - 360 = 60
        CHECK(std::abs(sanitized_hsl.get_s() - 1.0) < 0.01);
        CHECK(std::abs(sanitized_hsl.get_l() - 0.0) < 0.01);
        
        HSL negative_hue = sanitize_hsl(-60.0, 0.5, 0.5);
        CHECK(std::abs(negative_hue.get_h() - 300.0) < 1.0); // -60 + 360 = 300
    }
}

TEST_CASE("Color Distance Calculations") {
    SUBCASE("RGB Distance") {
        RGB red(255, 0, 0);
        RGB green(0, 255, 0);
        RGB similar_red(250, 5, 5);
        
        double dist_red_green = rgb_distance(red, green);
        double dist_red_similar = rgb_distance(red, similar_red);
        
        CHECK(dist_red_green > dist_red_similar);
        CHECK(dist_red_similar < 10.0);
        CHECK(rgb_distance(red, red) == 0.0);
        
        // Test known distance
        RGB black(0, 0, 0);
        RGB white(255, 255, 255);
        double expected = std::sqrt(255*255 + 255*255 + 255*255);
        CHECK(std::abs(rgb_distance(black, white) - expected) < 0.01);
    }

    SUBCASE("Brightness Difference") {
        RGB black(0, 0, 0);
        RGB white(255, 255, 255);
        RGB gray(128, 128, 128);
        
        double diff_black_white = brightness_difference(black, white);
        double diff_black_gray = brightness_difference(black, gray);
        
        CHECK(diff_black_white > diff_black_gray);
        CHECK(brightness_difference(black, black) == 0.0);
        
        // Test known values
        RGB red(255, 0, 0);
        RGB green(0, 255, 0);
        // Green should appear brighter than red due to luminance formula
        CHECK(brightness_difference(red, green) > 0.0);
    }

    SUBCASE("Hue Difference") {
        RGB red(255, 0, 0);      // 0°
        RGB green(0, 255, 0);    // 120°
        RGB blue(0, 0, 255);     // 240°
        
        double diff_red_green = hue_difference(red, green);
        double diff_red_blue = hue_difference(red, blue);
        double diff_green_blue = hue_difference(green, blue);
        
        // Should be approximately 120° between each
        CHECK(std::abs(diff_red_green - 120.0) < 5.0);
        CHECK(std::abs(diff_red_blue - 120.0) < 5.0);
        CHECK(std::abs(diff_green_blue - 120.0) < 5.0);
        
        // Test wrap-around (shortest path)
        RGB orange(255, 128, 0);  // ~30°
        RGB violet(128, 0, 255);  // ~270°
        double diff_wrap = hue_difference(orange, violet);
        CHECK(diff_wrap < 150.0); // Should take shorter path, but allow for conversion differences
        
        CHECK(hue_difference(red, red) == 0.0);
    }

    SUBCASE("Saturation Difference") {
        RGB saturated_red(255, 0, 0);
        RGB desaturated_red(255, 128, 128);
        RGB gray(128, 128, 128);
        
        double diff_sat_desat = saturation_difference(saturated_red, desaturated_red);
        double diff_sat_gray = saturation_difference(saturated_red, gray);
        
        CHECK(diff_sat_gray > diff_sat_desat);
        CHECK(saturation_difference(saturated_red, saturated_red) == 0.0);
    }

    SUBCASE("Lightness Difference") {
        RGB bright_blue(128, 128, 255);
        RGB dark_blue(32, 32, 128);
        RGB medium_blue(64, 64, 192);
        
        double diff_bright_dark = lightness_difference(bright_blue, dark_blue);
        double diff_bright_medium = lightness_difference(bright_blue, medium_blue);
        
        CHECK(diff_bright_dark > diff_bright_medium);
        CHECK(lightness_difference(bright_blue, bright_blue) == 0.0);
    }

    SUBCASE("Colors Similar") {
        RGB red(255, 0, 0);
        RGB similar_red(250, 5, 5);
        RGB green(0, 255, 0);
        
        CHECK(colors_similar(red, similar_red));
        CHECK_FALSE(colors_similar(red, green));
        CHECK(colors_similar(red, red));
        
        // Test with custom thresholds
        RGB somewhat_different(200, 50, 50);
        CHECK_FALSE(colors_similar(red, somewhat_different, 30.0, 20.0, 15.0));
        CHECK(colors_similar(red, somewhat_different, 100.0, 100.0, 50.0));
    }

    SUBCASE("LAB Color Distance") {
        RGB red(255, 0, 0);
        RGB green(0, 255, 0);
        RGB similar_red(250, 5, 5);
        
        double dist_red_green = color_distance(red, green);
        double dist_red_similar = color_distance(red, similar_red);
        
        CHECK(dist_red_green > dist_red_similar);
        CHECK(dist_red_similar < 10.0);
        CHECK(color_distance(red, red) == 0.0);
        
        // LAB distance should be perceptually meaningful
        RGB blue(0, 0, 255);
        double dist_red_blue = color_distance(red, blue);
        CHECK(dist_red_blue > 0.0);
    }
}

TEST_CASE("Advanced Color Properties") {
    SUBCASE("Gamma Correction") {
        RGB color(128, 64, 192);
        
        // Test applying and removing gamma
        RGB gamma_applied = color.apply_gamma(2.2);
        RGB gamma_removed = gamma_applied.remove_gamma(2.2);
        
        // Should be close to original after roundtrip
        CHECK(std::abs(gamma_removed.r - color.r) <= 2);
        CHECK(std::abs(gamma_removed.g - color.g) <= 2);
        CHECK(std::abs(gamma_removed.b - color.b) <= 2);
        
        // Different gamma values should produce different results
        RGB gamma_1 = color.apply_gamma(1.0);
        RGB gamma_2 = color.apply_gamma(2.2);
        CHECK(gamma_1 != gamma_2);
        
        // Gamma 1.0 should be identity
        CHECK(gamma_1 == color);
    }

    SUBCASE("Color Temperature") {
        // Test various color temperatures
        RGB warm = temperature_to_rgb(3000);   // Warm/orange
        RGB daylight = temperature_to_rgb(6500); // Daylight
        RGB cool = temperature_to_rgb(9000);   // Cool/blue
        
        // Warm light should be more red
        CHECK(warm.r > warm.b);
        
        // Cool light should be more blue
        CHECK(cool.b > cool.r);
        
        // Daylight should be relatively balanced
        CHECK(daylight.r > 200);
        CHECK(daylight.g > 200);
        CHECK(daylight.b > 200);
        
        // Extreme temperatures should be clamped
        RGB very_warm = temperature_to_rgb(500);   // Should clamp to 1000
        RGB very_cool = temperature_to_rgb(50000); // Should clamp to 40000
        CHECK(very_warm.r > 0);
        CHECK(very_cool.b > 0);
    }

    SUBCASE("Grayscale Variants") {
        RGB color(255, 128, 64);
        
        RGB gray_avg = to_grayscale_average(color);
        RGB gray_lum = to_grayscale_luminance(color);
        RGB gray_light = to_grayscale_lightness(color);
        RGB gray_desat = to_grayscale_desaturate(color);
        
        // All should be grayscale (r == g == b)
        CHECK(gray_avg.r == gray_avg.g);
        CHECK(gray_avg.g == gray_avg.b);
        CHECK(gray_lum.r == gray_lum.g);
        CHECK(gray_lum.g == gray_lum.b);
        CHECK(gray_light.r == gray_light.g);
        CHECK(gray_light.g == gray_light.b);
        CHECK(gray_desat.r == gray_desat.g);
        CHECK(gray_desat.g == gray_desat.b);
        
        // Different methods should produce different results
        CHECK(gray_avg.r != gray_lum.r);
        CHECK(gray_lum.r != gray_light.r);
        
        // Alpha should be preserved
        CHECK(gray_avg.a == color.a);
        CHECK(gray_lum.a == color.a);
        CHECK(gray_light.a == color.a);
        CHECK(gray_desat.a == color.a);
    }

    SUBCASE("Sepia Tone") {
        RGB color(128, 96, 64);
        RGB sepia = to_sepia(color);
        
        // Sepia should have a brownish tint (more red/yellow than blue)
        CHECK(sepia.r >= sepia.g);
        CHECK(sepia.g >= sepia.b);
        
        // Should be different from original
        CHECK(sepia != color);
        
        // Alpha should be preserved
        CHECK(sepia.a == color.a);
        
        // Test with extreme colors
        RGB white(255, 255, 255);
        RGB sepia_white = to_sepia(white);
        CHECK(sepia_white.r <= 255);
        CHECK(sepia_white.g <= 255);
        CHECK(sepia_white.b <= 255);
    }
}