#pragma once
#include "IStream.hpp"
#include "MutableArraySequence.hpp"
#include "exceptions.hpp"
#include <functional>
#include <memory>

template<typename T>
std::shared_ptr<MutableArraySequence<T>> CollectToSequence(std::shared_ptr<IStream<T>> stream) {
    auto result = std::make_shared<MutableArraySequence<T>>();
    while (!stream->IsEnd()) {
        result->Append(stream->Read());
    }
    return result;
}

template<typename T>
class FilterStream : public IStream<T> {
public:
    FilterStream(std::shared_ptr<IStream<T>> src, std::function<bool(const T&)> pred);
    T Read() override;
    bool IsEnd() const override;
    size_t GetPosition() const override { return src->GetPosition(); }
    void Reset() override { src->Reset(); hasCached = false; }
    std::shared_ptr<IStream<T>> Clone() const override;
private:
    std::shared_ptr<IStream<T>> source;
    std::function<bool(const T&)> predicate;
    mutable T nextCache;
    mutable bool hasCached = false;
    bool findNext() const;
};

template<typename T>
FilterStream<T>::FilterStream(std::shared_ptr<IStream<T>> src, std::function<bool(const T&)> pred)
    : source(src), predicate(pred) {}

template<typename T>
bool FilterStream<T>::findNext() const {
    while (!source->IsEnd()) {
        T val = source->Read();
        if (predicate(val)) {
            nextCache = val;
            hasCached = true;
            return true;
        }
    }
    hasCached = false;
    return false;
}

template<typename T>
T FilterStream<T>::Read() {
    if (!hasCached) findNext();
    if (!hasCached) throw OutOfRangeException("FilterStream: нет элементов");
    T res = nextCache;
    hasCached = false;
    return res;
}

template<typename T>
bool FilterStream<T>::IsEnd() const {
    if (hasCached) return false;
    return !findNext();
}

template<typename T>
std::shared_ptr<IStream<T>> FilterStream<T>::Clone() const {
    auto clone = std::make_shared<FilterStream<T>>(source->Clone(), predicate);
    clone->hasCached = hasCached;
    clone->nextCache = nextCache;
    return clone;
}

// Ленивое преобразование потока (map)
template<typename T, typename U>
class MapStream : public IStream<U> {
public:
    MapStream(std::shared_ptr<IStream<T>> src, std::function<U(const T&)> func);
    U Read() override;
    bool IsEnd() const override;
    size_t GetPosition() const override { return src->GetPosition(); }
    void Reset() override { src->Reset(); }
    std::shared_ptr<IStream<U>> Clone() const override;
private:
    std::shared_ptr<IStream<T>> source;
    std::function<U(const T&)> mapper;
};

template<typename T, typename U>
MapStream<T,U>::MapStream(std::shared_ptr<IStream<T>> src, std::function<U(const T&)> f)
    : source(src), mapper(f) {}

template<typename T, typename U>
U MapStream<T,U>::Read() {
    return mapper(source->Read());
}

template<typename T, typename U>
bool MapStream<T,U>::IsEnd() const {
    return source->IsEnd();
}

template<typename T, typename U>
std::shared_ptr<IStream<U>> MapStream<T,U>::Clone() const {
    return std::make_shared<MapStream<T,U>>(source->Clone(), mapper);
}