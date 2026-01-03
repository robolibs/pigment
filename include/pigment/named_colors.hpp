#pragma once

#include "types_basic.hpp"
#include <string>
#include <unordered_map>

namespace pigment {
    namespace colors {

        // Red and Pink Variations
        // Encompasses various shades of red, from light pinks to deep crimsons
        inline RGB indianred() { return RGB("#CD5C5C"); }
        inline RGB lightcoral() { return RGB("#F08080"); }
        inline RGB salmon() { return RGB("#FA8072"); }
        inline RGB darksalmon() { return RGB("#E9967A"); }
        inline RGB lightsalmon() { return RGB("#FFA07A"); }
        inline RGB crimson() { return RGB("#DC143C"); }
        inline RGB red() { return RGB("#FF0000"); }
        inline RGB firebrick() { return RGB("#B22222"); }
        inline RGB darkred() { return RGB("#8B0000"); }
        inline RGB pink() { return RGB("#FFC0CB"); }
        inline RGB lightpink() { return RGB("#FFB6C1"); }
        inline RGB hotpink() { return RGB("#FF69B4"); }
        inline RGB deeppink() { return RGB("#FF1493"); }
        inline RGB mediumvioletred() { return RGB("#C71585"); }
        inline RGB palevioletred() { return RGB("#DB7093"); }

        // Orange and Yellow Hues
        // Warm colors ranging from coral to golden yellows
        inline RGB coral() { return RGB("#FF7F50"); }
        inline RGB tomato() { return RGB("#FF6347"); }
        inline RGB orangered() { return RGB("#FF4500"); }
        inline RGB darkorange() { return RGB("#FF8C00"); }
        inline RGB orange() { return RGB("#FFA500"); }
        inline RGB gold() { return RGB("#FFD700"); }
        inline RGB yellow() { return RGB("#FFFF00"); }
        inline RGB lightyellow() { return RGB("#FFFFE0"); }
        inline RGB lemonchiffon() { return RGB("#FFFACD"); }
        inline RGB lightgoldenrodyellow() { return RGB("#FAFAD2"); }
        inline RGB papayawhip() { return RGB("#FFEFD5"); }
        inline RGB moccasin() { return RGB("#FFE4B5"); }
        inline RGB peachpuff() { return RGB("#FFDAB9"); }
        inline RGB palegoldenrod() { return RGB("#EEE8AA"); }
        inline RGB khaki() { return RGB("#F0E68C"); }
        inline RGB darkkhaki() { return RGB("#BDB76B"); }

        // Purple and Violet Shades
        // Range from light lavender to deep purple and indigo
        inline RGB lavender() { return RGB("#E6E6FA"); }
        inline RGB thistle() { return RGB("#D8BFD8"); }
        inline RGB plum() { return RGB("#DDA0DD"); }
        inline RGB violet() { return RGB("#EE82EE"); }
        inline RGB orchid() { return RGB("#DA70D6"); }
        inline RGB fuchsia() { return RGB("#FF00FF"); }
        inline RGB magenta() { return RGB("#FF00FF"); }
        inline RGB mediumorchid() { return RGB("#BA55D3"); }
        inline RGB mediumpurple() { return RGB("#9370DB"); }
        inline RGB blueviolet() { return RGB("#8A2BE2"); }
        inline RGB darkviolet() { return RGB("#9400D3"); }
        inline RGB darkorchid() { return RGB("#9932CC"); }
        inline RGB darkmagenta() { return RGB("#8B008B"); }
        inline RGB purple() { return RGB("#800080"); }
        inline RGB rebeccapurple() { return RGB("#663399"); }
        inline RGB indigo() { return RGB("#4B0082"); }

        // Green Variations
        // Full spectrum of greens from yellow-green to forest green
        inline RGB greenyellow() { return RGB("#ADFF2F"); }
        inline RGB chartreuse() { return RGB("#7FFF00"); }
        inline RGB lawngreen() { return RGB("#7CFC00"); }
        inline RGB lime() { return RGB("#00FF00"); }
        inline RGB limegreen() { return RGB("#32CD32"); }
        inline RGB palegreen() { return RGB("#98FB98"); }
        inline RGB lightgreen() { return RGB("#90EE90"); }
        inline RGB mediumspringgreen() { return RGB("#00FA9A"); }
        inline RGB springgreen() { return RGB("#00FF7F"); }
        inline RGB mediumseagreen() { return RGB("#3CB371"); }
        inline RGB seagreen() { return RGB("#2E8B57"); }
        inline RGB forestgreen() { return RGB("#228B22"); }
        inline RGB green() { return RGB("#008000"); }
        inline RGB darkgreen() { return RGB("#006400"); }
        inline RGB yellowgreen() { return RGB("#9ACD32"); }
        inline RGB olivedrab() { return RGB("#6B8E23"); }
        inline RGB olive() { return RGB("#808000"); }
        inline RGB darkolivegreen() { return RGB("#556B2F"); }

        // Cyan and Turquoise Colors
        // Aquatic colors ranging from light cyan to deep teal
        inline RGB mediumaquamarine() { return RGB("#66CDAA"); }
        inline RGB aqua() { return RGB("#00FFFF"); }
        inline RGB cyan() { return RGB("#00FFFF"); }
        inline RGB lightcyan() { return RGB("#E0FFFF"); }
        inline RGB paleturquoise() { return RGB("#AFEEEE"); }
        inline RGB aquamarine() { return RGB("#7FFFD4"); }
        inline RGB turquoise() { return RGB("#40E0D0"); }
        inline RGB mediumturquoise() { return RGB("#48D1CC"); }
        inline RGB darkturquoise() { return RGB("#00CED1"); }
        inline RGB lightseagreen() { return RGB("#20B2AA"); }
        inline RGB cadetblue() { return RGB("#5F9EA0"); }
        inline RGB darkcyan() { return RGB("#008B8B"); }
        inline RGB teal() { return RGB("#008080"); }

        // Blue Variations
        // Complete range of blues from light to navy
        inline RGB lightsteelblue() { return RGB("#B0C4DE"); }
        inline RGB powderblue() { return RGB("#B0E0E6"); }
        inline RGB lightblue() { return RGB("#ADD8E6"); }
        inline RGB skyblue() { return RGB("#87CEEB"); }
        inline RGB lightskyblue() { return RGB("#87CEFA"); }
        inline RGB deepskyblue() { return RGB("#00BFFF"); }
        inline RGB dodgerblue() { return RGB("#1E90FF"); }
        inline RGB cornflowerblue() { return RGB("#6495ED"); }
        inline RGB steelblue() { return RGB("#4682B4"); }
        inline RGB royalblue() { return RGB("#4169E1"); }
        inline RGB blue() { return RGB("#0000FF"); }
        inline RGB mediumblue() { return RGB("#0000CD"); }
        inline RGB darkblue() { return RGB("#00008B"); }
        inline RGB navy() { return RGB("#000080"); }
        inline RGB midnightblue() { return RGB("#191970"); }

        // Brown and Earth Tones
        // Natural earth-toned colors from light tan to deep brown
        inline RGB cornsilk() { return RGB("#FFF8DC"); }
        inline RGB blanchedalmond() { return RGB("#FFEBCD"); }
        inline RGB bisque() { return RGB("#FFE4C4"); }
        inline RGB navajowhite() { return RGB("#FFDEAD"); }
        inline RGB wheat() { return RGB("#F5DEB3"); }
        inline RGB burlywood() { return RGB("#DEB887"); }
        inline RGB tan() { return RGB("#D2B48C"); }
        inline RGB rosybrown() { return RGB("#BC8F8F"); }
        inline RGB sandybrown() { return RGB("#F4A460"); }
        inline RGB goldenrod() { return RGB("#DAA520"); }
        inline RGB darkgoldenrod() { return RGB("#B8860B"); }
        inline RGB peru() { return RGB("#CD853F"); }
        inline RGB chocolate() { return RGB("#D2691E"); }
        inline RGB saddlebrown() { return RGB("#8B4513"); }
        inline RGB sienna() { return RGB("#A0522D"); }
        inline RGB brown() { return RGB("#A52A2A"); }
        inline RGB maroon() { return RGB("#800000"); }

        // White and Off-White Shades
        // Pure white and various warm and cool white tints
        inline RGB white() { return RGB("#FFFFFF"); }
        inline RGB snow() { return RGB("#FFFAFA"); }
        inline RGB honeydew() { return RGB("#F0FFF0"); }
        inline RGB mintcream() { return RGB("#F5FFFA"); }
        inline RGB azure() { return RGB("#F0FFFF"); }
        inline RGB aliceblue() { return RGB("#F0F8FF"); }
        inline RGB ghostwhite() { return RGB("#F8F8FF"); }
        inline RGB whitesmoke() { return RGB("#F5F5F5"); }
        inline RGB seashell() { return RGB("#FFF5EE"); }
        inline RGB beige() { return RGB("#F5F5DC"); }
        inline RGB oldlace() { return RGB("#FDF5E6"); }
        inline RGB floralwhite() { return RGB("#FFFAF0"); }
        inline RGB ivory() { return RGB("#FFFFF0"); }
        inline RGB antiquewhite() { return RGB("#FAEBD7"); }
        inline RGB linen() { return RGB("#FAF0E6"); }
        inline RGB lavenderblush() { return RGB("#FFF0F5"); }
        inline RGB mistyrose() { return RGB("#FFE4E1"); }

        // Gray Scale
        // Complete range of grays from light to dark
        inline RGB gainsboro() { return RGB("#DCDCDC"); }
        inline RGB lightgray() { return RGB("#D3D3D3"); }
        inline RGB silver() { return RGB("#C0C0C0"); }
        inline RGB darkgray() { return RGB("#A9A9A9"); }
        inline RGB gray() { return RGB("#808080"); }
        inline RGB dimgray() { return RGB("#696969"); }
        inline RGB lightslategray() { return RGB("#778899"); }
        inline RGB slategray() { return RGB("#708090"); }
        inline RGB darkslategray() { return RGB("#2F4F4F"); }
        inline RGB black() { return RGB("#000000"); }

    } // namespace colors
} // namespace pigment