//
// Created by mart on 8/16/21.
//

#include "background_layer.h"

void renderBoard(RenderContext* ctx) {
    ctx->canvas->clear(ctx->theme.background);

    SkPaint paint;
    paint.setColor(ctx->theme.backgroundAlt);

    float squareSize = ctx->size.width / 8.f;
    SkRect rect;

    for (int i = 0; i < 64; i++) {
        int row = i / 8;
        int col = i % 8;
        auto mv = ctx->board->lastMove;
        if (ctx->flipped) {
            mv.fromIndex = (char)rotateIndex(mv.fromIndex);
            mv.toIndex = (char)rotateIndex(mv.toIndex);
        }

        if ((row == mv.fromIndex / 10 - 2 && col == mv.fromIndex % 10 - 1) || (row == mv.toIndex / 10 - 2 && col == mv.toIndex % 10 - 1)) {
            rect.setXYWH(col * squareSize, row * squareSize, squareSize, squareSize);
            SkPaint p2;

            if (col % 2 == row % 2) {
                p2.setColor(ctx->theme.highlightAlt);
            } else {
                p2.setColor(ctx->theme.highlight);
            }

            ctx->canvas->drawRect(rect, p2);
        } else {
            if (col % 2 == row % 2) {
                rect.setXYWH(col * squareSize, row * squareSize, squareSize, squareSize);
                ctx->canvas->drawRect(rect, paint);
            }
        }
    }
}

static RenderLayer boardLayer {
    renderBoard,
    nullptr,
    nullptr
};

RenderLayer* getBackgroundLayer() {
    return &boardLayer;
}
