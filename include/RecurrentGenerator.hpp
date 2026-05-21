#pragma once
#include "Generator.hpp"
#include <functional>

template<typename T, template<typename> class Container>
class RecurrentGenerator : public Generator<T> {
public:
    RecurrentGenerator(std::function<T(const Container<T>&)> func, const Container<T>& startCache);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;

private:
    std::function<T(const Container<T>&)> func;
    Container<T> cache;
    size_t nextIndex = 0;
    bool infinite = true;
};

#include "RecurrentGenerator.tpp"