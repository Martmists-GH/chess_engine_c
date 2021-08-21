#include "rendermain.h"

#include "GLFW/glfw3.h"
#define SK_GL
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "layers/background_layer.h"
#include "layers/piece_layer.h"
#include "layers/moves_layer.h"
#include "../engine/minmax_engine.h"
#include <cstdio>
#include <cstdlib>
#include <list>

#define INITIAL_SIZE 480

void ui_draw();
void doEngineMove();
void resizeCallback(GLFWwindow* window, int width, int height);
void clickCallback(GLFWwindow* window, int button, int action, int mods);
void cursorCallback(GLFWwindow* window, double xpos, double ypos);
void errorCallback(int error, const char* description);

static std::list<RenderLayer*> layers = std::list<RenderLayer*>();
static GLFWwindow* window = nullptr;
static GrDirectContext* sContext = nullptr;
static SkSurface* sSurface = nullptr;
static RenderContext ctx;
static GameState prevState = GameState::PLAYING;

void initSkia(int w, int h) {
    auto interface = GrGLMakeNativeInterface();
    sContext = GrDirectContext::MakeGL(interface).release();

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType = kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget(w, h, 0, 0, framebufferInfo);

    //(replace line below with this one to enable correct color spaces) sSurface = SkSurface::MakeFromBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
    sSurface = SkSurface::MakeFromBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
    if (sSurface == nullptr) abort();

    ctx.canvas = sSurface->getCanvas();
    ctx.size.width = w;
    ctx.size.height = h;
}

void cleanupSkia() {
    delete sSurface;
    delete sContext;
}

void setup_game() {
    ctx.board = standard();
    ctx.flipped = true;
    ctx.players[0].isHuman = true;
    ctx.players[0].engine = nullptr;
    ctx.players[1].isHuman = false;
    ctx.players[1].engine = new MinMaxEngine(6, 10);
}

void ui_init() {
    ctx.theme = DEFAULT_THEME;
    setup_game();

    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(INITIAL_SIZE, INITIAL_SIZE, "Chess", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 1, 1);

    initSkia(INITIAL_SIZE, INITIAL_SIZE);

    glfwSwapInterval(1);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, clickCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwShowWindow(window);

    layers.emplace_back(getBackgroundLayer());
    layers.emplace_back(getPieceLayer());
    layers.emplace_back(getMovesLayer());
}

void ui_loop() {
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
        ui_draw();
        sContext->flush();
        glfwSwapBuffers(window);

        if (ctx.board->state == GameState::PLAYING) {
            doEngineMove();
        }

        if (ctx.board->state != GameState::PLAYING && prevState == GameState::PLAYING) {
            prevState = ctx.board->state;
            switch(prevState) {
                case DRAW:
                    printf("Game ended in a draw\n");
                    break;
                case CHECKMATE_WHITE:
                    printf("Game ended in a win by checkmate for white\n");
                    break;
                case CHECKMATE_BLACK:
                    printf("Game ended in a win by checkmate for black\n");
                    break;
                case STALEMATE:
                    printf("Game ended in a draw by stalemate\n");
                    break;
            }
        }
    }

    cleanupSkia();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void doEngineMove() {
    auto cfg = &ctx.players[ctx.board->blackToMove];
    if (!cfg->isHuman) {
        auto mv = cfg->engine->process(ctx.board);
        move(ctx.board, mv);
    }
}

void ui_draw() {
    for (auto layer : layers) {
        layer->render(&ctx);
    }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
    initSkia(width, height);
}

void clickCallback(GLFWwindow* window, int button, int action, int mods) {
    for (auto layer : layers) {
        if (layer->onClick != nullptr) {
            layer->onClick((MetaContext*)&ctx, button, action, mods);
        }
    }
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    for (auto layer : layers) {
        if (layer->onMouse != nullptr) {
            layer->onMouse((MetaContext*)&ctx, xpos, ypos);
        }
    }
}

void errorCallback(int error, const char* description) {
    fputs(description, stderr);
}
