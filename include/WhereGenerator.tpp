#include "WhereGenerator.hpp"
#include "exceptions.hpp"

template<typename T>
WhereGenerator<T>::WhereGenerator(std::shared_ptr<Generator<T>> src, std::function<bool(const T&)> p)
    : source(src), pred(p) {}

template<typename T>
void WhereGenerator<T>::findNext() const {
    while (source && source->HasNext()) {
        T val = source->GetNext();
        if (pred(val)) {
            cachedNext = val;
            hasCached = true;
            return;
        }
    }
    hasCached = false;
    cachedNext.reset();
}

template<typename T>
T WhereGenerator<T>::GetNext() {
    if (!hasCached) findNext();
    if (!hasCached) throw OutOfRangeException("WhereGenerator: нет элементов");
    T res = *cachedNext;
    hasCached = false;
    cachedNext.reset();
    return res;
}

template<typename T>
bool WhereGenerator<T>::HasNext() const {
    if (hasCached) return true;
    findNext();
    return hasCached;
}

template<typename T>
Cardinal WhereGenerator<T>::GetPotentialSize() const {
    return Cardinal::Omega();
}

template<typename T>
Generator<T>* WhereGenerator<T>::Clone() const {
    auto clonedSource = source ? std::shared_ptr<Generator<T>>(source->Clone()) : nullptr;
    return new WhereGenerator<T>(clonedSource, pred);
}