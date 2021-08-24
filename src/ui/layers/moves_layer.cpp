//
// Created by mart on 8/19/21.
//

#include <include/core/SkStream.h>
#include <modules/svg/include/SkSVGDOM.h>
#include "moves_layer.h"
#include "../pieces_svg.h"

static bool moving = false;
static int movingIndex = -1;
static float mouseX = -1;
static float mouseY = -1;
static std::vector<ChessMove> movesPossible;
static ChessMove promotionMove {};

static int PROMOTION_SQUARES[4] = {
        27, 28, 35, 36
};

void renderMoves(RenderContext* ctx) {
    float squareSize = ctx->size.width / 8.f;

    float sqX = mouseX - fmodf(mouseX, squareSize);
    float sqY = mouseY - fmodf(mouseY, squareSize);

    SkPaint pBorder, pCircle;
    pBorder.setColor(ctx->theme.border);
    pCircle.setColor(ctx->theme.meta);
    pBorder.setStroke(true);
    pBorder.setStrokeWidth(2.f);
    pCircle.setAntiAlias(true);
    SkRect square;

    if (moving) {
        int col = movingIndex % 8;
        int row = movingIndex / 8;

        square.setXYWH(col * squareSize, row * squareSize, squareSize, squareSize);

        for (const auto &item : movesPossible) {
            int idx = item.toIndex;

            if (ctx->flipped) {
                idx = 119 - idx;
            }

            row = idx / 10 - 2;
            col = idx % 10 - 1;

            ctx->canvas->drawCircle((col + .5f) * squareSize, (row + .5f) * squareSize, squareSize / 4, pCircle);
        }
    } else {
        square.setXYWH(sqX, sqY, squareSize, squareSize);
    }

    ctx->canvas->drawRect(square, pBorder);

    if (promotionMove.fromIndex != -1) {
        square.setXYWH(
                2.5f * squareSize,
                2.5f * squareSize,
                3.f * squareSize,
                3.f * squareSize
        );
        SkPaint pOverlay;
        pOverlay.setColor(ctx->theme.overlay);
        ctx->canvas->drawRect(square, pOverlay);

        SkMemoryStream stream(svg.c_str(), svg.length());
        auto root = SkSVGDOM::MakeFromStream(stream);

        for (auto i : PROMOTION_SQUARES) {
            int col = i % 8;
            int row = i / 8;

            PieceType promotion = PieceType::INVALID;
            switch (i) {
                case 27:
                    promotion = PieceType::KNIGHT;
                    break;
                case 28:
                    promotion = PieceType::BISHOP;
                    break;
                case 35:
                    promotion = PieceType::ROOK;
                    break;
                case 36:
                    promotion = PieceType::QUEEN;
                    break;
            }

            ctx->canvas->save();

            SkRect clip {
                0.f,
                0.f,
                squareSize,
                squareSize,
            };

            ctx->canvas->translate(squareSize * col, squareSize * row);

            ctx->canvas->clipRect(clip, false);

            ctx->canvas->save();
            ctx->canvas->scale(squareSize / 45.f, squareSize / 45.f);
            ctx->canvas->translate(-((float)promotion) * 45.f, -((float)ctx->board->pieces[promotionMove.fromIndex].black) * 45.f);
            root->render(ctx->canvas);
            ctx->canvas->restore();

            ctx->canvas->restore();
        }
    }
}

void clickMouse(MetaContext* ctx, int action, int button, int mods) {
    if (action == 0 && button == GLFW_MOUSE_BUTTON_LEFT && ctx->board->status == Status::PLAYING) {
        moving = !moving;

        float squareSize = ctx->size.width / 8.f;

        int sqX = (int)((mouseX - fmodf(mouseX, squareSize)) / squareSize);
        int sqY = (int)((mouseY - fmodf(mouseY, squareSize)) / squareSize);
        int idx = sqX + 8 * sqY;
        int bidx = 1 + sqX + 10 * (sqY + 2);
        if (ctx->flipped) {
            bidx = 119 - bidx;
        }

        if (promotionMove.fromIndex != -1) {
            PieceType promotion = PieceType::INVALID;
            switch (idx) {
                case 27:
                    promotion = PieceType::KNIGHT;
                    break;
                case 28:
                    promotion = PieceType::BISHOP;
                    break;
                case 35:
                    promotion = PieceType::ROOK;
                    break;
                case 36:
                    promotion = PieceType::QUEEN;
                    break;
            }

            if (promotion == PieceType::INVALID) {
                moving = false;
            } else {
                moving = false;
                promotionMove.promotion = promotion;
                ctx->board->move(promotionMove);
                ctx->board->update();
            }

            promotionMove.dummy();
            return;
        }

        if (moving) {
            if (ctx->board->pieces[bidx].type != PieceType::EMPTY && ctx->board->pieces[bidx].black == ctx->board->blackToMove) {
                // clicked 1st
                movingIndex = idx;
                ctx->board->getPossibleMoves(movesPossible, bidx);
                if (movesPossible.empty()) {
                    moving = false;
                    return;
                }
            } else {
                moving = false;
                return;
            }
        } else {
            // clicked 2nd; check moves and do them
            ChessMove mv { };
            mv.dummy();

            for (auto& move : movesPossible) {
                if (move.toIndex == bidx) {
                    mv = move;
                }
            }

            if (mv.fromIndex != -1) {
                if (mv.promotion != PieceType::INVALID) {
                    promotionMove = mv;
                } else {
                    ctx->board->move(mv);
                    ctx->board->update();
                }
            }

            // free resources
            movesPossible.clear();
            movingIndex = -1;
        }
    }
}

void movesMouse(MetaContext* ctx, double x, double y) {
    mouseX = (float)x;
    mouseY = (float)y;
}

static RenderLayer movesLayer {
    renderMoves,
    clickMouse,
    movesMouse
};

RenderLayer* getMovesLayer() {
    promotionMove.dummy();
    return &movesLayer;
}
