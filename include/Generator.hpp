#pragma once
#include "Cardinal.hpp"
#include "Option.hpp"

template<typename T>
class Generator {
public:
    virtual ~Generator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual Cardinal GetPotentialSize() const = 0;
    virtual Option<T> TryGetNext() {
        if (HasNext()) return Option<T>(GetNext());
        return Option<T>();
    }
    virtual Generator<T>* Clone() const = 0;
};