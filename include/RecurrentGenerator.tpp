template<typename T, template<typename> class Container>
RecurrentGenerator<T, Container>::RecurrentGenerator(std::function<T(const Container<T>&)> func,
                                                     const Container<T>& startCache)
    : func(func), cache(startCache), nextIndex(startCache.GetLength()) {}

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
    return true;
}

template<typename T, template<typename> class Container>
Cardinal RecurrentGenerator<T, Container>::GetPotentialSize() const {
    return Cardinal::Omega();
}

template<typename T, template<typename> class Container>
Generator<T>* RecurrentGenerator<T, Container>::Clone() const {
    auto* clone = new RecurrentGenerator<T, Container>(func, cache);
    clone->nextIndex = nextIndex;
    return clone;
}