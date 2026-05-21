#include "RecurrentGenerator.hpp"
template<typename T, template<typename> class Container>
RecurrentGenerator<T, Container>::RecurrentGenerator(std::function<T(const Container<T>&)> f,
                                                     const Container<T>& startCache)
    : func(std::move(f)), cache(startCache) {
    nextIndex = cache.GetLength();
}

template<typename T, template<typename> class Container>
T RecurrentGenerator<T, Container>::GetNext() {
    if (nextIndex < cache.GetLength()) {
        return cache.Get(nextIndex++);
    }
    T val = func(cache);
    cache.Append(val);
    nextIndex++;
    return val;
}

template<typename T, template<typename> class Container>
bool RecurrentGenerator<T, Container>::HasNext() const {
    return infinite;
}

template<typename T, template<typename> class Container>
Cardinal RecurrentGenerator<T, Container>::GetPotentialSize() const {
    return Cardinal::Omega();
}

template<typename T, template<typename> class Container>
Generator<T>* RecurrentGenerator<T, Container>::Clone() const {
    auto* clone = new RecurrentGenerator<T, Container>(func, cache);
    clone->nextIndex = nextIndex;
    clone->infinite = infinite;
    return clone;
}