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
    explicit LazySequence(T* items, size_t count);
    LazySequence(const Container<T>& cache, std::shared_ptr<Generator<T>> generator);
    LazySequence(const LazySequence<T, Container>& other) = default;
    LazySequence(LazySequence<T, Container>&& other) = default;
    LazySequence(std::initializer_list<T> list);

    LazySequence& operator=(const LazySequence<T, Container>& other) = default;
    LazySequence& operator=(LazySequence<T, Container>&& other) = default;

    static LazySequence<T, Container> FromGenerator(std::shared_ptr<Generator<T>> gen);
    static LazySequence<T, Container> Recurrent(std::function<T(const Container<T>&)> func,
                                                const Container<T>& startCache);

    Cardinal GetSizeSequence() const;
    size_t GetSizeCache() const;
    T GetFirst();
    T GetLast();
    T Get(Cardinal index);
    LazySequence<T, Container> GetSubsequence(size_t startIndex, size_t endIndex);

    LazySequence<T, Container> Append(T item) const;
    LazySequence<T, Container> Prepend(T item) const;
    LazySequence<T, Container> InsertAt(T item, size_t index) const;
    LazySequence<T, Container> Concat(const LazySequence<T, Container>& other) const;
    LazySequence<T, Container> SkipFirst(size_t count) const;
    LazySequence<T, Container> Skip(size_t start, size_t end) const;

    template<typename U>
    LazySequence<U, Container> Map(std::function<U(const T&)> func) const;
    LazySequence<T, Container> Where(std::function<bool(const T&)> pred) const;
    template<typename U>
    U Reduce(std::function<U(const U&, const T&)> func, U init) const;
    template<typename U>
    LazySequence<std::pair<T, U>, Container> Zip(const LazySequence<U, Container>& other) const;

private:
    std::shared_ptr<Generator<T>> m_generator;
    Container<T> m_cache;
    void EnsureMaterialized(size_t index);
};

#include "LazySequence.tpp"