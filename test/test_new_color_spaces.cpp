#include <cmath>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>

using namespace pigment;

TEST_CASE("XYZ Color Space Tests") {
    SUBCASE("XYZ RGB Conversion") {
        RGB red = RGB::red();
        XYZ xyz_red = XYZ::fromRGB(red);
        RGB back_to_rgb = xyz_red.to_rgb();

        // Allow small rounding errors (within 2 units)
        CHECK(std::abs(back_to_rgb.r - red.r) <= 2);
        CHECK(std::abs(back_to_rgb.g - red.g) <= 2);
        CHECK(std::abs(back_to_rgb.b - red.b) <= 2);

        // Test known values for white
        RGB white = RGB::white();
        XYZ xyz_white = XYZ::fromRGB(white);
        CHECK(xyz_white.X > 90.0);  // Should be around 95.047
        CHECK(xyz_white.Y > 95.0);  // Should be around 100
        CHECK(xyz_white.Z > 100.0); // Should be around 108.883
    }

    SUBCASE("XYZ Equality") {
        XYZ xyz1(50.0, 60.0, 70.0);
        XYZ xyz2(50.0, 60.0, 70.0);
        XYZ xyz3(50.1, 60.0, 70.0);

        CHECK(xyz1 == xyz2);
        CHECK(xyz1 != xyz3);
    }

    SUBCASE("XYZ Luminance") {
        XYZ xyz(50.0, 75.0, 40.0);
        CHECK(xyz.luminance() == 75.0);
    }
}

TEST_CASE("OKLAB Color Space Tests") {
    SUBCASE("OKLAB RGB Conversion") {
        RGB blue = RGB::blue();
        OKLAB oklab_blue = OKLAB::fromRGB(blue);
        RGB back_to_rgb = oklab_blue.to_rgb();

        // Allow small rounding errors (within 3 units due to complex conversion)
        CHECK(std::abs(back_to_rgb.r - blue.r) <= 3);
        CHECK(std::abs(back_to_rgb.g - blue.g) <= 3);
        CHECK(std::abs(back_to_rgb.b - blue.b) <= 3);
    }

    SUBCASE("OKLAB Properties") {
        RGB red = RGB::red();
        OKLAB oklab = OKLAB::fromRGB(red);

        // Lightness should be reasonable
        CHECK(oklab.lightness() > 0.0);
        CHECK(oklab.lightness() < 1.0);

        // Chroma should be positive for red
        CHECK(oklab.chroma() > 0.0);

        // Hue should be reasonable
        double hue_deg = oklab.hue_degrees();
        CHECK(hue_deg >= 0.0);
        CHECK(hue_deg < 360.0);
    }

    SUBCASE("OKLAB Adjustments") {
        RGB color(128, 64, 192);
        OKLAB oklab = OKLAB::fromRGB(color);

        // Test lightness adjustment
        OKLAB lighter = oklab.adjust_lightness(0.1);
        CHECK(lighter.lightness() > oklab.lightness());

        OKLAB darker = oklab.adjust_lightness(-0.1);
        CHECK(darker.lightness() < oklab.lightness());

        // Test chroma adjustment
        OKLAB more_saturated = oklab.adjust_chroma(1.2);
        CHECK(more_saturated.chroma() > oklab.chroma());

        // Test hue rotation
        OKLAB rotated = oklab.rotate_hue(45.0);
        double hue_diff = std::abs(rotated.hue_degrees() - oklab.hue_degrees());
        if (hue_diff > 180.0) hue_diff = 360.0 - hue_diff;
        CHECK(std::abs(hue_diff - 45.0) < 2.0);
    }

    SUBCASE("OKLAB Distance") {
        OKLAB color1 = OKLAB::fromRGB(RGB::red());
        OKLAB color2 = OKLAB::fromRGB(RGB::blue());
        OKLAB color3 = OKLAB::fromRGB(RGB(255, 0, 1)); // Very similar to red

        double dist_red_blue = color1.distance(color2);
        double dist_red_similar = color1.distance(color3);

        CHECK(dist_red_blue > dist_red_similar);
        CHECK(dist_red_similar < 0.1); // Should be very small
    }
}

TEST_CASE("LCH Color Space Tests") {
    SUBCASE("LCH LAB Conversion") {
        LAB lab(50.0, 20.0, -30.0);
        LCH lch = LCH::fromLAB(lab);
        LAB back_to_lab = lch.to_lab();

        // Check roundtrip conversion
        CHECK(std::abs(back_to_lab.l - lab.l) < 0.01);
        CHECK(std::abs(back_to_lab.a - lab.a) < 0.01);
        CHECK(std::abs(back_to_lab.b - lab.b) < 0.01);
    }

    SUBCASE("LCH RGB Conversion") {
        RGB green = RGB::green();
        LCH lch_green = LCH::fromRGB(green);
        RGB back_to_rgb = lch_green.to_rgb();

        // Allow for conversion errors
        CHECK(std::abs(back_to_rgb.r - green.r) <= 5);
        CHECK(std::abs(back_to_rgb.g - green.g) <= 5);
        CHECK(std::abs(back_to_rgb.b - green.b) <= 5);
    }

    SUBCASE("LCH Properties") {
        RGB red = RGB::red();
        LCH lch = LCH::fromRGB(red);

        CHECK(lch.lightness() >= 0.0);
        CHECK(lch.lightness() <= 100.0);
        CHECK(lch.chroma() >= 0.0);
        CHECK(lch.hue() >= 0.0);
        CHECK(lch.hue() < 360.0);
    }

    SUBCASE("LCH Adjustments") {
        RGB color(100, 150, 200);
        LCH lch = LCH::fromRGB(color);

        // Test lightness adjustment
        LCH lighter = lch.adjust_lightness(10.0);
        CHECK(lighter.lightness() > lch.lightness());

        // Test chroma adjustment
        LCH more_saturated = lch.adjust_chroma(10.0);
        CHECK(more_saturated.chroma() > lch.chroma());

        // Test hue rotation
        LCH rotated = lch.rotate_hue(30.0);
        double expected_hue = lch.hue() + 30.0;
        if (expected_hue >= 360.0) expected_hue -= 360.0;
        CHECK(std::abs(rotated.hue() - expected_hue) < 1.0);
    }

    SUBCASE("LCH Color Harmonies") {
        RGB base(200, 100, 50);
        LCH lch = LCH::fromRGB(base);

        // Test complementary
        LCH complement = lch.complement();
        double hue_diff = std::abs(complement.hue() - lch.hue());
        if (hue_diff > 180.0) hue_diff = 360.0 - hue_diff;
        CHECK(std::abs(hue_diff - 180.0) < 2.0);

        // Test analogous
        auto analogous = lch.analogous();
        CHECK(analogous.first.hue() != lch.hue());
        CHECK(analogous.second.hue() != lch.hue());

        // Test triadic
        auto triadic = lch.triadic();
        CHECK(std::get<0>(triadic).hue() != lch.hue());
        CHECK(std::get<1>(triadic).hue() != lch.hue());
    }

    SUBCASE("LCH Distance") {
        LCH color1 = LCH::fromRGB(RGB::red());
        LCH color2 = LCH::fromRGB(RGB::green());
        LCH color3 = LCH::fromRGB(RGB(255, 0, 0)); // Same as red

        double dist1 = color1.distance(color2);
        double dist2 = color1.distance(color3);

        CHECK(dist1 > dist2);
        CHECK(dist2 < 1.0); // Should be very small
    }
}

TEST_CASE("Color Space Normalization") {
    SUBCASE("LCH Normalization") {
        LCH lch(400.0, -10.0, 450.0); // Invalid values
        CHECK(lch.L == 100.0); // Clamped
        CHECK(lch.C == 0.0);   // Clamped
        CHECK(lch.H == 90.0);  // Wrapped (450 - 360 = 90)
    }

    SUBCASE("XYZ Normalization") {
        XYZ xyz(-10.0, 50.0, 200.0);
        xyz.normalize();
        CHECK(xyz.X == 0.0); // Clamped
        CHECK(xyz.Y == 50.0); // Unchanged
        CHECK(xyz.Z == 200.0); // Unchanged (no upper limit)
    }
}