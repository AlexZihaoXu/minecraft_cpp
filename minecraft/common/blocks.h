//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_BLOCKS_H
#define MINECRAFT_BLOCKS_H

namespace minecraft::blocks {
    class Blocks {
    private:
        inline static Blocks* instance;
        Blocks() {
        }
    public:
        static void registerBlocks() {
            if (!instance) {
                instance = new Blocks();
            }
        }

        static Blocks* get() {
            return instance;
        }

        int AIR = Block::registerBlock("minecraft:air", "air")->getID();

        int STONE = Block::registerBlock("minecraft:stone", "stone")->renderFaceDefsPushQuadsAll("res/textures/blocks/stone.png")->getID();

        int GRASS_BLOCK = Block::registerBlock("minecraft:grass_block", "grass_block")
                ->renderFaceDefsPushQuadTop("res/textures/blocks/grass_top.png")
                ->renderFaceDefsPushQuadBottom("res/textures/blocks/dirt.png")
                ->renderFaceDefsPushQuadsNSEW("res/textures/blocks/grass_side.png")
                -> getID();

        int DIRT = Block::registerBlock("minecraft:dirt", "dirt")->renderFaceDefsPushQuadsAll("res/textures/blocks/dirt.png")->getID();

        int COBBLESTONE = Block::registerBlock("minecraft:cobblestone", "cobblestone")->renderFaceDefsPushQuadsAll("res/textures/blocks/cobblestone.png")->getID();

        int OAK_PLANKS = Block::registerBlock("minecraft:oak_planks", "oak planks")->renderFaceDefsPushQuadsAll("res/textures/blocks/planks_oak.png")->getID();

        int OAK_SAPLING = Block::registerBlock("minecraft:oak_sapling", "oak sapling")->getID();

        int BEDROCK = Block::registerBlock("minecraft:bedrock", "bedrock")
                ->renderFaceDefsPushQuadsAll("res/textures/blocks/bedrock.png")
                ->getID();
    };

}

#endif //MINECRAFT_BLOCKS_H
