#pragma once

#include <vector>
#include <random>

static std::mt19937_64 generator(std::random_device{}());

int randRange(int min, int max);

template <typename T>
T randomItem(std::vector<T>& ptr) {
    std::uniform_int_distribution<int> distribution(0, ptr.size()-1);
    int index = distribution(generator);
    return ptr.at(index);
}
