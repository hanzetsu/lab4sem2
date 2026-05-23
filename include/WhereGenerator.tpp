template<typename T>
WhereGenerator<T>::WhereGenerator(std::shared_ptr<Generator<T>> src, std::function<bool(const T&)> pred)
    : m_src(src), m_pred(pred) {}

template<typename T>
void WhereGenerator<T>::findNext() const {
    while (m_src && m_src->HasNext()) {
        T val = m_src->GetNext();
        if (m_pred(val)) {
            m_cache = Option<T>(val);
            m_cached = true;
            return;
        }
    }
    m_cached = false;
    m_cache = Option<T>();
}

template<typename T>
T WhereGenerator<T>::GetNext() {
    if (!m_cached) findNext();
    if (!m_cached) throw OutOfRangeException("WhereGenerator: end");
    T res = m_cache.GetValue();
    m_cached = false;
    m_cache = Option<T>();
    return res;
}

template<typename T>
bool WhereGenerator<T>::HasNext() const {
    if (m_cached) return true;
    findNext();
    return m_cached;
}

template<typename T>
Cardinal WhereGenerator<T>::GetPotentialSize() const {
    return Cardinal::Omega();
}

template<typename T>
Generator<T>* WhereGenerator<T>::Clone() const {
    auto copy = m_src ? std::shared_ptr<Generator<T>>(m_src->Clone()) : nullptr;
    return new WhereGenerator<T>(copy, m_pred);
}