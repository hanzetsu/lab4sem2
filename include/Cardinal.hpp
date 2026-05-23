#pragma once
#include <cstddef>
#include "exceptions.hpp"

class Cardinal {
private:
    size_t finite = 0;
    bool infinite = false;

    explicit Cardinal(size_t val, bool inf) noexcept : finite(val), infinite(inf) {}

public:
    Cardinal() noexcept : Cardinal(0, false) {}
    explicit Cardinal(size_t val) noexcept : Cardinal(val, false) {}

    static Cardinal Omega() noexcept { return Cardinal(0, true); }
    static Cardinal Finite(size_t val) noexcept { return Cardinal(val, false); }

    bool IsFinite() const noexcept { return !infinite; }
    bool IsInfinite() const noexcept { return infinite; }
    bool IsInfiniteNumber() const noexcept { return infinite; }
    bool IsFinalNumber() const noexcept { return !infinite; }

    size_t GetValue() const {
        if (infinite) throw IsInfiniteLengthException("Cardinal is infinite");
        return finite;
    }
    size_t GetSize() const { return GetValue(); }

    bool operator==(const Cardinal& other) const noexcept {
        if (infinite && other.infinite) return true;
        if (infinite || other.infinite) return false;
        return finite == other.finite;
    }
    bool operator!=(const Cardinal& other) const noexcept { return !(*this == other); }
    bool operator<(const Cardinal& other) const noexcept {
        if (infinite) return false;
        if (other.infinite) return true;
        return finite < other.finite;
    }
    bool operator<=(const Cardinal& other) const noexcept { return (*this < other) || (*this == other); }
    bool operator>(const Cardinal& other) const noexcept { return !(*this <= other); }
    bool operator>=(const Cardinal& other) const noexcept { return !(*this < other); }
};