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

// void generate_pipe_rule_table(void **table);

char rotate_inputs_right(char inputs);

char rotate_inputs_left(char inputs);

#endif //RAYLIB_GAMEJAM_ENTRY_PIPE_RULES_H
