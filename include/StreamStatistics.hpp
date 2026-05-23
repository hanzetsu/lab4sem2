#pragma once
#include "IStream.hpp"
#include "MutableArraySequence.hpp"
#include <cmath>
#include <random>
#include <memory>
#include <QMetaType>

template<typename T>
static void insertionSort(MutableArraySequence<T>& arr) {
    for (size_t i = 1; i < arr.GetLength(); ++i) {
        T key = arr.Get(i);
        int j = i - 1;
        while (j >= 0 && arr.Get(j) > key) {
            arr.Set(j + 1, arr.Get(j));
            --j;
        }
        arr.Set(j + 1, key);
    }
}

class NaturalNumberStream : public IStream<int> {
private:
    int current = 0;
public:
    int Read() override { return current++; }
    bool IsEnd() const override { return false; }
    size_t GetPosition() const override { return current; }
    void Reset() override { current = 0; }
    std::shared_ptr<IStream<int>> Clone() const override {
        auto copy = std::make_shared<NaturalNumberStream>();
        copy->current = current;
        return copy;
    }
};

class RandomNumberStream : public IStream<int> {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist{0, 99};
    size_t pos = 0;
public:
    RandomNumberStream() : rng(std::random_device{}()) {}
    int Read() override { ++pos; return dist(rng); }
    bool IsEnd() const override { return false; }
    size_t GetPosition() const override { return pos; }
    void Reset() override { pos = 0; }
    std::shared_ptr<IStream<int>> Clone() const override {
        auto copy = std::make_shared<RandomNumberStream>();
        copy->pos = pos;
        return copy;
    }
};

class StreamStatistics {
public:
    struct Result {
        size_t count = 0;
        double sum = 0.0;
        double mean = 0.0;
        double variance = 0.0;
        double stddev = 0.0;
        double min = INFINITY;
        double max = -INFINITY;
        double median = 0.0;
    };

    struct OnlineStats {
        size_t count = 0;
        double sum = 0.0;
        double mean = 0.0;
        double m2 = 0.0;
        double min = INFINITY;
        double max = -INFINITY;
        size_t windowSize = 100;
        MutableArraySequence<double> window;
        size_t windowPos = 0;
        bool windowFull = false;

        void update(double val) {
            count++;
            sum += val;
            double delta = val - mean;
            mean += delta / count;
            double delta2 = val - mean;
            m2 += delta * delta2;
            if (val < min) min = val;
            if (val > max) max = val;
            if (window.GetLength() < windowSize) {
                window.Append(val);
            } else {
                if (!windowFull) windowFull = true;
                window.Set(windowPos, val);
            }
            windowPos = (windowPos + 1) % windowSize;
        }

        double getVariance() const {
            if (count < 2) return 0.0;
            return m2 / count;
        }

        double getStddev() const {
            return std::sqrt(getVariance());
        }

        double getMedian() {
            if (window.GetLength() == 0) return 0.0;
            MutableArraySequence<double> sorted = window;
            insertionSort(sorted);
            size_t m = sorted.GetLength();
            if (m % 2 == 0)
                return (sorted.Get(m/2 - 1) + sorted.Get(m/2)) / 2.0;
            else
                return sorted.Get(m/2);
        }

        Result getResult() const {
            Result res;
            res.count = count;
            res.sum = sum;
            res.mean = mean;
            res.variance = getVariance();
            res.stddev = getStddev();
            res.min = min;
            res.max = max;
            return res;
        }
    };

    static Result ComputeFinite(std::shared_ptr<IStream<int>> stream) {
        Result res;
        MutableArraySequence<double> values;
        while (!stream->IsEnd()) {
            double val = static_cast<double>(stream->Read());
            res.sum += val;
            res.count++;
            if (val < res.min) res.min = val;
            if (val > res.max) res.max = val;
            values.Append(val);
        }
        if (res.count == 0) return res;
        res.mean = res.sum / res.count;
        double sq = 0.0;
        for (size_t i = 0; i < values.GetLength(); ++i) {
            double v = values.Get(i);
            sq += (v - res.mean) * (v - res.mean);
        }
        res.variance = sq / res.count;
        res.stddev = std::sqrt(res.variance);
        insertionSort(values);
        size_t m = values.GetLength();
        if (m % 2 == 0)
            res.median = (values.Get(m/2 - 1) + values.Get(m/2)) / 2.0;
        else
            res.median = values.Get(m/2);
        return res;
    }
};

Q_DECLARE_METATYPE(StreamStatistics::OnlineStats)