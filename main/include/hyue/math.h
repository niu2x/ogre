#pragma once

#include <climits>

#include <hyue/export.h>

namespace hyue::math {

HYUE_API inline float saturate(float t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }
HYUE_API inline float saturate(double t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }

HYUE_API inline bool equal(float a, float b,
    float tolerance = std::numeric_limits<float>::epsilon()) {
    return std::abs(b-a) <= tolerance;
}
HYUE_API inline bool equal(double a, double b,
    double tolerance = std::numeric_limits<double>::epsilon()) {
    
    return std::abs(b-a) <= tolerance;
}


}