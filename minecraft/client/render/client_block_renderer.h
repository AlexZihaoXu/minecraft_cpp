//
// Created by Alex on 9/11/2022.
//

#ifndef MINECRAFT_CLIENT_BLOCK_RENDERER_H
#define MINECRAFT_CLIENT_BLOCK_RENDERER_H

namespace minecraft::client::render {
    class BlockRenderer {
    private:
        inline static bool initialized = false;
        inline static engine::Framebuffer *textureAtlasBlocks;
    public:

        static void initialize() {
            if (initialized)
                return;
            console.info("Initializing block renderer ...");
            std::vector<std::string> texPaths;
            std::set<std::string> texPathsSet;

            for (int i = 0; blocks::Block::get(i) != nullptr; ++i) {
                var block = blocks::Block::get(i);
                console.info("Renderer", "Loading block: " + block->getName());
                for (var def: block->getModelFaceRenderDefinition()) {
                    if (texPathsSet.contains(def.texturePath))
                        continue;
                    texPathsSet.insert(def.texturePath);
                    texPaths.push_back(def.texturePath);
                }
            }

            int totalWidth = 0;
            int totalHeight = 0;
            for (var path: texPaths) {
                var texture = engine::Texture::get(path);
                totalWidth += texture->width();
                totalHeight = std::max(totalHeight, texture->height());
            }

            textureAtlasBlocks = new engine::Framebuffer(totalWidth, totalHeight);
            console.info(
                    "Renderer",
                    "Created block texture atlas (" + std::to_string(totalWidth) + "x" + std::to_string(totalHeight) +
                    ")");

            int xOffset = 0;

            initialized = true;
        }

        static void renderBlock(const blocks::Block *&block, const glm::mat4 &trans) {
            initialize();


        }
    };
}

#endif //MINECRAFT_CLIENT_BLOCK_RENDERER_H
