#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;

TEST_CASE("HSL Color Tests") {
    SUBCASE("HSL Construction and Normalization") {
        HSL hsl(120.0, 0.5, 0.7);
        CHECK(hsl.h == 12000);  // 120.0 * 100
        CHECK(hsl.s == 127);    // 0.5 * 255 ≈ 127
        CHECK(hsl.l == 178);    // 0.7 * 255 ≈ 178

        // Test normalization
        HSL hsl_wrap(370.0, 1.5, -0.1);
        CHECK(hsl_wrap.h == 1000); // (370 % 360) * 100 = 10 * 100
        CHECK(hsl_wrap.s == 255);  // Clamped to 255
        CHECK(hsl_wrap.l == 0);    // Clamped to 0
    }

    SUBCASE("HSL RGB Conversion") {
        RGB red = RGB::red();
        HSL hsl_red = HSL::fromRGB(red);
        RGB back_to_rgb = hsl_red.to_rgb();

        // Allow small rounding errors
        CHECK(std::abs(back_to_rgb.r - red.r) <= 1);
        CHECK(std::abs(back_to_rgb.g - red.g) <= 1);
        CHECK(std::abs(back_to_rgb.b - red.b) <= 1);

        // Test pure red
        CHECK(hsl_red.h == 0);      // 0.0 * 100
        CHECK(hsl_red.s == 255);    // 1.0 * 255
        CHECK(std::abs(hsl_red.l - 127) <= 2); // ~0.5 * 255, allow rounding
    }

    SUBCASE("HSL Color Adjustments") {
        HSL base(180.0, 0.5, 0.5);

        HSL hue_adjusted = base.adjust_hue(30.0);
        CHECK(hue_adjusted.h == 21000); // (180 + 30) * 100

        HSL saturated = base.saturate(0.2);
        CHECK(std::abs(saturated.s - 178) <= 2); // ~0.7 * 255, allow rounding

        HSL desaturated = base.desaturate(0.2);
        CHECK(std::abs(desaturated.s - 76) <= 2); // ~0.3 * 255, allow rounding

        HSL lighter = base.lighten(0.2);
        CHECK(std::abs(lighter.l - 178) <= 2); // ~0.7 * 255, allow rounding

        HSL darker = base.darken(0.2);
        CHECK(std::abs(darker.l - 76) <= 2); // ~0.3 * 255, allow rounding
    }

    SUBCASE("HSL Color Harmonies") {
        HSL base(120.0, 0.8, 0.6);

        HSL complement = base.complement();
        CHECK(std::abs(complement.h - 30000) <= 10); // 300.0 * 100, allow small variance

        auto triadic = base.triadic();
        CHECK(triadic.size() == 3);
        CHECK(triadic[0].h == base.h);
        CHECK(std::abs(triadic[1].h - (base.h + 12000)) <= 10); // +120.0 * 100
        // Handle hue wrapping: 120 + 240 = 360, which normalizes to 0
        uint16_t expected_h2 = (base.h + 24000) % 36000; // 240.0 * 100
        CHECK(std::abs(triadic[2].h - expected_h2) <= 10);

        auto analogous = base.analogous();
        CHECK(analogous.size() == 3);
        CHECK(analogous[1].h == base.h); // Middle one is the base

        auto split_comp = base.split_complementary();
        CHECK(split_comp.size() == 3);
        CHECK(split_comp[0].h == base.h);
    }
}
