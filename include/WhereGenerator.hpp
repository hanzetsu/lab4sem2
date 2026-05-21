#pragma once
#include "Generator.hpp"
#include <functional>
#include <memory>
#include <optional>

template<typename T>
class WhereGenerator : public Generator<T> {
public:
    WhereGenerator(std::shared_ptr<Generator<T>> src, std::function<bool(const T&)> pred);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;

private:
    std::shared_ptr<Generator<T>> source;
    std::function<bool(const T&)> pred;
    mutable std::optional<T> cachedNext;
    mutable bool hasCached = false;
    void findNext() const;
};

#include "WhereGenerator.tpp"