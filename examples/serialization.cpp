#include <iomanip>
#include <iostream>
#include <pigment/pigment.hpp>

#include <datapod/datapod.hpp>

int main() {
    using namespace pigment;
    namespace dp = datapod;

    std::cout << "=== Pigment Serialization Demo ===" << std::endl;

    // -------------------------------------------------------------------------
    // Basic RGB serialization
    // -------------------------------------------------------------------------
    std::cout << "\n--- Basic RGB Serialization ---" << std::endl;

    RGB original(255, 128, 64);
    std::cout << "Original color: " << original.to_hex() << std::endl;
    std::cout << "  RGBA: (" << (int)original.r() << ", " << (int)original.g() << ", " << (int)original.b() << ", "
              << (int)original.a() << ")" << std::endl;

    // Serialize to binary buffer
    dp::ByteBuf buffer = dp::serialize(original);
    std::cout << "Serialized size: " << buffer.size() << " bytes" << std::endl;

    // Deserialize back
    RGB restored = dp::deserialize<dp::Mode::NONE, RGB>(buffer);
    std::cout << "Restored color: " << restored.to_hex() << std::endl;
    std::cout << "  Match: " << (original == restored ? "YES" : "NO") << std::endl;

    // -------------------------------------------------------------------------
    // MONO serialization
    // -------------------------------------------------------------------------
    std::cout << "\n--- MONO Serialization ---" << std::endl;

    MONO gray(128);
    std::cout << "Original gray: " << (int)gray.v() << std::endl;

    dp::ByteBuf mono_buf = dp::serialize(gray);
    std::cout << "Serialized size: " << mono_buf.size() << " bytes" << std::endl;

    MONO restored_gray = dp::deserialize<dp::Mode::NONE, MONO>(mono_buf);
    std::cout << "Restored gray: " << (int)restored_gray.v() << std::endl;
    std::cout << "  Match: " << (gray == restored_gray ? "YES" : "NO") << std::endl;

    // -------------------------------------------------------------------------
    // Serialization with version checking
    // -------------------------------------------------------------------------
    std::cout << "\n--- Serialization with Version Check ---" << std::endl;

    RGB color_v(100, 150, 200);
    dp::ByteBuf versioned_buf = dp::serialize<dp::Mode::WITH_VERSION>(color_v);
    std::cout << "Versioned buffer size: " << versioned_buf.size() << " bytes" << std::endl;

    RGB restored_v = dp::deserialize<dp::Mode::WITH_VERSION, RGB>(versioned_buf);
    std::cout << "Original: " << color_v.to_hex() << " -> Restored: " << restored_v.to_hex() << std::endl;
    std::cout << "  Match: " << (color_v == restored_v ? "YES" : "NO") << std::endl;

    // -------------------------------------------------------------------------
    // Serialization with integrity check
    // -------------------------------------------------------------------------
    std::cout << "\n--- Serialization with Integrity Check ---" << std::endl;

    RGB color_i(50, 100, 150);
    dp::ByteBuf integrity_buf = dp::serialize<dp::Mode::WITH_INTEGRITY>(color_i);
    std::cout << "Integrity buffer size: " << integrity_buf.size() << " bytes" << std::endl;

    RGB restored_i = dp::deserialize<dp::Mode::WITH_INTEGRITY, RGB>(integrity_buf);
    std::cout << "Original: " << color_i.to_hex() << " -> Restored: " << restored_i.to_hex() << std::endl;
    std::cout << "  Match: " << (color_i == restored_i ? "YES" : "NO") << std::endl;

    // -------------------------------------------------------------------------
    // Serializing a vector of colors
    // -------------------------------------------------------------------------
    std::cout << "\n--- Vector of Colors Serialization ---" << std::endl;

    dp::Vector<RGB> palette;
    palette.push_back(RGB::red());
    palette.push_back(RGB::green());
    palette.push_back(RGB::blue());
    palette.push_back(RGB("#FF6B6B"));
    palette.push_back(RGB("#4ECDC4"));

    std::cout << "Original palette (" << palette.size() << " colors): ";
    for (const auto &c : palette) {
        std::cout << c.to_hex() << " ";
    }
    std::cout << std::endl;

    dp::ByteBuf palette_buf = dp::serialize(palette);
    std::cout << "Serialized size: " << palette_buf.size() << " bytes" << std::endl;

    auto restored_palette = dp::deserialize<dp::Mode::NONE, dp::Vector<RGB>>(palette_buf);
    std::cout << "Restored palette (" << restored_palette.size() << " colors): ";
    for (const auto &c : restored_palette) {
        std::cout << c.to_hex() << " ";
    }
    std::cout << std::endl;

    // -------------------------------------------------------------------------
    // HSL serialization
    // -------------------------------------------------------------------------
    std::cout << "\n--- HSL Serialization ---" << std::endl;

    HSL hsl(180.0, 0.5, 0.6);
    std::cout << "Original HSL: H=" << hsl.get_h() << " S=" << hsl.get_s() << " L=" << hsl.get_l() << std::endl;

    dp::ByteBuf hsl_buf = dp::serialize(hsl);
    std::cout << "Serialized size: " << hsl_buf.size() << " bytes" << std::endl;

    HSL restored_hsl = dp::deserialize<dp::Mode::NONE, HSL>(hsl_buf);
    std::cout << "Restored HSL: H=" << restored_hsl.get_h() << " S=" << restored_hsl.get_s()
              << " L=" << restored_hsl.get_l() << std::endl;

    // -------------------------------------------------------------------------
    // Hex dump of serialized data
    // -------------------------------------------------------------------------
    std::cout << "\n--- Hex Dump of RGB Buffer ---" << std::endl;

    RGB demo_color(0xAB, 0xCD, 0xEF, 0xFF);
    dp::ByteBuf demo_buf = dp::serialize(demo_color);

    std::cout << "Color: " << demo_color.to_hex(true) << std::endl;
    std::cout << "Bytes: ";
    for (size_t i = 0; i < demo_buf.size(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)demo_buf[i] << " ";
    }
    std::cout << std::dec << std::endl;

    std::cout << "\n=== Serialization Demo Complete ===" << std::endl;
    return 0;
}
