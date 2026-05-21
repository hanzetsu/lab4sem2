#pragma once
#include "Generator.hpp"
#include <functional>
#include <memory>
template<typename T>
class WhereGenerator : public Generator<T> {
public:
    WhereGenerator(std::unique_ptr<Generator<T>> source, std::function<bool(const T&)> pred);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;

private:
    std::unique_ptr<Generator<T>> source;
    std::function<bool(const T&)> pred;
    mutable T nextCached;
    mutable bool hasCached = false;
    void findNext() const;
};

#include "WhereGenerator.tpp"