//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_BLOCKS_H
#define MINECRAFT_BLOCKS_H

namespace minecraft::blocks {
    void registerBlocks() {
        Block::registerBlock("minecraft:stone", "stone")->renderFaceDefsPushQuadsAll("res/stone.png");
        Block::registerBlock("minecraft:cobblestone", "cobblestone")->renderFaceDefsPushQuadsAll("res/cobblestone.png");
        Block::registerBlock("minecraft:crafting_table", "crafting table")
                ->renderFaceDefsPushQuadNorth("res/crafting_table_front.png")
                ->renderFaceDefsPushQuadEast("res/crafting_table_side.png")
                ->renderFaceDefsPushQuadWest("res/crafting_table_side.png")
                ->renderFaceDefsPushQuadSouth("res/crafting_table_side.png")
                ->renderFaceDefsPushQuadTop("res/crafting_table_top.png")
                ->renderFaceDefsPushQuadBottom("res/planks_oak.png");
    }
}

#endif //MINECRAFT_BLOCKS_H
