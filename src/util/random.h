#pragma once

#include <vector>
#include "../game/move.h"

int randRange(int min, int max);

// fuck templates
void shuffleVector(std::vector<Move>* ptr);
