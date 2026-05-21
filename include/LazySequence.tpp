#include "SequenceGenerator.hpp"
#include "RecurrentGenerator.hpp"
#include "CompositeGenerator.hpp"
#include "MapGenerator.hpp"
#include "WhereGenerator.hpp"
#include "exceptions.hpp"

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence()
    : m_generator(nullptr), m_cache() {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(const Container<T>& cache)
    : m_generator(nullptr), m_cache(cache) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(T* items, size_t count)
    : m_generator(nullptr), m_cache(items, count) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(const Container<T>& cache, std::shared_ptr<Generator<T>> generator)
    : m_generator(generator), m_cache(cache) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(std::initializer_list<T> list)
    : m_generator(nullptr), m_cache() {
    for (const T& val : list) m_cache.Append(val);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::FromGenerator(std::shared_ptr<Generator<T>> gen) {
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Recurrent(
    std::function<T(const Container<T>&)> func,
    const Container<T>& startCache) {
    auto gen = std::make_shared<RecurrentGenerator<T, Container>>(func, startCache);
    return LazySequence(startCache, gen);
}

template<typename T, template<typename> class Container>
void LazySequence<T, Container>::EnsureMaterialized(size_t index) {
    while (m_cache.GetLength() <= index) {
        if (!m_generator || !m_generator->HasNext()) {
            throw OutOfRangeException("Недостаточно элементов");
        }
        m_cache.Append(m_generator->GetNext());
    }
}

template<typename T, template<typename> class Container>
Cardinal LazySequence<T, Container>::GetSizeSequence() const {
    if (m_generator) return m_generator->GetPotentialSize();
    return Cardinal(m_cache.GetLength());
}

template<typename T, template<typename> class Container>
size_t LazySequence<T, Container>::GetSizeCache() const {
    return m_cache.GetLength();
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::GetFirst() {
    if (m_cache.GetLength() == 0 && (!m_generator || !m_generator->HasNext())) {
        throw EmptySequenceException("LazySequence пуста");
    }
    if (m_cache.GetLength() == 0) EnsureMaterialized(0);
    return m_cache.Get(0);
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::GetLast() {
    Cardinal total = GetSizeSequence();
    if (total.IsInfiniteNumber()) {
        throw InvalidArgumentException("GetLast на бесконечной последовательности");
    }
    if (total.GetSize() == 0) {
        throw EmptySequenceException("LazySequence пуста");
    }
    size_t last = total.GetSize() - 1;
    EnsureMaterialized(last);
    return m_cache.Get(last);
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::Get(Cardinal index) {
    if (index.IsInfiniteNumber()) throw OutOfRangeException("Индекс бесконечен");
    size_t idx = index.GetSize();
    EnsureMaterialized(idx);
    return m_cache.Get(idx);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::GetSubsequence(size_t startIndex, size_t endIndex) {
    if (startIndex > endIndex) throw InvalidArgumentException("start > end");
    EnsureMaterialized(endIndex);
    Container<T> sub;
    for (size_t i = startIndex; i <= endIndex; ++i) sub.Append(m_cache.Get(i));
    return LazySequence(sub);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Append(T item) const {
    struct AppendGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> seq;
        T val;
        size_t pos = 0;
        bool sourceFinished = false;
        AppendGen(std::shared_ptr<LazySequence<T, Container>> s, T v) : seq(s), val(v) {}
        T GetNext() override {
            if (!sourceFinished) {
                try {
                    return seq->Get(Cardinal(pos++));
                } catch (const OutOfRangeException&) {
                    sourceFinished = true;
                }
            }
            return val;
        }
        bool HasNext() const override { return !sourceFinished; }
        Cardinal GetPotentialSize() const override {
            if (seq->GetSizeSequence().IsInfiniteNumber()) return Cardinal::Omega();
            return Cardinal(seq->GetSizeSequence().GetSize() + 1);
        }
        Generator<T>* Clone() const override { return new AppendGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<AppendGen>(self, item);
    return LazySequence(Container<T>(), gen);
}