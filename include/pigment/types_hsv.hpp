#pragma once

#include "types_basic.hpp"
#include <algorithm>
#include <cmath>

namespace pigment {

    struct HSV {
        // Hue in [0,360), saturation/value in [0,1]
        float h = 0.0f;
        float s = 0.0f;
        float v = 0.0f;

        HSV() = default;
        HSV(float h_, float s_, float v_) : h(h_), s(s_), v(v_) { normalize(); }

        // Clamp fields into valid ranges
        void normalize() {
            // wrap hue into [0,360)
            if (h < 0.0f || h >= 360.0f) {
                h = std::fmod(h, 360.0f);
                if (h < 0.0f)
                    h += 360.0f;
            }
            s = std::clamp(s, 0.0f, 1.0f);
            v = std::clamp(v, 0.0f, 1.0f);
        }

        // Create HSV from an RGB (alpha ignored)
        static HSV fromRGB(const RGB &c) {
            float rf = c.r / 255.0f;
            float gf = c.g / 255.0f;
            float bf = c.b / 255.0f;

            float mx = std::max({rf, gf, bf});
            float mn = std::min({rf, gf, bf});
            float delta = mx - mn;

            HSV out;

            // Hue calculation
            if (delta < 1e-6f) {
                out.h = 0.0f;
            } else if (mx == rf) {
                out.h = 60.0f * std::fmod((gf - bf) / delta, 6.0f);
            } else if (mx == gf) {
                out.h = 60.0f * (((bf - rf) / delta) + 2.0f);
            } else {
                out.h = 60.0f * (((rf - gf) / delta) + 4.0f);
            }
            if (out.h < 0)
                out.h += 360.0f;

            // Saturation & Value
            out.s = (mx < 1e-6f ? 0.0f : (delta / mx));
            out.v = mx;

            out.normalize();
            return out;
        }

        // Convert this HSV to RGB (alpha = 255)
        RGB to_rgb() const {
            float C = v * s;
            float X = C * (1 - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1));
            float m = v - C;

            float rp, gp, bp;
            if (h < 60.0f) {
                rp = C;
                gp = X;
                bp = 0;
            } else if (h < 120.0f) {
                rp = X;
                gp = C;
                bp = 0;
            } else if (h < 180.0f) {
                rp = 0;
                gp = C;
                bp = X;
            } else if (h < 240.0f) {
                rp = 0;
                gp = X;
                bp = C;
            } else if (h < 300.0f) {
                rp = X;
                gp = 0;
                bp = C;
            } else {
                rp = C;
                gp = 0;
                bp = X;
            }

            RGB out;
            out.r = int(std::round((rp + m) * 255));
            out.g = int(std::round((gp + m) * 255));
            out.b = int(std::round((bp + m) * 255));
            out.a = 255;
            return out;
        }
        // delta in [-1,1]:
        //   0 = no change
        //  -1 = full dark (v→0)
        //  +1 = full bright (v→1)
        inline void adjustBrightness(float delta) {
            delta = std::clamp(delta, -1.0f, 1.0f);
            if (delta > 0.0f) {
                // move v toward 1.0
                v = std::clamp(v + delta * (1.0f - v), 0.0f, 1.0f);
            } else {
                // move v toward 0.0
                v = std::clamp(v + delta * v, 0.0f, 1.0f);
            }
        }

        // delta in [-1,1]:
        //   0 = no change
        //  -1 = full desaturate (s→0)
        //  +1 = full saturate   (s→1)
        inline void adjustSaturation(float delta) {
            delta = std::clamp(delta, -1.0f, 1.0f);
            if (delta > 0.0f) {
                // move s toward 1.0
                s = std::clamp(s + delta * (1.0f - s), 0.0f, 1.0f);
            } else {
                // move s toward 0.0
                s = std::clamp(s + delta * s, 0.0f, 1.0f);
            }
        }
    };

    // Implementation of RGB conversion constructor  
    inline RGB::RGB(const HSV& hsv) {
        RGB temp = hsv.to_rgb();
        r = temp.r;
        g = temp.g;
        b = temp.b; 
        a = temp.a;
    }

} // namespace pigment
