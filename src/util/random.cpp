//
// Created by mart on 8/20/21.
//

#include <random>
#include "random.h"

int randRange(int min, int max) {
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}
