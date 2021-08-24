//
// Created by mart on 8/20/21.
//

#include <cstdio>
#include <fstream>
#include "minmax_engine.h"
#include "../util/random.h"
#include "gperftools/malloc_extension.h"


ChessMove MinMaxEngine::process(ChessGameState &state) {
    printf("Process called\n");
    if (state.lastMove2.fromIndex == -1) {
        this->root.currentState.move(state.lastMove);
    } else {
        auto item = this->root.deallocAndGet(state.lastMove);
        this->root.~Node();
        // siblings cleared, we can override parent
        this->root = item;
        item.parent = nullptr;
    }

    printf("Looping k times...\n");
    for (int x = 0; x < k; x++) {
        this->root.doMCTS(depth);
    }

    ChessMove mv = this->root.getBestMove();
    printf("Move: %d,%d\n", mv.fromIndex, mv.toIndex);

//    printf("Updating internal board...\n");
//    this->root.currentState.move(mv);
//    this->root.currentState.update();
    auto item2 = this->root.deallocAndGet(mv);
    this->root.~Node();
    this->root = item2;
    item2.parent = nullptr;

//    printf("Done.\n");

//    std::string data;
//    MallocExtension::instance()->GetHeapSample(&data);
//    std::ofstream myfile;
//    myfile.open("heapz_dump");
//    myfile.write(data.c_str(), data.size());
//    myfile.close();

    return mv;
}

MinMaxEngine::MinMaxEngine(int depth, int k, int c) : depth(depth), k(k) {
    this->root.currentState.standard();
    this->root.C = c;
}
