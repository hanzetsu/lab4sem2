#pragma once
#include "Generator.hpp"
#include <memory>

template<typename T, template<typename> class Container>
class SequenceGenerator : public Generator<T> {
public:
    explicit SequenceGenerator(std::shared_ptr<Container<T>> seq);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;
private:
    std::shared_ptr<Container<T>> m_data;
    size_t m_pos = 0;
};

#include "SequenceGenerator.tpp"