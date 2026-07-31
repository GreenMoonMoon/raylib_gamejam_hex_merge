//
// Created by green on 2026-07-31.
//
#include "tst.h"
#include "../src/pipe_rules.h"

// Source - https://stackoverflow.com/a/3208376
// Posted by William Whyte, modified by community. See post 'Timeline' for change history
// Retrieved 2026-07-31, License - CC BY-SA 4.0
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

char test_branch_pipe = IN_A | IN_B | IN_D; // 0b00001011
char test_input_tile = IN_D | IN_E | IN_A; // 0b00011001

tstsuite("pipe rules tdd") {
    tstcase("rotate inputs right") {
        char result = rotate_inputs_right(test_branch_pipe);
        tstcheck(result == 0b00100101, "rotation don't work");
        result = rotate_inputs_right(result);
        tstcheck(result == 0b00110010, "rotation don't work");
        result = rotate_inputs_right(result);
        tstcheck(result == 0b00011001, "rotation don't work");
        result = rotate_inputs_right(result);
        tstcheck(result == 0b00101100, "rotation don't work");
    }
    tstcase("rotate inputs left") {
        char result = rotate_inputs_left(test_branch_pipe);
        tstcheck(result == 0b00010110, "rotation don't work");
        result = rotate_inputs_left(result);
        tstcheck(result == 0b00101100, "rotation don't work");
        result = rotate_inputs_left(result);
        tstcheck(result == 0b00011001, "rotation don't work");
        result = rotate_inputs_left(result);
        tstcheck(result == 0b00110010, "rotation don't work");
    }
}