template<typename T, template<typename> class Container>
SequenceGenerator<T, Container>::SequenceGenerator(std::shared_ptr<Container<T>> seq)
    : m_data(seq) {}

template<typename T, template<typename> class Container>
T SequenceGenerator<T, Container>::GetNext() {
    if (!HasNext()) throw OutOfRangeException("SequenceGenerator end");
    return m_data->Get(m_pos++);
}

template<typename T, template<typename> class Container>
bool SequenceGenerator<T, Container>::HasNext() const {
    return m_pos < m_data->GetLength();
}

template<typename T, template<typename> class Container>
Cardinal SequenceGenerator<T, Container>::GetPotentialSize() const {
    return Cardinal::Finite(m_data->GetLength());
}

template<typename T, template<typename> class Container>
Generator<T>* SequenceGenerator<T, Container>::Clone() const {
    return new SequenceGenerator<T, Container>(m_data);
}