#include "CompositeGenerator.hpp"
template<typename T>
CompositeGenerator<T>::CompositeGenerator(std::unique_ptr<Generator<T>> f,
                                          std::unique_ptr<Generator<T>> s)
    : first(std::move(f)), second(std::move(s)) {}

template<typename T>
T CompositeGenerator<T>::GetNext() {
    if (!firstFinished && first->HasNext()) {
        return first->GetNext();
    } else if (!firstFinished) {
        firstFinished = true;
    }
    if (second->HasNext()) {
        return second->GetNext();
    }
    throw OutOfRangeException("CompositeGenerator: нет элементов");
}

template<typename T>
bool CompositeGenerator<T>::HasNext() const {
    if (!firstFinished && first->HasNext()) return true;
    return second->HasNext();
}

template<typename T>
Cardinal CompositeGenerator<T>::GetPotentialSize() const {
    Cardinal sz1 = first->GetPotentialSize();
    Cardinal sz2 = second->GetPotentialSize();
    if (sz1.IsInfiniteNumber() || sz2.IsInfiniteNumber()) {
        return Cardinal::Omega();
    }
    return Cardinal(sz1.GetSize() + sz2.GetSize());
}

template<typename T>
Generator<T>* CompositeGenerator<T>::Clone() const {
    auto* fclone = first ? first->Clone() : nullptr;
    auto* sclone = second ? second->Clone() : nullptr;
    return new CompositeGenerator<T>(std::unique_ptr<Generator<T>>(fclone),
                                      std::unique_ptr<Generator<T>>(sclone));
}