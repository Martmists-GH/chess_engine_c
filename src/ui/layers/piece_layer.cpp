//
// Created by mart on 8/17/21.
//

#include <include/core/SkStream.h>
#include <modules/svg/include/SkSVGDOM.h>
#include "piece_layer.h"
#include "../pieces_svg.h"

static ChessGameState renderBoard;

void renderPieces(RenderContext* ctx) {
    auto canvas = ctx->canvas;
    float squareSize = ctx->size.width / 8.f;

    SkMemoryStream stream(svg.c_str(), svg.length());
    auto root = SkSVGDOM::MakeFromStream(stream);

    renderBoard = *ctx->board;
//    rotate(&renderBoard);

    for (int i = 0; i < 64; i++) {
        int col = i % 8;
        int row = i / 8;
        int idx = (10 * (row + 2)) + (col + 1);
        if (ctx->flipped) {
            idx = 119 - idx;
        }
        auto p = renderBoard.pieces[idx];

        if (p.type == PieceType::EMPTY) continue;

        canvas->save();

        SkRect clip {
            0.f,
            0.f,
            squareSize,
            squareSize,
        };

        canvas->translate(squareSize * col, squareSize * row);

        canvas->clipRect(clip, false);

        canvas->save();
        canvas->scale(squareSize / 45.f, squareSize / 45.f);
        canvas->translate(-((float)p.type) * 45.f, -((float)p.black) * 45.f);
        root->render(canvas);
        canvas->restore();

        canvas->restore();
    }
}

static RenderLayer pieceLayer {
    renderPieces,
    nullptr,
    nullptr
};

RenderLayer* getPieceLayer() {
    return &pieceLayer;
}
