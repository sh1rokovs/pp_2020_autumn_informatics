// Copyright 2020 Gushchin Artem
#ifndef MODULES_TASK_3_GUSHCHIN_A_RADIX_WITH_MERGE_RADIX_WITH_MERGE_H_
#define MODULES_TASK_3_GUSHCHIN_A_RADIX_WITH_MERGE_RADIX_WITH_MERGE_H_

#include <vector>
#include <random>
#include <algorithm>

using uchar = unsigned char;

std::vector<int> parallelRadixSort(const std::vector<int>& source, const int sourceSize);

template<typename T>
std::vector<T> generateRandomVector(const int size) {
    std::random_device rd;
    std::mt19937 gen;
    gen.seed(rd());

    std::vector<T> randomVector(size);

    for (int i = 0; i < size; i++)
        randomVector[i] = gen() % 10000 - 5000;

    return randomVector;
}

template<typename T>
std::vector<long> calculateCounters(const std::vector<T>& data) {
    auto dataSize = data.size();

    std::vector<long> counters(256 * sizeof(T));

    const uchar* dataBytes = reinterpret_cast<const uchar*>(data.data());
    const uchar* dataEnd = reinterpret_cast<const uchar*>(data.data() + dataSize);

    while (dataBytes != dataEnd) {
        for (short i = 0; i < sizeof(T); i++) {
            counters[256 * static_cast<size_t>(i) + *dataBytes]++;
            dataBytes++;
        }
    }

    return counters;
}

template<typename T>
std::vector<T> radixPass(const std::vector<T>& source, std::vector<long> counter, short byteOffset) {
    long long partialSum = 0;
    long currentCount;
    for (short i = 0; i < 256; i++) {
        currentCount = counter[i];
        counter[i] = partialSum;
        partialSum += currentCount;
    }

    const uchar* sourceBytes = reinterpret_cast<const uchar*>(source.data());
    std::vector<T> dest(source.size());
    for (size_t i = 0; i < source.size(); i++) {
        dest[counter[sourceBytes[sizeof(T) * i + byteOffset]]] = source[i];
        counter[sourceBytes[sizeof(T) * i + byteOffset]]++;
    }

    return dest;
}

template<typename T>
std::vector<T> radixLastPassSigned(const std::vector<T>& source, std::vector<long> counter, short byteOffset) {
    long numNeg = 0;
    for (short i = 128; i < 256; i++)
        numNeg += counter[i];

    long long partialSum = numNeg;
    long currentCount;
    for (short i = 0; i < 128; i++) {
        currentCount = counter[i];
        counter[i] = partialSum;
        partialSum += currentCount;
    }

    partialSum = 0;
    for (short i = 128; i < 256; i++) {
        currentCount = counter[i];
        counter[i] = partialSum;
        partialSum += currentCount;
    }

    const uchar* sourceBytes = reinterpret_cast<const uchar *>(source.data());
    std::vector<T> dest(source.size());
    for (size_t i = 0; i < source.size(); i++) {
        dest[counter[sourceBytes[sizeof(T) * i + byteOffset]]] = source[i];
        counter[sourceBytes[sizeof(T) * i + byteOffset]]++;
    }

    return dest;
}

template<typename T>
std::vector<T> radixSortSigned(const std::vector<T>& in) {
    auto size = in.size();
    std::vector<T> out(in);

    auto counters = calculateCounters(in);

    for (uchar i = 0; i < sizeof(T); i++) {
        std::vector<long> counter(counters.begin() + 256 * static_cast<size_t>(i),
                                  counters.begin() + 256 * static_cast<size_t>(i) + 256);

        if (counter[0] == size)
            continue;

        if (i != sizeof(T) - 1)
            out = radixPass(out, counter, i);
        else
            out = radixLastPassSigned(out, counter, i);
    }

    return out;
}

template <typename T>
std::vector<T> mergeVectors(const std::vector<T>& source1, const std::vector<T>& source2) {
    std::vector<T> dest(source1.size() + source2.size());

    std::merge(source1.begin(), source1.end(), source2.begin(), source2.end(), dest.begin());

    return dest;
}

#endif  // MODULES_TASK_3_GUSHCHIN_A_RADIX_WITH_MERGE_RADIX_WITH_MERGE_H_
