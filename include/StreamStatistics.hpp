#pragma once
#include "IStream.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <memory>
#include <QMetaType>

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
        std::vector<double> window;
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
            if (window.size() < windowSize) {
                window.push_back(val);
            } else {
                if (!windowFull) windowFull = true;
                window[windowPos] = val;
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
            if (window.empty()) return 0.0;
            std::vector<double> sorted = window;
            std::sort(sorted.begin(), sorted.end());
            size_t m = sorted.size();
            if (m % 2 == 0)
                return (sorted[m/2 - 1] + sorted[m/2]) / 2.0;
            else
                return sorted[m/2];
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
        std::vector<double> values;
        while (!stream->IsEnd()) {
            double val = static_cast<double>(stream->Read());
            res.sum += val;
            res.count++;
            if (val < res.min) res.min = val;
            if (val > res.max) res.max = val;
            values.push_back(val);
        }
        if (res.count == 0) return res;
        res.mean = res.sum / res.count;
        double sq = 0.0;
        for (double v : values) sq += (v - res.mean) * (v - res.mean);
        res.variance = sq / res.count;
        res.stddev = std::sqrt(res.variance);
        std::sort(values.begin(), values.end());
        size_t m = values.size();
        if (m % 2 == 0)
            res.median = (values[m/2 - 1] + values[m/2]) / 2.0;
        else
            res.median = values[m/2];
        return res;
    }
};

Q_DECLARE_METATYPE(StreamStatistics::OnlineStats)