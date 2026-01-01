#pragma once

#include "types_basic.hpp"
#include "types_lab.hpp"

#include <algorithm>
#include <cmath>

namespace pigment {

    /**
     * @brief LCH color type built on datapod::mat::vector<double, 3>
     *
     * Stores L (lightness 0-100), C (chroma 0-~140), H (hue 0-360 degrees).
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct LCH : public datapod::mat::vector<double, 3> {
        using base_type = datapod::mat::vector<double, 3>;

        // Accessors for color components
        double &l() { return data_[0]; }
        double &c() { return data_[1]; }
        double &h() { return data_[2]; }

        const double &l() const { return data_[0]; }
        const double &c() const { return data_[1]; }
        const double &h() const { return data_[2]; }

        LCH() {
            data_[0] = 0.0;
            data_[1] = 0.0;
            data_[2] = 0.0;
        }

        LCH(double l_, double c_, double h_) {
            data_[0] = l_;
            data_[1] = c_;
            data_[2] = h_;
            normalize();
        }

        // Normalize hue to [0, 360) range
        void normalize() {
            data_[0] = std::clamp(l(), 0.0, 100.0);
            data_[1] = std::max(0.0, c());

            // Wrap hue into [0, 360) range
            if (h() < 0.0 || h() >= 360.0) {
                data_[2] = std::fmod(h(), 360.0);
                if (h() < 0.0) {
                    data_[2] += 360.0;
                }
            }
        }

        // Create LCH from LAB
        static LCH fromLAB(const LAB &lab) {
            LCH result;
            result.data_[0] = lab.l();
            result.data_[1] = std::sqrt(lab.a() * lab.a() + lab.b() * lab.b());
            result.data_[2] = std::atan2(lab.b(), lab.a()) * 180.0 / M_PI;

            // Convert negative hue to positive
            if (result.h() < 0) {
                result.data_[2] += 360.0;
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
            double h_rad = h() * M_PI / 180.0;
            return LAB(l(), c() * std::cos(h_rad), c() * std::sin(h_rad));
        }

        // Convert LCH to RGB (via LAB conversion)
        RGB to_rgb() const { return to_lab().to_rgb(); }

        // Equality operators
        bool operator==(const LCH &other) const {
            const double epsilon = 1e-6;
            return std::abs(l() - other.l()) < epsilon && std::abs(c() - other.c()) < epsilon &&
                   std::abs(h() - other.h()) < epsilon;
        }

        bool operator!=(const LCH &other) const { return !(*this == other); }

        // Get lightness
        double lightness() const { return l(); }

        // Get chroma (colorfulness)
        double chroma() const { return c(); }

        // Get hue in degrees
        double hue() const { return h(); }

        // Get hue in radians
        double hue_radians() const { return h() * M_PI / 180.0; }

        // Adjust lightness
        LCH adjust_lightness(double delta) const { return LCH(std::clamp(l() + delta, 0.0, 100.0), c(), h()); }

        // Adjust chroma (saturation)
        LCH adjust_chroma(double delta) const { return LCH(l(), std::max(0.0, c() + delta), h()); }

        // Adjust chroma by factor
        LCH scale_chroma(double factor) const { return LCH(l(), std::max(0.0, c() * factor), h()); }

        // Rotate hue by angle in degrees
        LCH rotate_hue(double degrees) const { return LCH(l(), c(), h() + degrees); }

        // Set hue to specific angle in degrees
        LCH set_hue(double hue_degrees) const { return LCH(l(), c(), hue_degrees); }

        // Calculate perceptual distance to another LCH color (Delta E CIE 2000 approximation)
        double distance(const LCH &other) const {
            // Simplified Delta E calculation
            double dl = l() - other.l();
            double dc = c() - other.c();
            double dh = h() - other.h();

            // Handle hue wrap-around
            if (std::abs(dh) > 180.0) {
                dh = dh > 0 ? dh - 360.0 : dh + 360.0;
            }

            // Convert hue difference to chroma-weighted difference
            double avg_c = (c() + other.c()) / 2.0;
            double dh_weighted = 2.0 * std::sqrt(avg_c * other.c()) * std::sin(dh * M_PI / 360.0);

            return std::sqrt(dl * dl + dc * dc + dh_weighted * dh_weighted);
        }

        // Generate complementary color (180 deg hue rotation)
        LCH complement() const { return rotate_hue(180.0); }

        // Generate analogous colors (+/-30 deg hue rotation)
        std::pair<LCH, LCH> analogous() const { return {rotate_hue(-30.0), rotate_hue(30.0)}; }

        // Generate triadic colors (+/-120 deg hue rotation)
        std::pair<LCH, LCH> triadic() const { return {rotate_hue(120.0), rotate_hue(240.0)}; }

        // Generate split-complementary colors (150 deg and 210 deg rotation)
        std::pair<LCH, LCH> split_complementary() const { return {rotate_hue(150.0), rotate_hue(210.0)}; }

        // Generate tetradic/square colors (90 deg, 180 deg, 270 deg rotation)
        std::tuple<LCH, LCH, LCH> tetradic() const { return {rotate_hue(90.0), rotate_hue(180.0), rotate_hue(270.0)}; }
    };

    // Implementation of RGB conversion constructor for LCH
    inline RGB::RGB(const LCH &lch) {
        RGB temp = lch.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
