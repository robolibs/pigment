#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;

TEST_CASE("HSV Color Tests") {
    SUBCASE("HSV Construction and Normalization") {
        HSV hsv(240.0f, 0.8f, 0.9f);
        CHECK(hsv.h == 240.0f);
        CHECK(hsv.s == 0.8f);
        CHECK(hsv.v == 0.9f);

        // Test normalization
        HSV hsv_wrap(400.0f, 1.2f, -0.1f);
        CHECK(hsv_wrap.h == 40.0f); // 400 - 360
        CHECK(hsv_wrap.s == 1.0f);  // Clamped
        CHECK(hsv_wrap.v == 0.0f);  // Clamped
    }

    SUBCASE("HSV RGB Conversion") {
        RGB blue = RGB::blue();
        HSV hsv_blue = HSV::fromRGB(blue);
        RGB back_to_rgb = hsv_blue.to_rgb();

        // Allow small rounding errors
        CHECK(std::abs(back_to_rgb.r - blue.r) <= 1);
        CHECK(std::abs(back_to_rgb.g - blue.g) <= 1);
        CHECK(std::abs(back_to_rgb.b - blue.b) <= 1);

        // Test pure blue
        CHECK(std::abs(hsv_blue.h - 240.0f) < 1.0f);
        CHECK(hsv_blue.s == 1.0f);
        CHECK(hsv_blue.v == 1.0f);
    }

    SUBCASE("HSV Adjustments") {
        HSV hsv(180.0f, 0.5f, 0.7f);

        hsv.adjustBrightness(0.2f);
        CHECK(hsv.v > 0.7f);

        HSV hsv2(180.0f, 0.5f, 0.7f);
        hsv2.adjustSaturation(0.3f);
        CHECK(hsv2.s > 0.5f);
    }
}
