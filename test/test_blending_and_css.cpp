#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;

TEST_CASE("Color Blending Modes") {
    SUBCASE("Blend Add") {
        RGB red(255, 0, 0);
        RGB green(0, 255, 0);
        RGB result = red.blend_add(green);
        
        CHECK(result.r == 255);
        CHECK(result.g == 255);
        CHECK(result.b == 0);
        
        // Test overflow clamping
        RGB bright1(200, 150, 100);
        RGB bright2(100, 150, 200);
        RGB clamped = bright1.blend_add(bright2);
        
        CHECK(clamped.r <= 255);
        CHECK(clamped.g <= 255);
        CHECK(clamped.b <= 255);
    }

    SUBCASE("Blend Subtract") {
        RGB color1(200, 150, 100);
        RGB color2(50, 100, 150);
        RGB result = color1.blend_subtract(color2);
        
        CHECK(result.r == 150); // 200 - 50
        CHECK(result.g == 50);  // 150 - 100
        CHECK(result.b == 0);   // 100 - 150 clamped to 0
    }

    SUBCASE("Blend Multiply") {
        RGB color1(255, 128, 64);
        RGB color2(128, 255, 192);
        RGB result = color1.blend_multiply(color2);
        
        // Formula: (a * b) / 255
        CHECK(result.r == (255 * 128) / 255); // 128
        CHECK(result.g == (128 * 255) / 255); // 128
        CHECK(result.b == (64 * 192) / 255);  // 48
    }

    SUBCASE("Blend Screen") {
        RGB color1(128, 64, 32);
        RGB color2(64, 128, 96);
        RGB result = color1.blend_screen(color2);
        
        // Formula: 255 - ((255-a) * (255-b)) / 255
        uint8_t expected_r = 255 - ((255-128) * (255-64)) / 255;
        uint8_t expected_g = 255 - ((255-64) * (255-128)) / 255;
        uint8_t expected_b = 255 - ((255-32) * (255-96)) / 255;
        
        CHECK(result.r == expected_r);
        CHECK(result.g == expected_g);
        CHECK(result.b == expected_b);
    }

    SUBCASE("Blend Overlay") {
        RGB base(100, 200, 50);
        RGB blend(150, 75, 180);
        RGB result = base.blend_overlay(blend);
        
        // For base < 128: (2 * base * blend) / 255
        // For base >= 128: 255 - (2 * (255-base) * (255-blend)) / 255
        uint8_t expected_r = (2 * 100 * 150) / 255; // base < 128
        uint8_t expected_g = 255 - (2 * (255-200) * (255-75)) / 255; // base >= 128
        uint8_t expected_b = (2 * 50 * 180) / 255; // base < 128
        
        CHECK(result.r == expected_r);
        CHECK(result.g == expected_g);
        CHECK(result.b == expected_b);
    }

    SUBCASE("Alpha Blending Simple") {
        RGB foreground(255, 0, 0, 128); // 50% transparent red
        RGB background(0, 255, 0, 255);  // Opaque green
        RGB result = foreground.alpha_blend_simple(background);
        
        // Should be approximately half red, half green
        CHECK(result.r > 100);
        CHECK(result.r < 150);
        CHECK(result.g > 100);
        CHECK(result.g < 150);
        CHECK(result.b == 0);
        CHECK(result.a == 255); // Result should be opaque
    }

    SUBCASE("Alpha Blending Full") {
        RGB fg(255, 0, 0, 128);   // 50% transparent red
        RGB bg(0, 255, 0, 128);   // 50% transparent green
        RGB result = fg.alpha_blend(bg);
        
        // Result should have some transparency
        CHECK(result.a < 255);
        CHECK(result.a > 128);
        
        // Should have both red and green components
        CHECK(result.r > 0);
        CHECK(result.g > 0);
    }

    SUBCASE("Alpha Utilities") {
        RGB color(100, 150, 200, 180);
        
        CHECK(color.is_transparent());
        CHECK_FALSE(color.is_opaque());
        CHECK(color.transparency() > 0.0);
        CHECK(color.transparency() < 1.0);
        
        RGB opaque = color.with_alpha(255);
        CHECK(opaque.is_opaque());
        CHECK_FALSE(opaque.is_transparent());
        CHECK(opaque.transparency() == 0.0);
    }
}

TEST_CASE("CSS Color Parsing") {
    SUBCASE("RGB CSS Parsing") {
        // Basic rgb() format
        RGB color1("rgb(255, 128, 64)");
        CHECK(color1.r == 255);
        CHECK(color1.g == 128);
        CHECK(color1.b == 64);
        CHECK(color1.a == 255);
        
        // rgba() format
        RGB color2("rgba(100, 200, 50, 0.5)");
        CHECK(color2.r == 100);
        CHECK(color2.g == 200);
        CHECK(color2.b == 50);
        CHECK(color2.a == 127); // 0.5 * 255
        
        // No spaces
        RGB color3("rgb(0,255,128)");
        CHECK(color3.r == 0);
        CHECK(color3.g == 255);
        CHECK(color3.b == 128);
        
        // Extra spaces
        RGB color4("rgb( 255 , 0 , 255 )");
        CHECK(color4.r == 255);
        CHECK(color4.g == 0);
        CHECK(color4.b == 255);
    }

    SUBCASE("RGB CSS Edge Cases") {
        // Values out of range should be clamped
        RGB color1("rgb(300, -50, 400)");
        CHECK(color1.r == 255); // Clamped to max
        CHECK(color1.g == 0);   // Clamped to min
        CHECK(color1.b == 255); // Clamped to max
        
        // Alpha out of range
        RGB color2("rgba(100, 100, 100, 2.0)");
        CHECK(color2.a == 255); // Clamped to max
        
        RGB color3("rgba(100, 100, 100, -0.5)");
        CHECK(color3.a == 0); // Clamped to min
    }

    SUBCASE("HSL CSS Parsing") {
        // Basic hsl() format
        HSL color1("hsl(120, 50%, 75%)");
        CHECK(std::abs(color1.get_h() - 120.0) < 1.0);
        CHECK(std::abs(color1.get_s() - 0.5) < 0.01);
        CHECK(std::abs(color1.get_l() - 0.75) < 0.01);
        
        // hsla() format
        HSL color2("hsla(240, 100%, 50%, 0.8)");
        CHECK(std::abs(color2.get_h() - 240.0) < 1.0);
        CHECK(std::abs(color2.get_s() - 1.0) < 0.01);
        CHECK(std::abs(color2.get_l() - 0.5) < 0.01);
        
        // Without % signs
        HSL color3("hsl(180, 25, 60)");
        CHECK(std::abs(color3.get_h() - 180.0) < 1.0);
        CHECK(std::abs(color3.get_s() - 0.25) < 0.01);
        CHECK(std::abs(color3.get_l() - 0.6) < 0.01);
        
        // Hue wrapping
        HSL color4("hsl(420, 50%, 50%)");
        CHECK(std::abs(color4.get_h() - 60.0) < 1.0); // 420 - 360 = 60
    }

    SUBCASE("HSL CSS Edge Cases") {
        // Negative hue should wrap
        HSL color1("hsl(-60, 50%, 50%)");
        CHECK(std::abs(color1.get_h() - 300.0) < 1.0); // -60 + 360 = 300
        
        // Values out of range
        HSL color2("hsl(180, 150%, -20%)");
        CHECK(std::abs(color2.get_s() - 1.0) < 0.01); // Clamped to 100%
        CHECK(std::abs(color2.get_l() - 0.0) < 0.01); // Clamped to 0%
    }

    SUBCASE("HSV Hex String Support") {
        // HSV from hex
        HSV hsv1("#ff0000"); // Red
        CHECK(std::abs(hsv1.h - 0.0f) < 1.0f);
        CHECK(std::abs(hsv1.s - 1.0f) < 0.01f);
        CHECK(std::abs(hsv1.v - 1.0f) < 0.01f);
        
        HSV hsv2("#00ff00"); // Green
        CHECK(std::abs(hsv2.h - 120.0f) < 2.0f);
        CHECK(std::abs(hsv2.s - 1.0f) < 0.01f);
        CHECK(std::abs(hsv2.v - 1.0f) < 0.01f);
        
        // HSV to hex
        HSV hsv3(240.0f, 1.0f, 1.0f); // Blue
        std::string hex = hsv3.to_hex();
        CHECK(hex == "#0000ff");
        
        // Test short hex
        HSV hsv4("#f0f");
        std::string hex_back = hsv4.to_hex();
        CHECK(hex_back == "#ff00ff");
    }

    SUBCASE("Invalid CSS Format Handling") {
        // Invalid RGB format should throw
        CHECK_THROWS(RGB("invalid_format"));
        CHECK_THROWS(RGB("rgb(255, 128)"));      // Too few components
        CHECK_THROWS(RGB("rgb(255, 128, 64, 1, 2)")); // Too many components
        CHECK_THROWS(RGB("rgb(255 128 64)"));    // Missing commas
        
        // Invalid HSL format should throw
        CHECK_THROWS(HSL("invalid_format"));
        CHECK_THROWS(HSL("hsl(120, 50)"));       // Too few components
        CHECK_THROWS(HSL("rgb(120, 50%, 75%)")); // Wrong function name
    }

    SUBCASE("Hex Fallback") {
        // Should fall back to hex parsing for non-CSS strings
        RGB color1("#ff8040");
        CHECK(color1.r == 255);
        CHECK(color1.g == 128);
        CHECK(color1.b == 64);
        
        RGB color2("ff8040");
        CHECK(color2.r == 255);
        CHECK(color2.g == 128);
        CHECK(color2.b == 64);
    }
}