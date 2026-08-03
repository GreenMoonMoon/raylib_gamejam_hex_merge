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

const char pipe_type[] = {
    0b00001011, // branch split
    0b00001001, // straight
    0b00011011, // x split
};

struct PipeRule {
    char key;
    struct PT {
        int id;
        char rotation;
    } value ;
};

void generate_pipe_rule_table(void **table, int pipe_types, int pipe_type_count) {
    for (int i = 1; i < pipe_type_count; ++i) {

    }
}