#pragma once
#include "Generator.hpp"
#include <memory>
template<typename T>
class CompositeGenerator : public Generator<T> {
public:
    CompositeGenerator(std::unique_ptr<Generator<T>> first, std::unique_ptr<Generator<T>> second);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;

private:
    std::unique_ptr<Generator<T>> first;
    std::unique_ptr<Generator<T>> second;
    bool firstFinished = false;
};

#include "CompositeGenerator.tpp"