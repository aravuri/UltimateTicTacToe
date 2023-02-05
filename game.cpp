#ifndef GAME_H
#define GAME_H

#include <bitset>
#include <iostream>
#include "grid.cpp"

struct TicTacToeGame {
    TicTacToeGrid grid;
    bool xTurn;
    uint16_t xWinners;
    uint16_t oWinners;
    std::bitset<81> validMoves;

    TicTacToeGame() {
        grid = TicTacToeGrid();
        xTurn = true;
        validMoves.set();
    }

    void makeMove(int* tindices) {
        int tileIndex = 27*tindices[0] + 9*tindices[1] + 3*tindices[2] + tindices[3];
        if (!validMoves[tileIndex])
            return;

        if (xTurn) {
            grid.x[tileIndex] = true;
        } else {
            grid.o[tileIndex] = true;
        }
        xTurn = !xTurn;

        xWinners = grid.winners(true);
        oWinners = grid.winners(false);

        validMoves = std::bitset<81>(0b111111111) << (9*(3*tindices[2] + tindices[3]));

        auto filledCells = grid.o | grid.x | expand9(xWinners | oWinners);
        validMoves &= ~filledCells;
        if (validMoves == 0) {
            validMoves.set();
            validMoves &= ~filledCells;
        }
        std::cout << validMoves << std::endl;
    }

    //Replaces an outer grid bitset of length 9 to an inner grid bitset of length 81
    //i.e., replaces 1 with 111111111 and 0 with 000000000
    std::bitset<81> expand9(uint16_t set) {
        std::bitset<81> ret;
        for (int i = 0; i < 9; i++) {
            if (set & (1 << i)) {
                ret |= std::bitset<81>(0b111111111) << 9*i;
            }
        }
        return ret;
    }
};

#endif