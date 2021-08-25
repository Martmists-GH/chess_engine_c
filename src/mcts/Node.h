//
// Created by mart on 8/24/21.
//

#pragma once

#include <cstdio>
#include <vector>
#include <cmath>
#include <memory>
#include <cstring>
#include "../util/random.h"
#include "../game/ChessGameState.h"

class Node {
public:
    bool player2 = false;
    ChessMove lastMove;
    std::vector<Node*> children;
    std::vector<ChessMove> possibleMoves;
    int C = 2;
    float won = 0.f;
    float total = 0.f;
    Node* parent = nullptr;
    ChessGameState currentState;

    Node(ChessGameState* state, Node* parent, ChessMove mv);
    Node(ChessGameState* state, Node* parent);
    Node(ChessGameState* state, ChessMove move);
    explicit Node(ChessGameState* state);

    ~Node();

    void generateMoves();

    void doMCTS(int depth);
    void selection();
    void expansion();
    void simulation(int depth);
    void backpropagation(int winner);

    float ucb1() const;

    void addChild(ChessMove move);
    Node* getChild(ChessMove move);
    void clearChildren(Node *except);

    ChessMove getBestMove();
};
