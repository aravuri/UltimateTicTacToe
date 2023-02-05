#ifndef GRID_H
#define GRID_H

#include <bitset>

const uint16_t WinPositions[] = {0b100100100, 0b010010010, 0b001001001, 0b111000000, 0b000111000, 0b000000111, 0b100010001, 0b001010100};

struct TicTacToeGrid {
    std::bitset<81> x;
    std::bitset<81> o;

    uint16_t winners(bool isX) {
        auto set = isX ? x : o;
        uint16_t ret = 0;
        for (int i = 0; i < 9; i++) {
            for (auto position : WinPositions) {
                auto test = std::bitset<81>(position) << i*9;
                if ((test & set) == test) {
                    ret |= (1 << i);
                    break;
                }
            }
        }
        return ret;
    }
    
};

#endif