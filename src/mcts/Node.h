//
// Created by mart on 8/24/21.
//

#pragma once

#include <cstdio>
#include <vector>
#include <cmath>
#include "GameState.h"
#include "Move.h"

#define StateT Derived<GameState<M>>

// forward decl
template <MoveT M, StateT S> class Node;

template<MoveT M, StateT S>
bool _sort(const Node<M, S>& a, const Node<M, S>& b) {
    return a.ucb1() < b.ucb1();
}

template<MoveT M, StateT S>
bool _sortBest(const Node<M, S>& a, const Node<M, S>& b) {
    return (a.won / a.total) < (b.won / b.total);
}

template <MoveT M, StateT S>
class Node {
private:
    int player = 0;
    M lastMove;
    std::vector<Node> children;
    std::vector<M> possibleMoves;

    void generateMoves() {
        this->possibleMoves.reserve(20);
        currentState.getPossibleMoves(this->possibleMoves);
        std::random_shuffle(possibleMoves.begin(),  possibleMoves.end());
//        this->possibleMoves.resize(C);
    }

    void selection() {
        if (this->children.empty()) {
            this->expansion();
        }

        if (children.size() > 1) {
            printf("Sorting children by ucb1\n");
            std::sort(children.begin(), children.end(), &_sort<M, S>);
        }
    }

    void expansion() {
        if (this->possibleMoves.empty()) {
            generateMoves();
        }

        if (this->children.empty()) {
            this->children.reserve(50);  // Good enough for most cases
            for (auto &mv : possibleMoves) {
                printf("Move: (%d, %d)\n", mv.fromIndex, mv.toIndex);
                addChild(mv);
            }
        }

        printf("Generated %d moves with %d children\n", possibleMoves.size(), children.size());
    }

    void simulation(int depth) {
        if (currentState.getWinner() != -1) {
            backpropagation(currentState.getWinner());
            return;
        }

        if (depth == 0) {
            backpropagation(currentState.getWinningPlayer());
            return;
        }

        if (this->possibleMoves.empty()) {
            expansion();
        }

        if (this->possibleMoves.empty()) {
            // no moves?? but also no winner or draw??
            printf("??????\n");
            return;
        }

        M &move = randomItem(possibleMoves);
        Node<M, S> nodeN(currentState, this, move);
        nodeN.simulation(depth - 1);
    }

    void backpropagation(int winner) {
        printf("Winner: %d, backpropagating\n", winner);

        total += 1;
        if (winner == currentState.getMaxPlayers()) {
            won += 0.5f;
        } else if (winner == player) {
            won += 1;
        }

//        printf("t = %.1f, n = %.1f\n", won, total);

        if (parent != nullptr) {
            parent->backpropagation(winner);
        }
    }

public:
    int C = 2;

    float won = 0.f;
    float total = 0.f;
    Node* parent = nullptr;
    S currentState;

    Node(S &state, Node* parent, M move) : parent(parent) {
        currentState = state;
        player = currentState.getCurrentPlayer();
        M tmp;
        if (tmp.hash() != move.hash()) {
            currentState.move(move);
        }
        currentState.update();
        lastMove = move;
        if (parent != nullptr) {
            C = parent->C;
        }
    }
    Node(S &state, Node* parent) : Node(state, parent, M()) {
        lastMove.dummy();
    }
    Node(S &state, M &move) : Node(state, nullptr, move) { };
    Node(S &state) : Node(state, nullptr) {};
    Node() : Node(currentState, nullptr) {};

    ~Node() {
        children.clear();
        children.shrink_to_fit();
        possibleMoves.clear();
        possibleMoves.shrink_to_fit();
    }

    float ucb1() const {
        if (parent == nullptr) {
            return 0;  // Not relevant
        }

        if (total == 0) {
            return std::numeric_limits<float>::infinity();
        }

        auto root = parent;
        while (root->parent != nullptr && root->parent != root) {
            root = root->parent;
        }

        float result = (won / total) + parent->children.size() * std::sqrt(std::log(root->total) / total);
//        printf("(%.1f / %.1f) + %d * sqrt(log(%.1f) / %.1f) = %.2f\n", won, total, C, root->total, total, result);
        return result;
    }

    void addChild(M &move) {
        children.emplace_back(currentState, this, move);
    }

    Node<M, S> getChild(M &move) {
        long h = move.hash();
        for (auto child : children) {
            if (child.lastMove.hash() == h) {
                return child;
            }
        }

        addChild(move);

        return children.at(children.size()-1);
    }

    void doMCTS(int depth) {
        this->selection();

        if (children.size() == 1) {
            return;
        }

//        printf("Getting child with highest ucb1\n");
        Node<M, S> &child = children.at(children.size()-1);

        if (child.total == 0) {
//            printf("n=0, simulating\n");
            child.simulation(depth);
        } else {
//            printf("n>0, expanding\n");
            child.expansion();
            child.doMCTS(depth);
        }
    }

    M getBestMove() {
        std::sort(children.begin(), children.end(), &_sortBest<M,S>);
        auto &child = children.at(children.size() - 1);
        printf("Selected move from %d to %d (ucb1 %.6f)\n", ((ChessMove)child.lastMove).fromIndex, ((ChessMove)child.lastMove).toIndex, child.ucb1());
        return child.lastMove;
    }

    Node<M, S> deallocAndGet(M move) {
        Node<M, S> found = getChild(move);

        found.parent = nullptr;

        // clear siblings
        children.clear();
        children.shrink_to_fit();
        possibleMoves.clear();
        possibleMoves.shrink_to_fit();

        return found;
    }
};
