#include "SequenceGenerator.hpp"
#include "RecurrentGenerator.hpp"
#include "CompositeGenerator.hpp"
#include "MapGenerator.hpp"
#include "WhereGenerator.hpp"
#include "exceptions.hpp"

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence()
    : m_gen(nullptr), m_cache() {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(const Container<T>& cache)
    : m_gen(nullptr), m_cache(cache) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(const Container<T>& cache, std::shared_ptr<Generator<T>> gen)
    : m_gen(gen), m_cache(cache) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(T* items, size_t count)
    : m_gen(nullptr), m_cache(items, count) {}

template<typename T, template<typename> class Container>
LazySequence<T, Container>::LazySequence(std::initializer_list<T> list)
    : m_gen(nullptr), m_cache() {
    for (const T& val : list) m_cache.Append(val);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::FromGenerator(std::shared_ptr<Generator<T>> gen) {
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Recurrent(std::function<T(const Container<T>&)> func, const Container<T>& start) {
    auto gen = std::make_shared<RecurrentGenerator<T, Container>>(func, start);
    return LazySequence(start, gen);
}

template<typename T, template<typename> class Container>
void LazySequence<T, Container>::EnsureMaterialized(size_t idx) {
    while (m_cache.GetLength() <= idx) {
        if (!m_gen || !m_gen->HasNext())
            throw OutOfRangeException("Not enough elements");
        m_cache.Append(m_gen->GetNext());
    }
}

template<typename T, template<typename> class Container>
Cardinal LazySequence<T, Container>::GetSizeSequence() const {
    if (m_gen) return m_gen->GetPotentialSize();
    return Cardinal::Finite(m_cache.GetLength());
}

template<typename T, template<typename> class Container>
size_t LazySequence<T, Container>::GetSizeCache() const {
    return m_cache.GetLength();
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::GetFirst() {
    if (m_cache.GetLength() == 0 && (!m_gen || !m_gen->HasNext()))
        throw EmptySequenceException("Empty sequence");
    if (m_cache.GetLength() == 0) EnsureMaterialized(0);
    return m_cache.Get(0);
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::GetLast() {
    Cardinal total = GetSizeSequence();
    if (total.IsInfinite())
        throw InvalidArgumentException("GetLast on infinite sequence");
    if (total.GetValue() == 0)
        throw EmptySequenceException("Empty sequence");
    size_t last = total.GetValue() - 1;
    EnsureMaterialized(last);
    return m_cache.Get(last);
}

template<typename T, template<typename> class Container>
T LazySequence<T, Container>::Get(Cardinal index) {
    if (index.IsInfinite()) throw OutOfRangeException("Infinite index");
    size_t idx = index.GetValue();
    EnsureMaterialized(idx);
    return m_cache.Get(idx);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::GetSubsequence(size_t start, size_t end) const {
    if (start > end) throw InvalidArgumentException("start > end");
    const_cast<LazySequence*>(this)->EnsureMaterialized(end);
    Container<T> sub;
    for (size_t i = start; i <= end; ++i)
        sub.Append(m_cache.Get(i));
    return LazySequence(sub);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Append(T item) const {
    struct AppendGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> seq;
        T val;
        size_t pos = 0;
        bool srcDone = false;
        AppendGen(std::shared_ptr<LazySequence<T, Container>> s, T v) : seq(s), val(v) {}
        T GetNext() override {
            if (!srcDone) {
                try { return seq->Get(Cardinal(pos++)); }
                catch (const OutOfRangeException&) { srcDone = true; }
            }
            return val;
        }
        bool HasNext() const override { return !srcDone; }
        Cardinal GetPotentialSize() const override {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite()) return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }
        Generator<T>* Clone() const override { return new AppendGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<AppendGen>(self, item);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Prepend(T item) const {
    struct PrependGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> seq;
        T val;
        size_t pos = 0;
        bool returned = false;
        PrependGen(std::shared_ptr<LazySequence<T, Container>> s, T v) : seq(s), val(v) {}
        T GetNext() override {
            if (!returned) {
                returned = true;
                return val;
            }
            try { return seq->Get(Cardinal(pos++)); }
            catch (const OutOfRangeException&) { throw OutOfRangeException("PrependGen end"); }
        }
        bool HasNext() const override {
            if (!returned) return true;
            auto sz = seq->GetSizeSequence();
            return sz.IsInfinite() || pos < sz.GetValue();
        }
        Cardinal GetPotentialSize() const override {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite()) return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }
        Generator<T>* Clone() const override { return new PrependGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<PrependGen>(self, item);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::InsertAt(T item, size_t index) const {
    struct InsertGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> seq;
        size_t idx;
        T val;
        size_t pos = 0;
        bool inserted = false;
        InsertGen(std::shared_ptr<LazySequence<T, Container>> s, size_t i, T v) : seq(s), idx(i), val(v) {}
        T GetNext() override {
            if (!inserted && pos == idx) {
                inserted = true;
                ++pos;
                return val;
            }
            try { return seq->Get(Cardinal(pos++)); }
            catch (const OutOfRangeException&) { throw OutOfRangeException("InsertGen end"); }
        }
        bool HasNext() const override {
            if (!inserted && pos <= idx) return true;
            auto sz = seq->GetSizeSequence();
            return sz.IsInfinite() || pos < sz.GetValue();
        }
        Cardinal GetPotentialSize() const override {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite()) return Cardinal::Omega();
            return Cardinal::Finite(sz.GetValue() + 1);
        }
        Generator<T>* Clone() const override { return new InsertGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<InsertGen>(self, index, item);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Concat(const LazySequence<T, Container>& other) const {
    struct ConcatGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> first;
        std::shared_ptr<LazySequence<T, Container>> second;
        size_t posFirst = 0;
        size_t posSecond = 0;
        bool firstDone = false;
        ConcatGen(std::shared_ptr<LazySequence<T, Container>> f, std::shared_ptr<LazySequence<T, Container>> s)
            : first(f), second(s) {}
        T GetNext() override {
            if (!firstDone) {
                try {
                    T val = first->Get(Cardinal(posFirst));
                    ++posFirst;
                    return val;
                } catch (const OutOfRangeException&) {
                    firstDone = true;
                }
            }
            T val = second->Get(Cardinal(posSecond));
            ++posSecond;
            return val;
        }
        bool HasNext() const override {
            if (!firstDone) return true;
            auto sz2 = second->GetSizeSequence();
            return sz2.IsInfinite() || posSecond < sz2.GetValue();
        }
        Cardinal GetPotentialSize() const override {
            auto sz1 = first->GetSizeSequence();
            auto sz2 = second->GetSizeSequence();
            if (sz1.IsInfinite() || sz2.IsInfinite()) return Cardinal::Omega();
            return Cardinal::Finite(sz1.GetValue() + sz2.GetValue());
        }
        Generator<T>* Clone() const override { return new ConcatGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto otherPtr = std::make_shared<LazySequence<T, Container>>(other);
    auto gen = std::make_shared<ConcatGen>(self, otherPtr);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::SkipFirst(size_t count) const {
    if (count == 0) return *this;
    Cardinal total = GetSizeSequence();
    if (total.IsFinite() && count >= total.GetValue()) return LazySequence();
    struct SkipGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> seq;
        size_t skip;
        size_t skipped = 0;
        SkipGen(std::shared_ptr<LazySequence<T, Container>> s, size_t c) : seq(s), skip(c) {}
        T GetNext() override {
            while (skipped < skip) {
                try { seq->Get(Cardinal(0)); } catch (...) {}
                ++skipped;
            }
            return seq->Get(Cardinal(skipped++));
        }
        bool HasNext() const override { return true; }
        Cardinal GetPotentialSize() const override {
            auto sz = seq->GetSizeSequence();
            if (sz.IsInfinite()) return Cardinal::Omega();
            if (skip >= sz.GetValue()) return Cardinal::Finite(0);
            return Cardinal::Finite(sz.GetValue() - skip);
        }
        Generator<T>* Clone() const override { return new SkipGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<SkipGen>(self, count);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
template<typename U>
LazySequence<U, Container> LazySequence<T, Container>::Map(std::function<U(const T&)> func) const {
    struct MapGen : Generator<U> {
        std::shared_ptr<LazySequence<T, Container>> src;
        std::function<U(const T&)> f;
        size_t pos = 0;
        MapGen(std::shared_ptr<LazySequence<T, Container>> s, std::function<U(const T&)> fn) : src(s), f(fn) {}
        U GetNext() override { return f(src->Get(Cardinal(pos++))); }
        bool HasNext() const override {
            auto sz = src->GetSizeSequence();
            return sz.IsInfinite() || pos < sz.GetValue();
        }
        Cardinal GetPotentialSize() const override { return src->GetSizeSequence(); }
        Generator<U>* Clone() const override { return new MapGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<MapGen>(self, func);
    return LazySequence<U, Container>(Container<U>(), gen);
}

template<typename T, template<typename> class Container>
LazySequence<T, Container> LazySequence<T, Container>::Where(std::function<bool(const T&)> pred) const {
    struct WhereGen : Generator<T> {
        std::shared_ptr<LazySequence<T, Container>> src;
        std::function<bool(const T&)> p;
        size_t pos = 0;
        WhereGen(std::shared_ptr<LazySequence<T, Container>> s, std::function<bool(const T&)> pr) : src(s), p(pr) {}
        T GetNext() override {
            while (true) {
                T val = src->Get(Cardinal(pos++));
                if (p(val)) return val;
            }
        }
        bool HasNext() const override { return true; }
        Cardinal GetPotentialSize() const override { return Cardinal::Omega(); }
        Generator<T>* Clone() const override { return new WhereGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto gen = std::make_shared<WhereGen>(self, pred);
    return LazySequence(Container<T>(), gen);
}

template<typename T, template<typename> class Container>
template<typename U>
U LazySequence<T, Container>::Reduce(std::function<U(const U&, const T&)> func, U init) const {
    U acc = init;
    for (size_t i = 0; ; ++i) {
        try { acc = func(acc, const_cast<LazySequence*>(this)->Get(Cardinal(i))); }
        catch (const OutOfRangeException&) { break; }
    }
    return acc;
}

template<typename T, template<typename> class Container>
template<typename U>
LazySequence<std::pair<T, U>, Container> LazySequence<T, Container>::Zip(const LazySequence<U, Container>& other) const {
    struct ZipGen : Generator<std::pair<T, U>> {
        std::shared_ptr<LazySequence<T, Container>> first;
        std::shared_ptr<LazySequence<U, Container>> second;
        size_t pos = 0;
        ZipGen(std::shared_ptr<LazySequence<T, Container>> f, std::shared_ptr<LazySequence<U, Container>> s)
            : first(f), second(s) {}
        std::pair<T, U> GetNext() override {
            return {first->Get(Cardinal(pos)), second->Get(Cardinal(pos++))};
        }
        bool HasNext() const override {
            auto sz1 = first->GetSizeSequence();
            auto sz2 = second->GetSizeSequence();
            if (sz1.IsInfinite() && sz2.IsInfinite()) return true;
            if (sz1.IsFinite() && sz2.IsFinite())
                return pos < sz1.GetValue() && pos < sz2.GetValue();
            if (sz1.IsInfinite())
                return pos < sz2.GetValue();
            return pos < sz1.GetValue();
        }
        Cardinal GetPotentialSize() const override {
            auto sz1 = first->GetSizeSequence();
            auto sz2 = second->GetSizeSequence();
            if (sz1.IsInfinite() && sz2.IsInfinite()) return Cardinal::Omega();
            if (sz1.IsInfinite()) return sz2;
            if (sz2.IsInfinite()) return sz1;
            return Cardinal::Finite(std::min(sz1.GetValue(), sz2.GetValue()));
        }
        Generator<std::pair<T, U>>* Clone() const override { return new ZipGen(*this); }
    };
    auto self = std::make_shared<LazySequence<T, Container>>(*this);
    auto otherPtr = std::make_shared<LazySequence<U, Container>>(other);
    auto gen = std::make_shared<ZipGen>(self, otherPtr);
    return LazySequence<std::pair<T, U>, Container>(Container<std::pair<T, U>>(), gen);
}