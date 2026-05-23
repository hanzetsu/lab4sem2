template<typename T>
CompositeGenerator<T>::CompositeGenerator(std::shared_ptr<Generator<T>> f, std::shared_ptr<Generator<T>> s)
    : m_first(f), m_second(s) {}

template<typename T>
T CompositeGenerator<T>::GetNext() {
    if (!m_firstDone && m_first && m_first->HasNext())
        return m_first->GetNext();
    if (!m_firstDone) m_firstDone = true;
    return m_second->GetNext();
}

template<typename T>
bool CompositeGenerator<T>::HasNext() const {
    return (!m_firstDone && m_first && m_first->HasNext()) || (m_second && m_second->HasNext());
}

template<typename T>
Cardinal CompositeGenerator<T>::GetPotentialSize() const {
    auto sz1 = m_first ? m_first->GetPotentialSize() : Cardinal::Finite(0);
    auto sz2 = m_second ? m_second->GetPotentialSize() : Cardinal::Finite(0);
    if (sz1.IsInfinite() || sz2.IsInfinite()) return Cardinal::Omega();
    return Cardinal::Finite(sz1.GetValue() + sz2.GetValue());
}

template<typename T>
Generator<T>* CompositeGenerator<T>::Clone() const {
    auto f = m_first ? std::shared_ptr<Generator<T>>(m_first->Clone()) : nullptr;
    auto s = m_second ? std::shared_ptr<Generator<T>>(m_second->Clone()) : nullptr;
    return new CompositeGenerator<T>(f, s);
}