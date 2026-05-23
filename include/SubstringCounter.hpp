#pragma once
#include "IStream.hpp"
#include <string>
#include <vector>
#include <memory>
#include "LazySequence.hpp"
#include "LazySequenceStream.hpp"
class SubstringCounter {
public:
    static std::vector<int> buildPrefixFunction(const std::string& pattern) {
        int m = pattern.size();
        std::vector<int> pi(m, 0);
        for (int i = 1, j = 0; i < m; ++i) {
            while (j > 0 && pattern[i] != pattern[j])
                j = pi[j - 1];
            if (pattern[i] == pattern[j])
                ++j;
            pi[i] = j;
        }
        return pi;
    }

    static size_t countInStream(std::shared_ptr<IStream<char>> stream, const std::string& pattern) {
        if (pattern.empty()) return 0;
        auto pi = buildPrefixFunction(pattern);
        size_t count = 0;
        int j = 0;
        while (!stream->IsEnd()) {
            char c = stream->Read();
            while (j > 0 && c != pattern[j])
                j = pi[j - 1];
            if (c == pattern[j]) {
                ++j;
                if (j == (int)pattern.size()) {
                    ++count;
                    j = pi[j - 1];
                }
            }
        }
        return count;
    }
    static size_t countInLazySequence(const LazySequence<char, MutableArraySequence>& seq, const std::string& pattern) {
    auto stream = std::make_shared<LazySequenceStream<char>>(seq);
    return countInStream(stream, pattern);
}
};