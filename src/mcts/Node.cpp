

#include "Node.h"
#include "../engine/board_evaluation.h"

bool _sort(Node* a, Node* b) {
    return a->ucb1() > b->ucb1();
}

bool _sortEval(Node* a, Node* b) {
    return evaluate(&a->currentState) > evaluate(&b->currentState);
}

bool _sortBest(Node* a, Node* b) {
    return a->total > b->total;
}

Node::Node(ChessGameState *state, Node *parent, ChessMove mv) {
    this->parent = parent;
    memcpy(&this->currentState, state, sizeof(ChessGameState));
    player2 = currentState.blackToMove;
    lastMove = mv;
    if (mv.fromIndex != -1) {
        move(&currentState, mv);
    }
    update(&currentState);
    if (parent != nullptr) {
        C = parent->C;
    }
}

Node::Node(ChessGameState* state, Node* parent) : Node::Node(state, parent, DUMMY_MOVE) { }
Node::Node(ChessGameState* state, ChessMove move) : Node::Node(state, nullptr, move) { };
Node::Node(ChessGameState* state) : Node::Node(state, nullptr) {};

Node::~Node() {
    children.clear();
    children.shrink_to_fit();
    possibleMoves.clear();
    possibleMoves.shrink_to_fit();
}

void Node::generateMoves() {
    this->possibleMoves.reserve(20);
    getPossibleMoves(&currentState, this->possibleMoves);
}

void Node::selection() {
    if (this->children.empty()) {
        this->expansion();
    }

    if (children.size() > 1) {
//        printf("Sorting children by ucb1\n");
        std::sort(children.begin(), children.end(), &_sort);
    }
}

void Node::expansion() {
    if (this->possibleMoves.empty()) {
        generateMoves();
    }

    if (this->children.empty()) {
        this->children.reserve(50);  // Good enough for most cases
        for (auto &mv : possibleMoves) {
//            printf("Move: (%d, %d)\n", mv.fromIndex, mv.toIndex);
            addChild(mv);
        }
        std::sort(children.begin(),  children.end(), &_sortEval);
    }

//    printf("Generated %d moves with %d children\n", possibleMoves.size(), children.size());
}

void Node::simulation(int depth) {
    if (getWinner(&currentState) != -1) {
        backpropagation(getWinner(&currentState));
        return;
    }

    if (depth == 0) {
        backpropagation(getWinningPlayer(&currentState));
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

    ChessMove move = randomItem(possibleMoves);
    Node n(&currentState, this, move);
    n.simulation(depth - 1);
}

void Node::backpropagation(int winner) {
//    printf("Winner: %d, backpropagating\n", winner);

    total += 1;
    if (winner == 2) {
        won += 0.5f;
    } else if (winner == player2) {
        won += 1;
    }

//        printf("t = %.1f, n = %.1f\n", won, total);

    if (parent != nullptr) {
        parent->backpropagation(winner);
    }
}

float Node::ucb1() const {
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

void Node::addChild(ChessMove move) {
    children.push_back(new Node(&currentState, this, move));
}

Node* Node::getChild(ChessMove move) {
    for (auto child : children) {
        if (memcmp(&child->lastMove, &move, sizeof(ChessMove)) == 0) {
            return child;
        }
    }

    addChild(move);

    return children.at(0);
}

void Node::doMCTS(int depth) {
    this->selection();

    if (children.size() == 0 || depth == 0) {
        // mate?
        simulation(0);
        return;
    }

    if (children.size() == 1) {
        return;
    }

//        printf("Getting child with highest ucb1\n");
    Node* child = children.at(0);

    if (child->total == 0) {
//            printf("n=0, simulating\n");
        child->simulation(depth);
    } else {
//            printf("n>0, expanding\n");
        child->expansion();
        child->doMCTS(depth);
    }
}

ChessMove Node::getBestMove() {
    std::sort(children.begin(), children.end(), &_sortBest);
    Node* child = children.at(0);
//    printf("Selected move from %d to %d (ucb1 %.6f)\n", child->lastMove.fromIndex, child->lastMove.toIndex, child->ucb1());
    return child->lastMove;
}

void Node::clearChildren(Node *except) {
    for (auto c : children) {
        if (c != except) {
            delete c;
        }
    }
}
