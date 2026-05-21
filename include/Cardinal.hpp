#pragma once
#include <cstddef>
#include "exceptions.hpp"

class Cardinal {
private:
    size_t finite = 0;
    bool alpha_null = false;
public:
    Cardinal() : finite(0), alpha_null(false) {}
    Cardinal(size_t size) : finite(size), alpha_null(false) {}
    static Cardinal Omega() {
        Cardinal c;
        c.alpha_null = true;
        c.finite = 0;
        return c;
    }
    bool IsFinalNumber() const { return !alpha_null; }
    bool IsInfiniteNumber() const { return alpha_null; }
    size_t GetSize() const {
        if (alpha_null) throw IsInfiniteLengthException("Длина равна бесконечности");
        return finite;
    }
    bool operator==(const Cardinal& other) const {
        if (alpha_null && other.alpha_null) return true;
        if (alpha_null || other.alpha_null) return false;
        return finite == other.finite;
    }
    bool operator!=(const Cardinal& other) const { return !(*this == other); }
    bool operator<(const Cardinal& other) const {
        if (alpha_null) return false;
        if (other.alpha_null) return true;
        return finite < other.finite;
    }
    bool operator<=(const Cardinal& other) const { return (*this < other) || (*this == other); }
    bool operator>(const Cardinal& other) const { return !(*this <= other); }
    bool operator>=(const Cardinal& other) const { return !(*this < other); }
};