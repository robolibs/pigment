#include <iomanip>
#include <iostream>
#include <pigment/pigment.hpp>

int main() {
    using namespace pigment;

    std::cout << "=== Pigment Library Demo ===" << std::endl;

    // Basic RGB operations
    std::cout << "\n--- RGB Operations ---" << std::endl;
    RGB red("#FF0000");
    RGB blue("#0000FF");
    std::cout << "Red: " << red.to_hex() << std::endl;
    std::cout << "Blue: " << blue.to_hex() << std::endl;
    std::cout << "Mixed (50/50): " << red.mix(blue).to_hex() << std::endl;
    std::cout << "Red brightness: " << red.brighten().to_hex() << std::endl;
    std::cout << "Red luminance: " << red.luminance() << std::endl;

    // HSL operations
    std::cout << "\n--- HSL Operations ---" << std::endl;
    HSL hsl = HSL::fromRGB(red);
    std::cout << "Red in HSL: H=" << hsl.get_h() << " S=" << hsl.get_s() << " L=" << hsl.get_l() << std::endl;
    std::cout << "Complement: " << hsl.complement().to_rgb().to_hex() << std::endl;
    std::cout << "Lighter: " << hsl.lighten(0.2).to_rgb().to_hex() << std::endl;

    // LAB operations
    std::cout << "\n--- LAB Operations ---" << std::endl;
    LAB lab1 = LAB::fromRGB(red);
    LAB lab2 = LAB::fromRGB(blue);
    std::cout << "Color difference (Delta E): " << lab1.delta_e(lab2) << std::endl;
    std::cout << "Are similar? " << (lab1.is_similar(lab2) ? "Yes" : "No") << std::endl;

    // Palette operations
    std::cout << "\n--- Palette Operations ---" << std::endl;
    auto material_palette = Palette::material_design();
    std::cout << "Material Design palette size: " << material_palette.size() << std::endl;
    std::cout << "Random material color: " << material_palette.random().to_hex() << std::endl;

    auto gradient = Palette::gradient(RGB::red(), RGB::blue(), 5);
    std::cout << "Red to Blue gradient: ";
    for (const auto &color : gradient) {
        std::cout << color.to_hex() << " ";
    }
    std::cout << std::endl;

    // Utility functions
    std::cout << "\n--- Utility Functions ---" << std::endl;
    RGB background("#FFFFFF");
    RGB foreground("#000000");
    double contrast = utils::contrast_ratio(foreground, background);
    std::cout << "Black on white contrast ratio: " << std::fixed << std::setprecision(2) << contrast << std::endl;

    auto accessibility = utils::check_accessibility(foreground, background);
    std::cout << "Accessibility level: " << accessibility << std::endl;

    std::cout << "Best contrast for red background: " << utils::best_contrast_color(red).to_hex() << std::endl;

    // Color harmonies
    std::cout << "\n--- Color Harmonies ---" << std::endl;
    auto triadic_colors = utils::generate_harmony(RGB("#3498db"), "triadic");
    std::cout << "Triadic harmony: ";
    for (const auto &color : triadic_colors) {
        std::cout << color.to_hex() << " ";
    }
    std::cout << std::endl;

    // Color blindness simulation
    std::cout << "\n--- Color Blindness Simulation ---" << std::endl;
    RGB original("#FF6B6B");
    RGB protanopia = utils::ColorBlindness::simulate(original, utils::ColorBlindness::PROTANOPIA);
    std::cout << "Original: " << original.to_hex() << " -> Protanopia: " << protanopia.to_hex() << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}