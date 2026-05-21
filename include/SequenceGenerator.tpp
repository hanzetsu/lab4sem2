#include "SequenceGenerator.hpp"
#include "exceptions.hpp"

template<typename T, template<typename> class Container>
SequenceGenerator<T, Container>::SequenceGenerator(std::shared_ptr<Container<T>> seq)
    : data(seq), pos(0) {}

template<typename T, template<typename> class Container>
T SequenceGenerator<T, Container>::GetNext() {
    if (!HasNext()) throw OutOfRangeException("SequenceGenerator::GetNext");
    return data->Get(pos++);
}

template<typename T, template<typename> class Container>
bool SequenceGenerator<T, Container>::HasNext() const {
    return pos < data->GetLength();
}

template<typename T, template<typename> class Container>
Cardinal SequenceGenerator<T, Container>::GetPotentialSize() const {
    return Cardinal(data->GetLength());
}

template<typename T, template<typename> class Container>
Generator<T>* SequenceGenerator<T, Container>::Clone() const {
    return new SequenceGenerator<T, Container>(data);
}