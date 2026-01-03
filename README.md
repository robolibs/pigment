<img align="right" width="26%" src="./misc/logo.png">

# Pigment

A modern, header-only C++20 library for comprehensive color manipulation, conversion, and analysis.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

## Features

- 🎨 **Multiple Color Spaces**: RGB, HSL, HSV, LAB, and Monochrome support
- 🔄 **Seamless Conversions**: Convert between any color spaces with high precision
- 🎭 **Color Harmonies**: Generate complementary, triadic, analogous, and other color schemes
- 🎨 **Color Palettes**: Create and manipulate color palettes with predefined schemes
- 🌈 **Named Colors**: 140+ predefined colors with familiar CSS-style names
- ♿ **Accessibility**: WCAG contrast ratio calculations and accessibility level checking
- 🌈 **Color Blindness**: Simulate different types of color vision deficiencies
- 🧮 **Mathematical Operations**: Add, subtract, mix, and scale colors
- 🔍 **Color Analysis**: Luminance, brightness, warmth, and perceptual difference calculations
- ⚡ **High Performance**: Header-only, optimized for speed and memory efficiency
- 🧪 **Thoroughly Tested**: Comprehensive test suite with 8000+ assertions

## Installation

### CMake (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
  pigment
  GIT_REPOSITORY https://github.com/bresilla/pigment.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(pigment)

target_link_libraries(your_target PRIVATE pigment::pigment)
```

### Manual Installation

```bash
git clone https://github.com/bresilla/pigment.git
cd pigment
mkdir build && cd build
cmake ..
make install
```

## Quick Start

```cpp
#include <pigment/pigment.hpp>
using namespace pigment;

// Create colors using predefined names
RGB red = colors::red();
RGB skyblue = colors::skyblue();
RGB lavender = colors::lavender();

// Create colors in different formats
RGB blue("#0000FF");
HSL cyan(180.0, 1.0, 0.5);
LAB lab(50.0, 20.0, -30.0);

// Convert between color spaces
HSL red_hsl = HSL::fromRGB(red);
RGB cyan_rgb = cyan.to_rgb();

// Color operations
RGB purple = red.mix(blue, 0.5);
RGB darker_red = red.darken(0.2);
RGB warmer_red = red.warm(0.1);

// Generate color palettes
auto gradient = Palette::gradient(red, blue, 10);
auto material_colors = Palette::material_design();
auto harmonious = Palette::analogous(red, 5);

// Accessibility and analysis
double contrast = utils::contrast_ratio(red, colors::white());
auto accessibility_level = utils::check_accessibility(red, colors::white());
bool is_light = red.is_light();
```

## Named Colors

### Using Named Colors
```cpp
#include <pigment/named_colors.hpp>
using namespace pigment;

// Get colors using function calls
RGB red = colors::red();
RGB skyblue = colors::skyblue();
RGB turquoise = colors::turquoise();

// Create palettes with named colors
Palette palette({
    colors::red(),
    colors::green(),
    colors::blue()
});

// Create gradients between named colors
auto gradient = Palette::gradient(
    colors::red(),
    colors::blue(),
    5
);

// Mix named colors
auto purple = colors::red().mix(colors::blue(), 0.5);
```

### Available Color Categories
The library includes 140 named colors organized in 10 categories:

1. **Reds** (9 colors)
   ```cpp
   colors::indianred()      // #CD5C5C
   colors::lightcoral()     // #F08080
   colors::salmon()         // #FA8072
   colors::darksalmon()     // #E9967A
   colors::lightsalmon()    // #FFA07A
   colors::crimson()        // #DC143C
   colors::red()           // #FF0000
   colors::firebrick()     // #B22222
   colors::darkred()       // #8B0000
   ```

2. **Pinks** (6 colors)
   ```cpp
   colors::pink()           // #FFC0CB
   colors::lightpink()      // #FFB6C1
   colors::hotpink()        // #FF69B4
   colors::deeppink()       // #FF1493
   colors::mediumvioletred() // #C71585
   colors::palevioletred()  // #DB7093
   ```

3. **Oranges** (5 colors)
   ```cpp
   colors::coral()          // #FF7F50
   colors::tomato()         // #FF6347
   colors::orangered()      // #FF4500
   colors::darkorange()     // #FF8C00
   colors::orange()         // #FFA500
   ```

[Additional color categories follow the same pattern...]

## Color Types

### RGB (Red, Green, Blue)
```cpp
// Construction
RGB color1(255, 128, 64);          // From values
RGB color2("#FF8040");             // From hex string
RGB color3("#F84");                // Short hex format
RGB color4("FF8040");              // Hex without #
RGB transparent(255, 0, 0, 128);   // With alpha channel
RGB named = colors::skyblue();     // From named color function

// Operations
std::string hex = color1.to_hex();           // Convert to hex
RGB mixed = color1.mix(color2, 0.3);         // Mix colors
RGB sum = color1 + color2;                   // Add colors
RGB scaled = color1 * 0.5;                   // Scale brightness
RGB inverted = color1.invert();              // Invert colors
RGB grayscale = color1.to_grayscale();       // Convert to grayscale

// Analysis
double luminance = color1.luminance();       // Calculate luminance
bool is_light = color1.is_light();           // Check if light/dark
RGB contrasted = color1.adjust_contrast(0.5);
```

[Rest of the README remains the same...]