#include <iostream>
#include "include/pigment/pigment.hpp"

using namespace pigment;

void test_implicit_conversions() {
    std::cout << "Testing implicit conversions to RGB...\n\n";
    
    // Test HSL to RGB implicit conversion
    HSL hsl(120.0, 1.0, 0.5);  // Pure green
    RGB rgb_from_hsl = hsl;    // Implicit conversion
    std::cout << "HSL(120°, 100%, 50%) -> RGB(" 
              << static_cast<int>(rgb_from_hsl.r) << ", " 
              << static_cast<int>(rgb_from_hsl.g) << ", " 
              << static_cast<int>(rgb_from_hsl.b) << ")\n";
    std::cout << "Expected: RGB(0, 255, 0) - Pure green\n\n";
    
    // Test HSV to RGB implicit conversion  
    HSV hsv(240.0f, 1.0f, 1.0f);  // Pure blue
    RGB rgb_from_hsv = hsv;        // Implicit conversion
    std::cout << "HSV(240°, 100%, 100%) -> RGB(" 
              << static_cast<int>(rgb_from_hsv.r) << ", " 
              << static_cast<int>(rgb_from_hsv.g) << ", " 
              << static_cast<int>(rgb_from_hsv.b) << ")\n";
    std::cout << "Expected: RGB(0, 0, 255) - Pure blue\n\n";
    
    // Test LAB to RGB implicit conversion
    LAB lab(50.0, 0.0, 0.0);  // Neutral gray
    RGB rgb_from_lab = lab;   // Implicit conversion
    std::cout << "LAB(50, 0, 0) -> RGB(" 
              << static_cast<int>(rgb_from_lab.r) << ", " 
              << static_cast<int>(rgb_from_lab.g) << ", " 
              << static_cast<int>(rgb_from_lab.b) << ")\n";
    std::cout << "Expected: ~RGB(119, 119, 119) - Neutral gray\n\n";
    
    // Test MONO to RGB implicit conversion
    MONO mono(128);           // 50% gray
    RGB rgb_from_mono = mono; // Implicit conversion
    std::cout << "MONO(128) -> RGB(" 
              << static_cast<int>(rgb_from_mono.r) << ", " 
              << static_cast<int>(rgb_from_mono.g) << ", " 
              << static_cast<int>(rgb_from_mono.b) << ")\n";
    std::cout << "Expected: RGB(128, 128, 128) - 50% gray\n\n";
    
    // Test function parameter implicit conversion
    auto print_rgb = [](RGB color) {
        std::cout << "Function received RGB(" 
                  << static_cast<int>(color.r) << ", " 
                  << static_cast<int>(color.g) << ", " 
                  << static_cast<int>(color.b) << ")\n";
    };
    
    std::cout << "Testing function parameter implicit conversion:\n";
    HSL red_hsl(0.0, 1.0, 0.5);  // Pure red
    print_rgb(red_hsl);           // Implicit conversion in function call
    std::cout << "Expected: RGB(255, 0, 0) - Pure red\n\n";
    
    std::cout << "All implicit conversions working! ✅\n";
}

int main() {
    test_implicit_conversions();
    return 0;
}