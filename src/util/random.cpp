//
// Created by mart on 8/20/21.
//

#include <random>
#include "random.h"

static std::mt19937_64 generator(std::random_device{}());

int randRange(int min, int max) {
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

void shuffleVector(std::vector<Move>* ptr) {
    std::shuffle(ptr->begin(), ptr->end(), generator);
}
