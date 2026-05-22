#pragma once
#include "Cardinal.hpp"
#include "Option.hpp"
#include <memory>

template<typename T>
class IStream {
public:
    virtual ~IStream() = default;
    virtual T Read() = 0;
    virtual bool IsEnd() const = 0;
    virtual Option<T> TryRead() {
        if (IsEnd()) return Option<T>();
        return Option<T>(Read());
    }
    virtual size_t GetPosition() const = 0;
    virtual void Reset() = 0;
    virtual std::shared_ptr<IStream<T>> Clone() const = 0;
};