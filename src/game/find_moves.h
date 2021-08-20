//
// Created by mart on 8/19/21.
//

#pragma once

#include <vector>
#include "board.h"

/**
 * Without legal moves
 */
std::vector<Move>* movesSimple(Board* board, int index);
void movesSimple(Board* board, int index, std::vector<Move>* ptr);

/**
 * With legal moves
 */
std::vector<Move>* movesSmart(Board* board, int index);
void movesSmart(Board* board, int index, std::vector<Move>*);