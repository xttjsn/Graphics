#include "BGRA.h"
#include "glm.hpp"
#include <cmath>

BGRA BGRA::operator+(const BGRA &that) const {
    return BGRA(glm::min(this->r + that.r, 255),
                glm::min(this->g + that.g, 255),
                glm::min(this->b + that.b, 255),
                glm::min(this->a + that.a, 255));
}

BGRA BGRA::operator-(const BGRA &that) const {
    return BGRA(this->r - that.r, this->g - that.g, this->b - that.b, this->a - that.a);
}

BGRA BGRA::operator*(const BGRA &that) const {
    return BGRA(this->r * that.r, this->g * that.g, this->b * that.b, this->a * that.a);
}

BGRA BGRA::operator/(const BGRA &that) const {
    return BGRA(this->r / that.r, this->g / that.g, this->b / that.b, this->a / that.a);
}

bool operator==(const BGRA &c1, const BGRA &c2) {
    return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a);
}

bool operator!=(const BGRA &c1, const BGRA &c2) {
    return !operator==(c1, c2);
}

BGRA BGRA::operator*(const float f) const {
    return BGRA(glm::round(static_cast<int>(this->r) * f),
                glm::round(static_cast<int>(this->g) * f),
                glm::round(static_cast<int>(this->b) * f),
                glm::round(static_cast<int>(this->a) * f));
}

BGRA BGRA::operator /(const float f) const {
    return BGRA(this->r / f, this->g / f, this->b / f, this->a / f);
}
