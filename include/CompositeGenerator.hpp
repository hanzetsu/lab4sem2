#pragma once
#include "Generator.hpp"
#include <memory>

template<typename T>
class CompositeGenerator : public Generator<T> {
public:
    CompositeGenerator(std::shared_ptr<Generator<T>> first, std::shared_ptr<Generator<T>> second);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;
private:
    std::shared_ptr<Generator<T>> m_first;
    std::shared_ptr<Generator<T>> m_second;
    bool m_firstDone = false;
};

#include "CompositeGenerator.tpp"