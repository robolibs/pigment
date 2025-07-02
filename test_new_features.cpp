#include <iostream>
#include <pigment/pigment.hpp>

using namespace pigment;

int main() {
    std::cout << "=== Testing New Color Spaces ===\n\n";
    
    RGB red = RGB::red();
    std::cout << "Original RGB: " << red.to_hex() << "\n";
    
    // Test XYZ conversion
    XYZ xyz = XYZ::fromRGB(red);
    RGB back_from_xyz = xyz.to_rgb();
    std::cout << "XYZ roundtrip: " << back_from_xyz.to_hex() << "\n";
    
    // Test OKLAB conversion
    OKLAB oklab = OKLAB::fromRGB(red);
    RGB back_from_oklab = oklab.to_rgb();
    std::cout << "OKLAB roundtrip: " << back_from_oklab.to_hex() << "\n";
    
    // Test LCH conversion
    LCH lch = LCH::fromRGB(red);
    RGB back_from_lch = lch.to_rgb();
    std::cout << "LCH roundtrip: " << back_from_lch.to_hex() << "\n";
    
    std::cout << "\n=== Testing New Color Harmonies ===\n\n";
    
    // Test monochromatic scheme
    auto mono = utils::generate_monochromatic(red, 5);
    std::cout << "Monochromatic: ";
    for (const auto& color : mono) {
        std::cout << color.to_hex() << " ";
    }
    std::cout << "\n";
    
    // Test golden ratio scheme
    auto golden = utils::generate_golden_ratio_scheme(red, 5);
    std::cout << "Golden ratio: ";
    for (const auto& color : golden) {
        std::cout << color.to_hex() << " ";
    }
    std::cout << "\n";
    
    std::cout << "\n=== Testing Constexpr Functions ===\n";
    
    constexpr RGB white = RGB::white();
    constexpr RGB black = RGB::black();
    constexpr RGB mixed = white.mix(black, 0.5);
    constexpr RGB inverted = white.invert();
    
    std::cout << "Constexpr white: " << white.to_hex() << "\n";
    std::cout << "Constexpr black: " << black.to_hex() << "\n";
    std::cout << "Constexpr mixed: " << mixed.to_hex() << "\n";
    std::cout << "Constexpr inverted: " << inverted.to_hex() << "\n";
    
    return 0;
}
EOF < /dev/null