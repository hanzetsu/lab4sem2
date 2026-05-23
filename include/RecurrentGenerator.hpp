#pragma once
#include "Generator.hpp"
#include <functional>

template<typename T, template<typename> class Container>
class RecurrentGenerator : public Generator<T> {
public:
    RecurrentGenerator(std::function<T(const Container<T>&)> func, const Container<T>& start);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;
private:
    std::function<T(const Container<T>&)> m_func;
    Container<T> m_cache;
    size_t m_idx = 0;
};

#include "RecurrentGenerator.tpp"