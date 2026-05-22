#pragma once
#include "IStream.hpp"
#include "exceptions.hpp"
#include <fstream>
#include <string>
#include <memory>
#include <functional>

template<typename T>
class FileStream : public IStream<T> {
public:
    FileStream(const std::string& filename, std::function<T(const std::string&)> deserialize);
    ~FileStream() { if (file.is_open()) file.close(); }
    T Read() override;
    bool IsEnd() const override;
    size_t GetPosition() const override { return pos; }
    void Reset() override;
    std::shared_ptr<IStream<T>> Clone() const override;

private:
    std::string filename;
    std::function<T(const std::string&)> deserialize;
    mutable std::ifstream file;
    size_t pos = 0;
    bool eof = false;
    void openFile() const;
};

template<typename T>
FileStream<T>::FileStream(const std::string& fname, std::function<T(const std::string&)> d)
    : filename(fname), deserialize(d) {
    openFile();
}

template<typename T>
void FileStream<T>::openFile() const {
    if (file.is_open()) file.close();
    file.open(filename);
    eof = false;
}

template<typename T>
T FileStream<T>::Read() {
    if (IsEnd()) throw OutOfRangeException("FileStream: конец файла");
    std::string line;
    std::getline(file, line);
    ++pos;
    if (file.eof()) eof = true;
    return deserialize(line);
}

template<typename T>
bool FileStream<T>::IsEnd() const {
    if (eof) return true;
    if (file.peek() == std::ifstream::traits_type::eof()) {
        const_cast<FileStream*>(this)->eof = true;
        return true;
    }
    return false;
}

template<typename T>
void FileStream<T>::Reset() {
    openFile();
    pos = 0;
    eof = false;
}

template<typename T>
std::shared_ptr<IStream<T>> FileStream<T>::Clone() const {
    auto copy = std::make_shared<FileStream<T>>(filename, deserialize);
    copy->pos = pos;
    copy->eof = eof;
    copy->openFile();
    for (size_t i = 0; i < pos; ++i) {
        std::string dummy;
        std::getline(copy->file, dummy);
    }
    return copy;
}