#pragma once
#include "IStream.hpp"
#include "exceptions.hpp"
#include <string>
#include <memory>

class StringStream : public IStream<char> {
public:
    explicit StringStream(const std::string& str);
    char Read() override;
    bool IsEnd() const override;
    size_t GetPosition() const override { return pos; }
    void Reset() override { pos = 0; }
    std::shared_ptr<IStream<char>> Clone() const override;

private:
    std::string data;
    size_t pos = 0;
};

inline StringStream::StringStream(const std::string& str) : data(str) {}

inline char StringStream::Read() {
    if (IsEnd()) throw OutOfRangeException("StringStream: конец потока");
    return data[pos++];
}

inline bool StringStream::IsEnd() const {
    return pos >= data.size();
}

inline std::shared_ptr<IStream<char>> StringStream::Clone() const {
    auto copy = std::make_shared<StringStream>(data);
    copy->pos = pos;
    return copy;
}