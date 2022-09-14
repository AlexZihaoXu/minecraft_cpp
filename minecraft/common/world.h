//
// Created by Alex on 9/7/2022.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H

namespace minecraft::blocks {

    namespace render {

        struct MappingPos {
            glm::vec2 texCoords;
            glm::vec3 modelCoords;
        };

        struct ModelFaceRenderDefinition {
            int direction;
            std::string texturePath;
            MappingPos point1, point2, point3;
        };

    }

    class Block {
    private:
        inline static std::vector<Block> registeredBlocks;
        inline static std::map<std::string, int> blockNameMap;

        std::string name;
        std::string displayName;
        int blockID;
        std::vector<render::ModelFaceRenderDefinition> modelFaceRenderDefinition;

        Block(int blockID, const std::string &name, const std::string &displayName) {
            this->name = name;
            this->displayName = displayName;
            this->blockID = blockID;
        }

    public:
        static Block *registerBlock(const std::string &name, const std::string &displayName) {
            int id = (int) (registeredBlocks.size());
            registeredBlocks.push_back({id, name, displayName});
            blockNameMap[name] = id;
            return get(id);
        }

        static Block *get(int id)  {
            if (id >= registeredBlocks.size() || id < 0)
                return nullptr;
            return &registeredBlocks[id];
        }

        static Block *get(const std::string &name) {
            return get(blockNameMap[name]);
        }

        // Modifiers

        virtual Block *renderFaceDefsPush(const render::ModelFaceRenderDefinition &defs) {
            modelFaceRenderDefinition.push_back(defs);
            return this;
        }

        // Getters

        int getID() const {
            return blockID;
        }

        std::string getName() const {
            return name;
        }

        std::string getDisplayName() const {
            return displayName;
        }

        const std::vector<render::ModelFaceRenderDefinition> getModelFaceRenderDefinition() const {
            return modelFaceRenderDefinition;
        }

        // Setters

        Block *setName(const std::string &new_name) {
            this->name = new_name;
            return this;
        }

        Block *setDisplayName(const std::string &newDisplayName) {
            this->displayName = newDisplayName;
            return this;
        }

    };
}

#endif //MINECRAFT_WORLD_H
