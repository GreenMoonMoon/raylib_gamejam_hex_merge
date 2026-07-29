//
// Created by green on 2026-07-17.
//

#include "building.h"

Blueprint create_blueprint() {
    const Blueprint bp = {0};
    return bp;
}

void delete_blueprint(Blueprint *blueprint) {
    delete_pipe_blueprint(&blueprint->pipes);
}
