template<typename T, template<typename> class Container>
RecurrentGenerator<T, Container>::RecurrentGenerator(std::function<T(const Container<T>&)> func, const Container<T>& start)
    : m_func(func), m_cache(start), m_idx(start.GetLength()) {}

template<typename T, template<typename> class Container>
T RecurrentGenerator<T, Container>::GetNext() {
    if (m_idx < m_cache.GetLength())
        return m_cache.Get(m_idx++);
    T val = m_func(m_cache);
    m_cache.Append(val);
    m_idx++;
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
    auto* clone = new RecurrentGenerator<T, Container>(m_func, m_cache);
    clone->m_idx = m_idx;
    return clone;
}