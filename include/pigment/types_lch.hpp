#pragma once

#include "types_basic.hpp"
#include "types_lab.hpp"
#include <algorithm>
#include <cmath>

namespace pigment {

    struct LCH {
        // L (lightness): 0-100, C (chroma): 0-approximately 140, H (hue): 0-360 degrees
        double L = 0.0;
        double C = 0.0;
        double H = 0.0;

        LCH() = default;
        LCH(double L_, double C_, double H_) : L(L_), C(C_), H(H_) { normalize(); }

        // Normalize hue to [0, 360) range
        void normalize() {
            L = std::clamp(L, 0.0, 100.0);
            C = std::max(0.0, C);
            
            // Wrap hue into [0, 360) range
            if (H < 0.0 || H >= 360.0) {
                H = std::fmod(H, 360.0);
                if (H < 0.0) {
                    H += 360.0;
                }
            }
        }

        // Create LCH from LAB
        static LCH fromLAB(const LAB &lab) {
            LCH result;
            result.L = lab.l;
            result.C = std::sqrt(lab.a * lab.a + lab.b * lab.b);
            result.H = std::atan2(lab.b, lab.a) * 180.0 / M_PI;
            
            // Convert negative hue to positive
            if (result.H < 0) {
                result.H += 360.0;
            }
            
            result.normalize();
            return result;
        }

        // Create LCH from RGB (via LAB conversion)
        static LCH fromRGB(const RGB &rgb) {
            LAB lab = LAB::fromRGB(rgb);
            return fromLAB(lab);
        }

        // Convert LCH to LAB
        LAB to_lab() const {
            double h_rad = H * M_PI / 180.0;
            return LAB(L, C * std::cos(h_rad), C * std::sin(h_rad));
        }

        // Convert LCH to RGB (via LAB conversion)
        RGB to_rgb() const {
            return to_lab().to_rgb();
        }

        // Equality operators
        bool operator==(const LCH &other) const {
            const double epsilon = 1e-6;
            return std::abs(L - other.L) < epsilon && 
                   std::abs(C - other.C) < epsilon && 
                   std::abs(H - other.H) < epsilon;
        }

        bool operator!=(const LCH &other) const {
            return !(*this == other);
        }

        // Get lightness
        double lightness() const {
            return L;
        }

        // Get chroma (colorfulness)
        double chroma() const {
            return C;
        }

        // Get hue in degrees
        double hue() const {
            return H;
        }

        // Get hue in radians
        double hue_radians() const {
            return H * M_PI / 180.0;
        }

        // Adjust lightness
        LCH adjust_lightness(double delta) const {
            return LCH(std::clamp(L + delta, 0.0, 100.0), C, H);
        }

        // Adjust chroma (saturation)
        LCH adjust_chroma(double delta) const {
            return LCH(L, std::max(0.0, C + delta), H);
        }

        // Adjust chroma by factor
        LCH scale_chroma(double factor) const {
            return LCH(L, std::max(0.0, C * factor), H);
        }

        // Rotate hue by angle in degrees
        LCH rotate_hue(double degrees) const {
            return LCH(L, C, H + degrees);
        }

        // Set hue to specific angle in degrees
        LCH set_hue(double hue_degrees) const {
            return LCH(L, C, hue_degrees);
        }

        // Calculate perceptual distance to another LCH color (Delta E CIE 2000 approximation)
        double distance(const LCH &other) const {
            // Simplified Delta E calculation
            double dL = L - other.L;
            double dC = C - other.C;
            double dH = H - other.H;
            
            // Handle hue wrap-around
            if (std::abs(dH) > 180.0) {
                dH = dH > 0 ? dH - 360.0 : dH + 360.0;
            }
            
            // Convert hue difference to chroma-weighted difference
            double avg_C = (C + other.C) / 2.0;
            double dH_weighted = 2.0 * std::sqrt(avg_C * other.C) * std::sin(dH * M_PI / 360.0);
            
            return std::sqrt(dL * dL + dC * dC + dH_weighted * dH_weighted);
        }

        // Generate complementary color (180° hue rotation)
        LCH complement() const {
            return rotate_hue(180.0);
        }

        // Generate analogous colors (±30° hue rotation)
        std::pair<LCH, LCH> analogous() const {
            return {rotate_hue(-30.0), rotate_hue(30.0)};
        }

        // Generate triadic colors (±120° hue rotation)
        std::pair<LCH, LCH> triadic() const {
            return {rotate_hue(120.0), rotate_hue(240.0)};
        }

        // Generate split-complementary colors (150° and 210° rotation)
        std::pair<LCH, LCH> split_complementary() const {
            return {rotate_hue(150.0), rotate_hue(210.0)};
        }

        // Generate tetradic/square colors (90°, 180°, 270° rotation)
        std::tuple<LCH, LCH, LCH> tetradic() const {
            return {rotate_hue(90.0), rotate_hue(180.0), rotate_hue(270.0)};
        }
    };

    // Implementation of RGB conversion constructor for LCH
    inline RGB::RGB(const LCH& lch) {
        RGB temp = lch.to_rgb();
        r = temp.r;
        g = temp.g;
        b = temp.b;
        a = temp.a;
    }

} // namespace pigment