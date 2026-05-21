#include "MapGenerator.hpp"
template<typename T, typename U>
MapGenerator<T,U>::MapGenerator(std::unique_ptr<Generator<T>> src,
                                std::function<U(const T&)> f)
    : source(std::move(src)), func(std::move(f)) {}

template<typename T, typename U>
U MapGenerator<T,U>::GetNext() {
    return func(source->GetNext());
}

template<typename T, typename U>
bool MapGenerator<T,U>::HasNext() const {
    return source->HasNext();
}

template<typename T, typename U>
Cardinal MapGenerator<T,U>::GetPotentialSize() const {
    return source->GetPotentialSize();
}

template<typename T, typename U>
Generator<U>* MapGenerator<T,U>::Clone() const {
    return new MapGenerator<T,U>(std::unique_ptr<Generator<T>>(source->Clone()), func);
}