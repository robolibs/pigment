#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;
using namespace pigment::utils;

TEST_CASE("Gamma Correction Tests") {
    SUBCASE("Apply and Remove Gamma") {
        RGB color(128, 64, 192);
        
        // Test applying and removing gamma
        RGB gamma_applied = color.apply_gamma(2.2);
        RGB gamma_removed = gamma_applied.remove_gamma(2.2);
        
        // Should be close to original after roundtrip (allow 2 unit difference)
        CHECK(std::abs(gamma_removed.r - color.r) <= 2);
        CHECK(std::abs(gamma_removed.g - color.g) <= 2);
        CHECK(std::abs(gamma_removed.b - color.b) <= 2);
        CHECK(gamma_removed.a == color.a);
    }
    
    SUBCASE("Different Gamma Values") {
        RGB color(150, 100, 200);
        
        RGB gamma_1 = color.apply_gamma(1.0);
        RGB gamma_2_2 = color.apply_gamma(2.2);
        RGB gamma_1_8 = color.apply_gamma(1.8);
        
        // Gamma 1.0 should be identity
        CHECK(gamma_1 == color);
        
        // Different gamma values should produce different results
        CHECK(gamma_2_2 != gamma_1_8);
        CHECK(gamma_2_2 != color);
        CHECK(gamma_1_8 != color);
    }
    
    SUBCASE("Gamma Edge Cases") {
        RGB black(0, 0, 0);
        RGB white(255, 255, 255);
        
        // Black should remain black
        RGB black_gamma = black.apply_gamma(2.2);
        CHECK(black_gamma == black);
        
        // White should remain white
        RGB white_gamma = white.apply_gamma(2.2);
        CHECK(white_gamma == white);
        
        // Test extreme gamma values
        RGB mid_gray(128, 128, 128);
        RGB extreme_low = mid_gray.apply_gamma(0.5);
        RGB extreme_high = mid_gray.apply_gamma(5.0);
        
        CHECK(extreme_low != mid_gray);
        CHECK(extreme_high != mid_gray);
        CHECK(extreme_low != extreme_high);
    }
}

TEST_CASE("Temperature Conversion Tests") {
    SUBCASE("Basic Temperature Conversion") {
        // Test various color temperatures
        RGB warm = temperature_to_rgb(3000);   // Warm/orange
        RGB daylight = temperature_to_rgb(6500); // Daylight
        RGB cool = temperature_to_rgb(9000);   // Cool/blue
        
        // Warm light should be more red
        CHECK(warm.r > warm.b);
        CHECK(warm.r > 150); // Should have significant red component
        
        // Cool light should be more blue  
        CHECK(cool.b > cool.r);
        CHECK(cool.b > 150); // Should have significant blue component
        
        // Daylight should be relatively balanced
        CHECK(daylight.r > 200);
        CHECK(daylight.g > 200);
        CHECK(daylight.b > 200);
    }
    
    SUBCASE("Temperature Extremes") {
        // Test temperature clamping
        RGB very_warm = temperature_to_rgb(500);   // Should clamp to 1000K
        RGB very_cool = temperature_to_rgb(50000); // Should clamp to 40000K
        
        // Should still produce valid colors
        CHECK(very_warm.r > 0);
        CHECK(very_warm.g >= 0);
        CHECK(very_warm.b >= 0);
        
        CHECK(very_cool.r >= 0);
        CHECK(very_cool.g >= 0);
        CHECK(very_cool.b > 0);
        
        // Test specific clamped values
        RGB min_temp = temperature_to_rgb(1000);
        RGB max_temp = temperature_to_rgb(40000);
        
        CHECK(very_warm == min_temp);
        CHECK(very_cool == max_temp);
    }
    
    SUBCASE("Temperature Progression") {
        // Test that temperature creates a logical progression
        RGB temp_2000 = temperature_to_rgb(2000);
        RGB temp_4000 = temperature_to_rgb(4000);
        RGB temp_6000 = temperature_to_rgb(6000);
        RGB temp_8000 = temperature_to_rgb(8000);
        RGB temp_10000 = temperature_to_rgb(10000);
        
        // As temperature increases, blue should increase
        CHECK(temp_2000.b < temp_6000.b);
        CHECK(temp_6000.b < temp_10000.b);
        
        // Very warm temps should have low blue component
        CHECK(temp_2000.b < 100);
        
        // Cool temps should have higher blue component
        CHECK(temp_10000.b > temp_2000.b);
    }
}

TEST_CASE("Grayscale Conversion Tests") {
    SUBCASE("Grayscale Methods") {
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
        
        // Alpha should be preserved
        CHECK(gray_avg.a == color.a);
        CHECK(gray_lum.a == color.a);
        CHECK(gray_light.a == color.a);
        CHECK(gray_desat.a == color.a);
    }
    
    SUBCASE("Grayscale Algorithm Differences") {
        RGB colorful(255, 128, 64);
        
        RGB gray_avg = to_grayscale_average(colorful);
        RGB gray_lum = to_grayscale_luminance(colorful);
        RGB gray_light = to_grayscale_lightness(colorful);
        
        // Different methods should produce different results for colorful images
        CHECK(gray_avg.r != gray_lum.r);
        CHECK(gray_lum.r != gray_light.r);
        CHECK(gray_avg.r != gray_light.r);
        
        // Check expected values
        CHECK(gray_avg.r == (255 + 128 + 64) / 3); // Average
        CHECK(gray_light.r == (255 + 64) / 2); // Lightness (max + min) / 2
        
        // Luminance should be weighted towards green
        double expected_lum = 0.299 * 255 + 0.587 * 128 + 0.114 * 64;
        CHECK(std::abs(gray_lum.r - expected_lum) < 1.0);
    }
    
    SUBCASE("Grayscale Edge Cases") {
        // Already grayscale color
        RGB gray(128, 128, 128);
        RGB gray_result = to_grayscale_luminance(gray);
        // Should be close since luminance of gray is roughly the same
        CHECK(std::abs(gray_result.r - gray.r) <= 1);
        
        // Pure colors
        RGB red(255, 0, 0);
        RGB green(0, 255, 0);
        RGB blue(0, 0, 255);
        
        RGB red_gray = to_grayscale_luminance(red);
        RGB green_gray = to_grayscale_luminance(green);
        RGB blue_gray = to_grayscale_luminance(blue);
        
        // Green should be brightest (highest luminance weight)
        CHECK(green_gray.r > red_gray.r);
        CHECK(green_gray.r > blue_gray.r);
        CHECK(red_gray.r > blue_gray.r); // Red has higher weight than blue
    }
}

TEST_CASE("Sepia Tone Tests") {
    SUBCASE("Basic Sepia Conversion") {
        RGB color(128, 96, 64);
        RGB sepia = to_sepia(color);
        
        // Sepia should have a brownish tint (more red/yellow than blue)
        CHECK(sepia.r >= sepia.g);
        CHECK(sepia.g >= sepia.b);
        
        // Should be different from original
        CHECK(sepia != color);
        
        // Alpha should be preserved
        CHECK(sepia.a == color.a);
    }
    
    SUBCASE("Sepia with Extreme Colors") {
        RGB white(255, 255, 255);
        RGB black(0, 0, 0);
        RGB red(255, 0, 0);
        
        RGB sepia_white = to_sepia(white);
        RGB sepia_black = to_sepia(black);
        RGB sepia_red = to_sepia(red);
        
        // Should produce valid colors (no overflow)
        CHECK(sepia_white.r <= 255);
        CHECK(sepia_white.g <= 255);
        CHECK(sepia_white.b <= 255);
        
        // Black should remain black
        CHECK(sepia_black.r == 0);
        CHECK(sepia_black.g == 0);
        CHECK(sepia_black.b == 0);
        
        // Sepia should maintain brownish characteristics
        CHECK(sepia_white.r >= sepia_white.g);
        CHECK(sepia_white.g >= sepia_white.b);
        CHECK(sepia_red.r >= sepia_red.g);
        CHECK(sepia_red.g >= sepia_red.b);
    }
    
    SUBCASE("Sepia Color Values") {
        RGB mid_gray(128, 128, 128);
        RGB sepia = to_sepia(mid_gray);
        
        // Check approximate sepia formula results
        // Sepia R = (R * 0.393) + (G * 0.769) + (B * 0.189)
        // Sepia G = (R * 0.349) + (G * 0.686) + (B * 0.168)  
        // Sepia B = (R * 0.272) + (G * 0.534) + (B * 0.131)
        
        double expected_r = (128 * 0.393) + (128 * 0.769) + (128 * 0.189);
        double expected_g = (128 * 0.349) + (128 * 0.686) + (128 * 0.168);
        double expected_b = (128 * 0.272) + (128 * 0.534) + (128 * 0.131);
        
        CHECK(std::abs(sepia.r - expected_r) <= 1);
        CHECK(std::abs(sepia.g - expected_g) <= 1);
        CHECK(std::abs(sepia.b - expected_b) <= 1);
    }
}

TEST_CASE("Palette Utilities Tests") {
    SUBCASE("Remove Duplicates") {
        std::vector<RGB> palette = {
            RGB(255, 0, 0),     // Red
            RGB(254, 1, 1),     // Very similar to red
            RGB(0, 255, 0),     // Green
            RGB(255, 0, 0),     // Exact duplicate of red
            RGB(0, 0, 255),     // Blue
            RGB(1, 254, 2)      // Very similar to green
        };
        
        std::vector<RGB> unique = remove_duplicates(palette, 5.0);
        
        // Should remove similar colors
        CHECK(unique.size() < palette.size());
        CHECK(unique.size() >= 3); // At least red, green, blue
        
        // Test with stricter threshold
        std::vector<RGB> strict_unique = remove_duplicates(palette, 1.0);
        CHECK(strict_unique.size() >= unique.size()); // More colors with stricter threshold
    }
    
    SUBCASE("Extract Dominant Colors") {
        std::vector<RGB> colors = {
            RGB(255, 0, 0), RGB(254, 1, 1), RGB(253, 2, 2), // Red family
            RGB(0, 255, 0), RGB(1, 254, 1), RGB(2, 253, 2), // Green family  
            RGB(0, 0, 255), RGB(1, 1, 254), RGB(2, 2, 253), // Blue family
            RGB(128, 128, 128), RGB(127, 127, 127)          // Gray family
        };
        
        std::vector<RGB> dominant = extract_dominant_colors(colors, 4);
        
        CHECK(dominant.size() == 4);
        
        // Should select representative colors from different families
        // (exact colors depend on algorithm, but should be diverse)
        for (size_t i = 0; i < dominant.size(); ++i) {
            for (size_t j = i + 1; j < dominant.size(); ++j) {
                // Dominant colors should be reasonably different
                CHECK(rgb_distance(dominant[i], dominant[j]) > 50.0);
            }
        }
    }
    
    SUBCASE("Extract Dominant Colors Edge Cases") {
        // Empty input
        std::vector<RGB> empty;
        std::vector<RGB> result_empty = extract_dominant_colors(empty, 3);
        CHECK(result_empty.empty());
        
        // Single color
        std::vector<RGB> single = {RGB(128, 64, 192)};
        std::vector<RGB> result_single = extract_dominant_colors(single, 3);
        CHECK(result_single.size() == 1);
        CHECK(result_single[0] == single[0]);
        
        // Request more colors than available
        std::vector<RGB> few = {RGB(255, 0, 0), RGB(0, 255, 0)};
        std::vector<RGB> result_few = extract_dominant_colors(few, 5);
        CHECK(result_few.size() == 2);
    }
    
    SUBCASE("Find Closest Color") {
        std::vector<RGB> palette = {
            RGB(255, 0, 0),   // Red
            RGB(0, 255, 0),   // Green  
            RGB(0, 0, 255),   // Blue
            RGB(0, 0, 0)      //# Black
        };
        
        RGB target_red(200, 50, 50);    // Reddish
        RGB target_green(50, 200, 50);  // Greenish
        RGB target_dark(30, 30, 30);    // Dark
        
        RGB closest_red = find_closest_color(target_red, palette);
        RGB closest_green = find_closest_color(target_green, palette);
        RGB closest_dark = find_closest_color(target_dark, palette);
        
        CHECK(closest_red == RGB(255, 0, 0));   // Should match red
        CHECK(closest_green == RGB(0, 255, 0)); // Should match green
        CHECK(closest_dark == RGB(0, 0, 0));    // Should match black
        
        // Empty palette should return target
        std::vector<RGB> empty_palette;
        RGB result = find_closest_color(target_red, empty_palette);
        CHECK(result == target_red);
    }
}