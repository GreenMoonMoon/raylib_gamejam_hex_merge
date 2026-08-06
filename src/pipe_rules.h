//
// Created by green on 2026-07-30.
//

#ifndef RAYLIB_GAMEJAM_ENTRY_PIPE_RULES_H
#define RAYLIB_GAMEJAM_ENTRY_PIPE_RULES_H

#define IN_A 0x1    // north
#define IN_B 0x2    // north east
#define IN_C 0x4    // south east
#define IN_D 0x8    // south
#define IN_E 0x10   // south west
#define IN_F 0x20   // north west
#define IN_MASK 0x3F
#include "pipes.h"

struct PipeEntry {
    int id;
    char rotation;
};

const struct PipeEntry pipe_ruleset[] = {
    {.id = PIPE_NONE, .rotation = 0},         // 000000
    {.id = PIPE_SHORT_END, .rotation = 0},    // 000001
    {.id = PIPE_SHORT_END, .rotation = 1},    // 000010
    {.id = PIPE_NONE, .rotation = 0},         // 000011
    {.id = PIPE_SHORT_END, .rotation = 2},    // 000100
    {.id = PIPE_BEND, .rotation = 3},         // 000101
    {.id = PIPE_NONE, .rotation = 0}, // 000110
    {.id = PIPE_NONE, .rotation = 0}, // 000111
    {.id = PIPE_SHORT_END, .rotation = 3}, // 001000
    {.id = PIPE_STRAIGHT, .rotation = 0}, //001001
    {.id = PIPE_BEND, .rotation = 4}, // 001010
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 3}, // 001011
    {.id = PIPE_NONE, .rotation = 0}, // 001100
    {.id = PIPE_SPLIT_BRANCH, .rotation = 0}, // 001101
    {.id = PIPE_NONE, .rotation = 0}, // 001110
    {.id = PIPE_NONE, .rotation = 0}, // 001111
    {.id = PIPE_SHORT_END, .rotation = 4}, // 010000
    {.id = PIPE_BEND, .rotation = 1}, // 010001
    {.id = PIPE_STRAIGHT, .rotation = 1}, // 010010
    {.id = PIPE_SPLIT_BRANCH, .rotation = 4}, // 010011
    {.id = PIPE_BEND, .rotation = 5}, // 010100
    {.id = PIPE_NONE, .rotation = 0}, // 010101
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 4}, // 010110
    {.id = PIPE_NONE, .rotation = 0}, // 010111
    {.id = PIPE_NONE, .rotation = 0}, // 011000
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 0}, // 011001
    {.id = PIPE_SPLIT_BRANCH, .rotation = 1}, // 011010
    {.id = PIPE_NONE, .rotation = 0}, // 011011
    {.id = PIPE_NONE, .rotation = 0}, // 011100
    {.id = PIPE_NONE, .rotation = 0}, // 011101
    {.id = PIPE_NONE, .rotation = 0}, // 011110
    {.id = PIPE_NONE, .rotation = 0}, // 011111
    {.id = PIPE_SHORT_END, .rotation = 5}, // 100000
    {.id = PIPE_NONE, .rotation = 0}, // 100001
    {.id = PIPE_BEND, .rotation = 2}, // 100010
    {.id = PIPE_NONE, .rotation = 0}, // 100011
    {.id = PIPE_STRAIGHT, .rotation = 2}, // 100100
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 2}, // 100101
    {.id = PIPE_SPLIT_BRANCH, .rotation = 5}, // 100110
    {.id = PIPE_NONE, .rotation = 0}, // 100111
    {.id = PIPE_BEND, .rotation = 0}, // 101000
    {.id = PIPE_SPLIT_BRANCH, .rotation = 3}, // 101001
    {.id = PIPE_NONE, .rotation = 0}, // 101010
    {.id = PIPE_NONE, .rotation = 0}, // 101011
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 5}, // 101100
    {.id = PIPE_NONE, .rotation = 0}, // 101101
    {.id = PIPE_NONE, .rotation = 0}, // 101110
    {.id = PIPE_NONE, .rotation = 0}, // 101111
    {.id = PIPE_NONE, .rotation = 0}, // 110000
    {.id = PIPE_NONE, .rotation = 0}, // 110001
    {.id = PIPE_SPLIT_BRANCH_FLIPPED, .rotation = 1}, // 110010
    {.id = PIPE_NONE, .rotation = 0}, // 110011
    {.id = PIPE_SPLIT_BRANCH, .rotation = 2}, // 110100
    {.id = PIPE_NONE, .rotation = 0}, // 110101
    {.id = PIPE_NONE, .rotation = 0}, // 110110
    {.id = PIPE_NONE, .rotation = 0}, // 110111
    {.id = PIPE_NONE, .rotation = 0}, // 111000
    {.id = PIPE_NONE, .rotation = 0}, // 111001
    {.id = PIPE_NONE, .rotation = 0}, // 111010
    {.id = PIPE_NONE, .rotation = 0}, // 111011
    {.id = PIPE_NONE, .rotation = 0}, // 111100
    {.id = PIPE_NONE, .rotation = 0}, // 111101
    {.id = PIPE_NONE, .rotation = 0}, // 111110
    {.id = PIPE_NONE, .rotation = 0}, // 111111
};

inline char rotate_inputs_right(const char inputs) {
    return inputs & IN_A ? inputs >> 1 | IN_F : inputs >> 1;
}

inline char rotate_inputs_left(const char inputs) {
    return inputs & IN_F ? inputs << 1 & IN_MASK | IN_A : inputs << 1 & IN_MASK;
}

#endif //RAYLIB_GAMEJAM_ENTRY_PIPE_RULES_H
