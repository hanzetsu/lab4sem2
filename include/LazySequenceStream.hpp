#pragma once
#include "IStream.hpp"
#include "LazySequence.hpp"
#include "MutableArraySequence.hpp"
#include <memory>

template<typename T>
class LazySequenceStream : public IStream<T> {
public:
    explicit LazySequenceStream(const LazySequence<T, MutableArraySequence>& seq)
        : m_sequence(std::make_shared<LazySequence<T, MutableArraySequence>>(seq)),
          m_pos(0),
          m_length(m_sequence->GetSizeSequence()) {}

    T Read() override {
        if (IsEnd()) throw OutOfRangeException("LazySequenceStream end");
        T val = m_sequence->Get(Cardinal(m_pos));
        ++m_pos;
        return val;
    }

    bool IsEnd() const override {
        if (m_length.IsInfinite()) return false;
        return m_pos >= m_length.GetValue();
    }

    size_t GetPosition() const override { return m_pos; }
    void Reset() override { m_pos = 0; }

    std::shared_ptr<IStream<T>> Clone() const override {
        auto copy = std::make_shared<LazySequenceStream<T>>(*m_sequence);
        copy->m_pos = m_pos;
        return copy;
    }

private:
    std::shared_ptr<LazySequence<T, MutableArraySequence>> m_sequence;
    size_t m_pos;
    Cardinal m_length;
};