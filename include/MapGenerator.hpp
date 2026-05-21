#pragma once
#include "Generator.hpp"
#include <functional>
#include <memory>

template<typename T, typename U>
class MapGenerator : public Generator<U> {
public:
    MapGenerator(std::shared_ptr<Generator<T>> src, std::function<U(const T&)> func);
    U GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<U>* Clone() const override;

private:
    std::shared_ptr<Generator<T>> source;
    std::function<U(const T&)> func;
};

#include "MapGenerator.tpp"