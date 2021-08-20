//
// Created by mart on 8/20/21.
//

#pragma once


#include "engine.h"

class RandomEngine : public Engine {
public:
    Move process(Board* state) override;
};
