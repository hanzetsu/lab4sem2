#pragma once
#include "Generator.hpp"
#include "MutableArraySequence.hpp"

template<typename T, template<typename> class Container>
class SequenceGenerator : public Generator<T> {
public:
    explicit SequenceGenerator(const Container<T>& seq);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;

private:
    Container<T> data;
    size_t pos = 0;
};

#include "SequenceGenerator.tpp"