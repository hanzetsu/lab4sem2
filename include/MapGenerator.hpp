#pragma once
#include "Generator.hpp"
#include <functional>
#include <memory>
template<typename T, typename U>
class MapGenerator : public Generator<U> {
public:
    MapGenerator(std::unique_ptr<Generator<T>> source, std::function<U(const T&)> func);
    U GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<U>* Clone() const override;

private:
    std::unique_ptr<Generator<T>> source;
    std::function<U(const T&)> func;
};

#include "MapGenerator.tpp"