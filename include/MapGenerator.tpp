template<typename T, typename U>
MapGenerator<T, U>::MapGenerator(std::shared_ptr<Generator<T>> src, std::function<U(const T&)> f)
    : m_src(src), m_func(f) {}

template<typename T, typename U>
U MapGenerator<T, U>::GetNext() {
    return m_func(m_src->GetNext());
}

template<typename T, typename U>
bool MapGenerator<T, U>::HasNext() const {
    return m_src && m_src->HasNext();
}

template<typename T, typename U>
Cardinal MapGenerator<T, U>::GetPotentialSize() const {
    return m_src ? m_src->GetPotentialSize() : Cardinal::Finite(0);
}

template<typename T, typename U>
Generator<U>* MapGenerator<T, U>::Clone() const {
    auto copy = m_src ? std::shared_ptr<Generator<T>>(m_src->Clone()) : nullptr;
    return new MapGenerator<T, U>(copy, m_func);
}