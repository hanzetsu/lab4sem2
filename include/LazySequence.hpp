#pragma once
#include "Cardinal.hpp"
#include "Generator.hpp"
#include "MutableArraySequence.hpp"
#include <memory>
#include <functional>
#include <initializer_list>

template<typename T, template<typename> class Container = MutableArraySequence>
class LazySequence {
public:
    LazySequence();
    explicit LazySequence(const Container<T>& cache);
    LazySequence(const Container<T>& cache, std::shared_ptr<Generator<T>> gen);
    LazySequence(T* items, size_t count);
    LazySequence(std::initializer_list<T> list);

    static LazySequence FromGenerator(std::shared_ptr<Generator<T>> gen);
    static LazySequence Recurrent(std::function<T(const Container<T>&)> func, const Container<T>& start);

    LazySequence(const LazySequence&) = default;
    LazySequence(LazySequence&&) = default;
    LazySequence& operator=(const LazySequence&) = default;
    LazySequence& operator=(LazySequence&&) = default;

    T GetFirst();
    T GetLast();
    T Get(Cardinal index);
    LazySequence GetSubsequence(size_t start, size_t end) const;

    LazySequence Append(T item) const;
    LazySequence Prepend(T item) const;
    LazySequence InsertAt(T item, size_t index) const;
    LazySequence Concat(const LazySequence& other) const;
    LazySequence SkipFirst(size_t count) const;

    template<typename U>
    LazySequence<U, Container> Map(std::function<U(const T&)> func) const;

    LazySequence Where(std::function<bool(const T&)> pred) const;

    template<typename U>
    U Reduce(std::function<U(const U&, const T&)> func, U init) const;

    template<typename U>
    LazySequence<std::pair<T, U>, Container> Zip(const LazySequence<U, Container>& other) const;

    Cardinal GetSizeSequence() const;
    size_t GetSizeCache() const;

private:
    std::shared_ptr<Generator<T>> m_gen;
    Container<T> m_cache;
    void EnsureMaterialized(size_t idx);
};

#include "LazySequence.tpp"