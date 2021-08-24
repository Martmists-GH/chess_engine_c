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

template <MoveT M, StateT S>
class Node {
private:
    int player;
    Node* parent;
    M lastMove;
    std::vector<Node> children;
    std::vector<M> possibleMoves;

    int C;

    float won = 0.f;
    float total = 0.f;

    void generateMoves() {
        this->possibleMoves.reserve(20);
        currentState.getPossibleMoves(this->possibleMoves);
    }

    void selection() {
        if (this->possibleMoves.empty()) {
            this->expansion();
        }

        std::sort(children.begin(), children.end(), &_sort<M, S>);
    }

    void expansion() {
        if (this->possibleMoves.empty()) {
            generateMoves();
            this->children.reserve(10);
            for (auto mv : possibleMoves) {
                addChild(mv);
            }
        }
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
            generateMoves();
        }

        if (this->possibleMoves.empty()) {
            // no moves?? but also no winner or draw??
            printf("??????\n");
            return;
        }

        M move = randomItem(possibleMoves);
        Node<M, S> node = Node(currentState, this, move);
        node.simulation(depth - 1);
    }

    void backpropagation(int winner) {
        total += 1;
        if (winner == currentState.getMaxPlayers()) {
            won += 0.5f;
        } else if (winner == player) {
            won += 1;
        }

        if (parent != nullptr) {
            parent->backpropagation(winner);
        }
    }

public:
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
        while (root->parent != nullptr) {
            root = root->parent;
        }

        return (won / total) + C * std::sqrt(std::log(root->total) / total);
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
        Node<M, S> child = children.at(children.size()-1);

        if (child.total == 0) {
            child.simulation(depth);
        } else {
            child.expansion();
            child.doMCTS(depth - 1);
        }
    }

    M getBestMove() {
        std::sort(children.begin(), children.end(), &_sort<M,S>);
        return children.at(children.size() - 1).lastMove;
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
