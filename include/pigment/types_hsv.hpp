#pragma once

#include "types_basic.hpp"

#include <algorithm>
#include <cmath>

namespace pigment {

    /**
     * @brief HSV color type built on datapod::mat::Vector<float, 3>
     *
     * Stores Hue (0-360), Saturation (0-1), and Value (0-1) as floats.
     * Uses datapod's vector for efficient storage and serialization support.
     */
    struct HSV : public dp::mat::Vector<float, 3> {
        using base_type = dp::mat::Vector<float, 3>;

        // Accessors for color components
        float &h() { return data_[0]; }
        float &s() { return data_[1]; }
        float &v() { return data_[2]; }

        const float &h() const { return data_[0]; }
        const float &s() const { return data_[1]; }
        const float &v() const { return data_[2]; }

        HSV() {
            data_[0] = 0.0f;
            data_[1] = 0.0f;
            data_[2] = 0.0f;
        }

        HSV(float h_, float s_, float v_) {
            data_[0] = h_;
            data_[1] = s_;
            data_[2] = v_;
            normalize();
        }

        // Hex string constructor
        HSV(const std::string &hex) {
            RGB rgb(hex); // Use RGB hex parsing
            *this = fromRGB(rgb);
        }

        // Clamp fields into valid ranges
        void normalize() {
            // wrap hue into [0,360)
            if (h() < 0.0f || h() >= 360.0f) {
                data_[0] = std::fmod(h(), 360.0f);
                if (h() < 0.0f)
                    data_[0] += 360.0f;
            }
            data_[1] = std::clamp(s(), 0.0f, 1.0f);
            data_[2] = std::clamp(v(), 0.0f, 1.0f);
        }

        // Create HSV from an RGB (alpha ignored)
        static HSV fromRGB(const RGB &c) {
            float rf = c.r() / 255.0f;
            float gf = c.g() / 255.0f;
            float bf = c.b() / 255.0f;

            float mx = std::max({rf, gf, bf});
            float mn = std::min({rf, gf, bf});
            float delta = mx - mn;

            HSV out;

            // Hue calculation
            if (delta < 1e-6f) {
                out.data_[0] = 0.0f;
            } else if (mx == rf) {
                out.data_[0] = 60.0f * std::fmod((gf - bf) / delta, 6.0f);
            } else if (mx == gf) {
                out.data_[0] = 60.0f * (((bf - rf) / delta) + 2.0f);
            } else {
                out.data_[0] = 60.0f * (((rf - gf) / delta) + 4.0f);
            }
            if (out.h() < 0)
                out.data_[0] += 360.0f;

            // Saturation & Value
            out.data_[1] = (mx < 1e-6f ? 0.0f : (delta / mx));
            out.data_[2] = mx;

            out.normalize();
            return out;
        }

        // Convert this HSV to RGB (alpha = 255)
        RGB to_rgb() const {
            float C = v() * s();
            float X = C * (1 - std::fabs(std::fmod(h() / 60.0f, 2.0f) - 1));
            float m = v() - C;

            float rp, gp, bp;
            if (h() < 60.0f) {
                rp = C;
                gp = X;
                bp = 0;
            } else if (h() < 120.0f) {
                rp = X;
                gp = C;
                bp = 0;
            } else if (h() < 180.0f) {
                rp = 0;
                gp = C;
                bp = X;
            } else if (h() < 240.0f) {
                rp = 0;
                gp = X;
                bp = C;
            } else if (h() < 300.0f) {
                rp = X;
                gp = 0;
                bp = C;
            } else {
                rp = C;
                gp = 0;
                bp = X;
            }

            return RGB(static_cast<uint8_t>(std::round((rp + m) * 255)),
                       static_cast<uint8_t>(std::round((gp + m) * 255)),
                       static_cast<uint8_t>(std::round((bp + m) * 255)), 255);
        }

        // delta in [-1,1]:
        //   0 = no change
        //  -1 = full dark (v->0)
        //  +1 = full bright (v->1)
        inline void adjust_brightness(float delta) {
            delta = std::clamp(delta, -1.0f, 1.0f);
            if (delta > 0.0f) {
                // move v toward 1.0
                data_[2] = std::clamp(v() + delta * (1.0f - v()), 0.0f, 1.0f);
            } else {
                // move v toward 0.0
                data_[2] = std::clamp(v() + delta * v(), 0.0f, 1.0f);
            }
        }

        // delta in [-1,1]:
        //   0 = no change
        //  -1 = full desaturate (s->0)
        //  +1 = full saturate   (s->1)
        inline void adjust_saturation(float delta) {
            delta = std::clamp(delta, -1.0f, 1.0f);
            if (delta > 0.0f) {
                // move s toward 1.0
                data_[1] = std::clamp(s() + delta * (1.0f - s()), 0.0f, 1.0f);
            } else {
                // move s toward 0.0
                data_[1] = std::clamp(s() + delta * s(), 0.0f, 1.0f);
            }
        }

        // Convert HSV to hex string
        std::string to_hex(bool include_alpha = false) const {
            RGB rgb = to_rgb();
            return rgb.to_hex(include_alpha);
        }
    };

    // Implementation of RGB conversion constructor
    inline RGB::RGB(const HSV &hsv) {
        RGB temp = hsv.to_rgb();
        data_[0] = temp.r();
        data_[1] = temp.g();
        data_[2] = temp.b();
        data_[3] = temp.a();
    }

} // namespace pigment
