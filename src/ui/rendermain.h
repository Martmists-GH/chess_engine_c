//
// Created by mart on 8/16/21.
//

#pragma once

#define SK_GL
#include <GLFW/glfw3.h>
#include <include/core/SkCanvas.h>
#include "theme.h"
#include "../game/board.h"
#include "../engine/engine.h"

typedef struct {
    bool isHuman;
    Engine* engine;
} PlayerConfig;

typedef struct {
    struct {
        int width;
        int height;
    } size;
    Board* board;
    PlayerConfig players[2];
    bool flipped;
} MetaContext;

typedef struct {
    struct {
        int width;
        int height;
    } size;
    Board* board;
    PlayerConfig players[2];
    bool flipped;
    Theme theme;
    SkCanvas* canvas;
} RenderContext;

typedef struct {
    void (*render)(RenderContext * surface);
    void (*onClick)(MetaContext* ctx, int button, int action, int mods);
    void (*onMouse)(MetaContext* ctx, double x, double y);
} RenderLayer;

void ui_init();
void ui_loop();
