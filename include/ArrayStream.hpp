#pragma once
#include "IStream.hpp"
#include "Sequence.hpp"
#include "MutableArraySequence.hpp"
#include "exceptions.hpp"
#include <memory>

template<typename T>
class ArrayStream : public IStream<T> {
public:
     ArrayStream(std::shared_ptr<Sequence<T>> seq);
     ArrayStream(const Sequence<T>& seq);
    T Read() override;
    bool IsEnd() const override;
    size_t GetPosition() const override { return pos; }
    void Reset() override { pos = 0; }
    std::shared_ptr<IStream<T>> Clone() const override;

private:
    std::shared_ptr<Sequence<T>> data;
    size_t pos = 0;
};

template<typename T>
ArrayStream<T>::ArrayStream(std::shared_ptr<Sequence<T>> seq) : data(seq) {}

template<typename T>
ArrayStream<T>::ArrayStream(const Sequence<T>& seq)
    : data(std::make_shared<MutableArraySequence<T>>()) {
    for (size_t i = 0; i < seq.GetLength(); ++i)
        data->Append(seq.Get(i));
}

template<typename T>
T ArrayStream<T>::Read() {
    if (IsEnd()) throw OutOfRangeException("ArrayStream: конец потока");
    return data->Get(pos++);
}

template<typename T>
bool ArrayStream<T>::IsEnd() const {
    return pos >= data->GetLength();
}

template<typename T>
std::shared_ptr<IStream<T>> ArrayStream<T>::Clone() const {
    auto copy = std::make_shared<ArrayStream<T>>(data);
    copy->pos = pos;
    return copy;
}