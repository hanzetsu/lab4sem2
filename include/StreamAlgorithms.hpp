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
    FilterStream(std::shared_ptr<IStream<T>> src, std::function<bool(const T&)> pred)
        : source(src), predicate(pred) {}

    T Read() override {
        if (!hasCached) findNext();
        if (!hasCached) throw OutOfRangeException("FilterStream: нет элементов");
        T res = nextCache;
        hasCached = false;
        return res;
    }

    bool IsEnd() const override {
        if (hasCached) return false;
        return !findNext();
    }

    size_t GetPosition() const override { return source->GetPosition(); }
    void Reset() override { source->Reset(); hasCached = false; }

    std::shared_ptr<IStream<T>> Clone() const override {
        auto clone = std::make_shared<FilterStream<T>>(source->Clone(), predicate);
        clone->hasCached = hasCached;
        clone->nextCache = nextCache;
        return clone;
    }

private:
    std::shared_ptr<IStream<T>> source;
    std::function<bool(const T&)> predicate;
    mutable T nextCache;
    mutable bool hasCached = false;

    bool findNext() const {
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
};
template<typename T, typename U>
class MapStream : public IStream<U> {
public:
    MapStream(std::shared_ptr<IStream<T>> src, std::function<U(const T&)> func)
        : source(src), mapper(func) {}

    U Read() override {
        return mapper(source->Read());
    }

    bool IsEnd() const override {
        return source->IsEnd();
    }

    size_t GetPosition() const override { return source->GetPosition(); }
    void Reset() override { source->Reset(); }

    std::shared_ptr<IStream<U>> Clone() const override {
        return std::make_shared<MapStream<T,U>>(source->Clone(), mapper);
    }

private:
    std::shared_ptr<IStream<T>> source;
    std::function<U(const T&)> mapper;
};