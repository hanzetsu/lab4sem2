#include "MapGenerator.hpp"
#include "exceptions.hpp"

template<typename T, typename U>
MapGenerator<T, U>::MapGenerator(std::shared_ptr<Generator<T>> src, std::function<U(const T&)> f)
    : source(src), func(f) {}

template<typename T, typename U>
U MapGenerator<T, U>::GetNext() {
    return func(source->GetNext());
}

template<typename T, typename U>
bool MapGenerator<T, U>::HasNext() const {
    return source && source->HasNext();
}

template<typename T, typename U>
Cardinal MapGenerator<T, U>::GetPotentialSize() const {
    if (source) return source->GetPotentialSize();
    return Cardinal(0);
}

template<typename T, typename U>
Generator<U>* MapGenerator<T, U>::Clone() const {
    auto clonedSource = source ? std::shared_ptr<Generator<T>>(source->Clone()) : nullptr;
    return new MapGenerator<T, U>(clonedSource, func);
}