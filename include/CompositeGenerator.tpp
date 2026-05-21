template<typename T>
CompositeGenerator<T>::CompositeGenerator(std::shared_ptr<Generator<T>> f, std::shared_ptr<Generator<T>> s)
    : first(f), second(s) {}

template<typename T>
T CompositeGenerator<T>::GetNext() {
    if (!firstFinished && first && first->HasNext()) {
        return first->GetNext();
    } else if (!firstFinished) {
        firstFinished = true;
    }
    return second->GetNext();
}

template<typename T>
bool CompositeGenerator<T>::HasNext() const {
    return (!firstFinished && first && first->HasNext()) || (second && second->HasNext());
}

template<typename T>
Cardinal CompositeGenerator<T>::GetPotentialSize() const {
    auto s1 = first ? first->GetPotentialSize() : Cardinal(0);
    auto s2 = second ? second->GetPotentialSize() : Cardinal(0);
    if (s1.IsInfiniteNumber() || s2.IsInfiniteNumber()) return Cardinal::Omega();
    return Cardinal(s1.GetSize() + s2.GetSize());
}

template<typename T>
Generator<T>* CompositeGenerator<T>::Clone() const {
    auto f = first ? std::shared_ptr<Generator<T>>(first->Clone()) : nullptr;
    auto s = second ? std::shared_ptr<Generator<T>>(second->Clone()) : nullptr;
    return new CompositeGenerator<T>(f, s);
}