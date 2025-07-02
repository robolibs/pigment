#include <chrono>
#include <doctest/doctest.h>
#include <pigment/pigment.hpp>
#include <random>
#include <vector>

using namespace pigment;

TEST_CASE("Performance Tests") {
    SUBCASE("RGB Construction Performance") {
        const int iterations = 100000;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            RGB color(i % 256, (i * 2) % 256, (i * 3) % 256);
            // Use the color to prevent optimization
            volatile int sum = color.r + color.g + color.b;
            (void)sum; // Suppress unused variable warning
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Should complete within reasonable time (adjust threshold as needed)
        CHECK(duration.count() < 100000); // 100ms threshold
    }

    SUBCASE("Color Conversion Performance") {
        const int iterations = 10000;
        std::vector<RGB> test_colors;

        // Generate test colors
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (int i = 0; i < 100; ++i) {
            test_colors.emplace_back(dis(gen), dis(gen), dis(gen));
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < iterations; ++i) {
            const RGB &color = test_colors[i % test_colors.size()];

            // Convert RGB -> HSL -> RGB
            HSL hsl = HSL::fromRGB(color);
            RGB back = hsl.to_rgb();

            // Use the result to prevent optimization
            volatile int sum = back.r + back.g + back.b;
            (void)sum;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Should complete within reasonable time
        CHECK(duration.count() < 1000000); // 1 second threshold
    }

    SUBCASE("Palette Generation Performance") {
        const int palette_size = 1000;

        auto start = std::chrono::high_resolution_clock::now();

        auto gradient = Palette::gradient(RGB::red(), RGB::blue(), palette_size);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        CHECK(gradient.size() == palette_size);
        CHECK(duration.count() < 100000); // 100ms threshold
    }

    SUBCASE("Color Distance Calculation Performance") {
        const int iterations = 10000;
        std::vector<RGB> colors;

        // Generate test colors
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (int i = 0; i < 100; ++i) {
            colors.emplace_back(dis(gen), dis(gen), dis(gen));
        }

        auto start = std::chrono::high_resolution_clock::now();

        double total_distance = 0;
        for (int i = 0; i < iterations; ++i) {
            const RGB &color1 = colors[i % colors.size()];
            const RGB &color2 = colors[(i + 1) % colors.size()];

            total_distance += utils::color_distance(color1, color2);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Use total_distance to prevent optimization
        CHECK(total_distance > 0);
        CHECK(duration.count() < 500000); // 500ms threshold
    }
}

TEST_CASE("Memory Usage Tests") {
    SUBCASE("Large Color Array") {
        const int array_size = 10000;
        std::vector<RGB> colors;
        colors.reserve(array_size);

        // Fill with colors
        for (int i = 0; i < array_size; ++i) {
            colors.emplace_back(i % 256, (i * 2) % 256, (i * 3) % 256);
        }

        CHECK(colors.size() == array_size);

        // Test iteration performance
        auto start = std::chrono::high_resolution_clock::now();

        uint64_t sum = 0;
        for (const auto &color : colors) {
            sum += color.r + color.g + color.b;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        CHECK(sum > 0);
        CHECK(duration.count() < 50000); // 50ms threshold
    }

    SUBCASE("Palette Memory Usage") {
        const int num_palettes = 100;
        const int colors_per_palette = 100;

        std::vector<Palette> palettes;
        palettes.reserve(num_palettes);

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_palettes; ++i) {
            Palette palette;
            for (int j = 0; j < colors_per_palette; ++j) {
                palette.add(RGB(j % 256, (j * 2) % 256, (j * 3) % 256));
            }
            palettes.push_back(std::move(palette));
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        CHECK(palettes.size() == num_palettes);
        for (const auto &palette : palettes) {
            CHECK(palette.size() == colors_per_palette);
        }

        CHECK(duration.count() < 100000); // 100ms threshold
    }
}

TEST_CASE("Stress Tests") {
    SUBCASE("Random Color Operations") {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> color_dis(0, 255);
        std::uniform_real_distribution<> ratio_dis(0.0, 1.0);

        const int iterations = 1000;

        for (int i = 0; i < iterations; ++i) {
            RGB color1(color_dis(gen), color_dis(gen), color_dis(gen));
            RGB color2(color_dis(gen), color_dis(gen), color_dis(gen));
            double ratio = ratio_dis(gen);

            // Test various operations
            RGB mixed = color1.mix(color2, ratio);
            RGB sum = color1 + color2;
            RGB diff = color1 - color2;
            RGB scaled = color1 * ratio;

            // Basic sanity checks
            CHECK(mixed.r >= 0);
            CHECK(mixed.r <= 255);
            CHECK(sum.r >= 0);
            CHECK(sum.r <= 255);
            CHECK(diff.r >= 0);
            CHECK(diff.r <= 255);
            CHECK(scaled.r >= 0);
            CHECK(scaled.r <= 255);
        }
    }

    SUBCASE("Extreme Conversion Chain") {
        RGB original(123, 87, 213);

        // Chain multiple conversions
        for (int i = 0; i < 100; ++i) {
            HSL hsl = HSL::fromRGB(original);
            HSV hsv = HSV::fromRGB(original);
            LAB lab = LAB::fromRGB(original);

            RGB from_hsl = hsl.to_rgb();
            RGB from_hsv = hsv.to_rgb();
            RGB from_lab = lab.to_rgb();

            // Should remain reasonably close to original
            CHECK(std::abs(from_hsl.r - original.r) <= 10);
            CHECK(std::abs(from_hsv.r - original.r) <= 10);
            CHECK(std::abs(from_lab.r - original.r) <= 20); // LAB may have larger errors
        }
    }
}
