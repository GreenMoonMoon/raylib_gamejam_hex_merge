//
// Created by green on 2026-07-30.
//

#include "pipe_rules.h"

char rotate_inputs_right(const char inputs) {
    if (inputs & IN_A) {
        return inputs >> 1 | IN_F;
    }
    return inputs >> 1;
}

char rotate_inputs_left(const char inputs) {
    if (inputs & IN_F) {
        return inputs << 1 & 0b00111111 | IN_A;
    }
    return inputs << 1 & 0b00111111;
}

// const int pipe_type[] = {
//
// };

// void generate_pipe_rule_table(void **table, int pipe_types, int pipe_type_count) {
//     for (int i = 0; i < pipe_type_count; ++i) {
//
//     }
// }