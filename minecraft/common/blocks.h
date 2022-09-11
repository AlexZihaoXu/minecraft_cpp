//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_BLOCKS_H
#define MINECRAFT_BLOCKS_H

namespace minecraft::blocks {
    void registerBlocks() {
        Block::registerBlock("minecraft:stone", "stone")
                ->renderFaceDefsPush({
                                             DONT_CARE,
                                             "res/cobblestone.png",
                                             {{0, 0}, {0, 0, 0}},
                                             {{0, 1}, {0, 1, 0}},
                                             {{1, 1}, {1, 1, 0}}
                                     })
                ->renderFaceDefsPush({
                                             DONT_CARE,
                                             "res/cobblestone.png",
                                             {{0, 0}, {0, 0, 0}},
                                             {{1, 1}, {1, 1, 0}},
                                             {{1, 0}, {1, 0, 0}}
                                     });
    }
}

#endif //MINECRAFT_BLOCKS_H
