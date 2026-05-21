#include <WhereGenerator.hpp>
template<typename T>
WhereGenerator<T>::WhereGenerator(std::unique_ptr<Generator<T>> src,
                                  std::function<bool(const T&)> p)
    : source(std::move(src)), pred(std::move(p)) {}

template<typename T>
void WhereGenerator<T>::findNext() const {
    while (source->HasNext()) {
        T val = source->GetNext();
        if (pred(val)) {
            nextCached = val;
            hasCached = true;
            return;
        }
    }
    hasCached = false;
}

template<typename T>
T WhereGenerator<T>::GetNext() {
    if (!hasCached) findNext();
    if (!hasCached) throw OutOfRangeException("WhereGenerator: нет элементов");
    T res = nextCached;
    hasCached = false;
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
    return new WhereGenerator<T>(std::unique_ptr<Generator<T>>(source->Clone()), pred);
}