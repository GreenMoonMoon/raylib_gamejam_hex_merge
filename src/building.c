//
// Created by green on 2026-07-17.
//

#include "building.h"
#include "extern/stb_ds.h"

Blueprint create_blueprint() {
    const Blueprint bp = {
        .bpp_list = nullptr
    };
    return bp;
}

void delete_blueprint(Blueprint *blueprint) {
    if (blueprint->bpp_list != nullptr) {
        arrfree(blueprint->bpp_list);
    }
}
