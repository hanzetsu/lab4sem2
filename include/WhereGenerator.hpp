#pragma once
#include "Generator.hpp"
#include "Option.hpp"
#include <functional>
#include <memory>

template<typename T>
class WhereGenerator : public Generator<T> {
public:
    WhereGenerator(std::shared_ptr<Generator<T>> src, std::function<bool(const T&)> pred);
    T GetNext() override;
    bool HasNext() const override;
    Cardinal GetPotentialSize() const override;
    Generator<T>* Clone() const override;
private:
    std::shared_ptr<Generator<T>> m_src;
    std::function<bool(const T&)> m_pred;
    mutable Option<T> m_cache;
    mutable bool m_cached = false;
    void findNext() const;
};

#include "WhereGenerator.tpp"