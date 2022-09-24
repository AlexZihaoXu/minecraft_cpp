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

        struct ModelQuadFaceRenderDefinition {
            int direction;
            std::string texturePath;
            MappingPos topleft, topright, bottomleft, bottomright;
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

        static Block *get(int id) {
            if (id >= registeredBlocks.size() || id < 0)
                return nullptr;
            return &registeredBlocks[id];
        }

        static Block *get(const std::string &name) {
            return get(blockNameMap[name]);
        }

        // Model Editing

        Block *renderFaceDefsPush(const render::ModelFaceRenderDefinition &defs) {
            modelFaceRenderDefinition.push_back(defs);
            return this;
        }

        Block *renderFaceDefsPushQuad(const render::ModelQuadFaceRenderDefinition &defs) {
            renderFaceDefsPush({
                                       defs.direction,
                                       defs.texturePath,
                                       defs.bottomleft,
                                       defs.topleft,
                                       defs.topright
                               });
            renderFaceDefsPush({
                                       defs.direction,
                                       defs.texturePath,
                                       defs.bottomleft,
                                       defs.topright,
                                       defs.bottomright
                               });
            return this;
        }


        Block *renderFaceDefsPushQuadNorth(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  NORTH, path,
                                                  {{0, 1}, {1, 1, -1}},
                                                  {{1, 1}, {0, 1, -1}},
                                                  {{0, 0}, {1, 0, -1}},
                                                  {{1, 0}, {0, 0, -1}},
                                          });
        }


        Block *renderFaceDefsPushQuadSouth(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  SOUTH, path,
                                                  {{0, 1}, {0, 1, 0}},
                                                  {{1, 1}, {1, 1, 0}},
                                                  {{0, 0}, {0, 0, 0}},
                                                  {{1, 0}, {1, 0, 0}},
                                          });
        }

        Block *renderFaceDefsPushQuadWest(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  WEST, path,
                                                  {{0, 1}, {0, 1, -1}},
                                                  {{1, 1}, {0, 1, 0}},
                                                  {{0, 0}, {0, 0, -1}},
                                                  {{1, 0}, {0, 0, 0}},
                                          });
        }

        Block *renderFaceDefsPushQuadEast(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  EAST, path,
                                                  {{0, 1}, {1, 1, 0}},
                                                  {{1, 1}, {1, 1, -1}},
                                                  {{0, 0}, {1, 0, 0}},
                                                  {{1, 0}, {1, 0, -1}},
                                          });
        }

        Block *renderFaceDefsPushQuadTop(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  UP, path,
                                                  {{0, 1}, {0, 1, -1}},
                                                  {{1, 1}, {1, 1, -1}},
                                                  {{0, 0}, {0, 1, 0}},
                                                  {{1, 0}, {1, 1, 0}},
                                          });
        }


        Block *renderFaceDefsPushQuadBottom(const std::string &path) {
            return renderFaceDefsPushQuad({
                                                  DOWN, path,
                                                  {{0, 1}, {1, 0, -1}},
                                                  {{1, 1}, {0, 0, -1}},
                                                  {{0, 0}, {1, 0, 0}},
                                                  {{1, 0}, {0, 0, 0}},
                                          });
        }

        Block *renderFaceDefsPushQuadsNSEW(const std::string &path) {
            renderFaceDefsPushQuadEast(path);
            renderFaceDefsPushQuadWest(path);
            renderFaceDefsPushQuadNorth(path);
            renderFaceDefsPushQuadSouth(path);
            return this;
        }

        Block *renderFaceDefsPushQuadsAll(const std::string &path) {
            renderFaceDefsPushQuadsNSEW(path);
            renderFaceDefsPushQuadTop(path);
            renderFaceDefsPushQuadBottom(path);
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

    struct BlockState {
        int blockID;
        int direction;

        Block *getBlock() const {
            return Block::get(blockID);
        }
    };

    class Chunk;

    class ChunkSection {
    private:
        std::vector<BlockState> blockStates;
        unsigned long changeID = 0;
        Chunk *chunk;
        int sectionY;

        ChunkSection *getSectionFromOwnChunk(int y);

    public:

        ChunkSection(int sectionY, Chunk *chunk) {
            this->chunk = chunk;
            this->sectionY = sectionY;
            for (int i = 0; i < 16 * 16 * 16; ++i) {
                blockStates.push_back({0, DONT_CARE});
            }
        }

        int getSectionY() {
            return sectionY;
        }

        Chunk *getChunk() {
            return chunk;
        }

        unsigned long getChangeID() const {
            return changeID;
        }

        void setState(const BlockState &state, int x, int y, int z) {
            BlockState &target = blockStates[x * 16 * 16 + y * 16 + z];
            if (!(state.blockID == target.blockID && state.direction == target.direction)) {
                blockStates[x * 16 * 16 + y * 16 + z] = state;
                changeID++;
            }
        }

        void setBlock(int block, int x, int y, int z) {
            setState({block, NORTH}, x, y, z);
        }

        BlockState *getState(int x, int y, int z) {
            if (0 <= x && x < 16 && 0 <= y && y < 16 && 0 <= z && z < 16)
                return &blockStates[x * 16 * 16 + y * 16 + z];
            else {

            }
        }

        Block *getBlock(int x, int y, int z) {
            return getState(x, y, z)->getBlock();
        }
    };

    class Chunk {
    private:
        std::vector<ChunkSection *> sections;
        int chunkX, chunkY;
    public:

        int getChunkX() const {
            return chunkX;
        }

        int getChunkY() const {
            return chunkY;
        }

        Chunk(int x, int y) {
            chunkX = x;
            chunkY = y;
            for (int i = 0; i < 16; ++i) {
                sections.push_back(nullptr);
            }
        }

        ~Chunk() {
            for (int i = 0; i < 16; ++i) {
                if (hasSection(i)) {
                    delete sections[i];
                }
            }
        }

        bool hasSection(int sectionY) const {
            return sections[sectionY] != nullptr;
        }

        ChunkSection *getSection(int sectionY) {
            if (!hasSection(sectionY)) {
                sections[sectionY] = new ChunkSection(sectionY, this);
            }
            return sections[sectionY];
        }

        BlockState *getState(int x, int y, int z) {
            if (y < 0 || y >= 256 || x < 0 || x >= 16 || z < 0 || z >= 16) {
                return nullptr;
            }
            return getSection(y / 16)->getState(x, y % 16, z);
        }

        void setState(const BlockState &state, int x, int y, int z) {
            getSection(y / 16)->setState(state, x, y % 16, z);
        }

        Block *getBlock(int x, int y, int z) {
            return getState(x, y, z)->getBlock();
        }

        void setBlock(int block, int x, int y, int z) {
            setState({block, NORTH}, x, y, z);
        }

    };

    ChunkSection *ChunkSection::getSectionFromOwnChunk(int y) {
        if (getChunk()->hasSection(y)) {
            return getChunk()->getSection(y);
        }
        return nullptr;
    }

    class World {
    private:
        std::map<unsigned long long, Chunk *> chunksMap;
        std::unordered_set<Chunk *> chunksSet;

    public:

        World() {

        }

        ~World() {
            for (const auto &item: chunksMap) {
                delete item.second;
            }
        }

        bool hasChunk(int x, int y) const {
            return chunksMap.contains((x + 0x80000000) + (y + 0x80000000) * 0x100000000);
        }

        Chunk *getChunk(int x, int y) {
            unsigned long long index = (x + 0x80000000) + (y + 0x80000000) * 0x100000000;
            if (!chunksMap.contains(index)) {
                chunksMap[index] = new Chunk(x, y);
                chunksSet.insert(chunksMap[index]);
            }
            return chunksMap[index];
        }

        const std::unordered_set<Chunk *>& getChunksSet() const {
            return chunksSet;
        }

        BlockState *getState(float x, float y, float z) {
            return getChunk(glm::floor(x / 16.0f), glm::floor(z / 16.0f))->getState(math::pmod(x, 16), y,
                                                                                    math::pmod(z, 16));
        }

        BlockState *getState(int x, int y, int z) {
            return getState((float) x, (float) y, (float) z);
        }

        void setState(const BlockState &state, float x, float y, float z) {
            getChunk(glm::floor(x / 16.0f), glm::floor(z / 16.0f))->setState(state, math::pmod(x, 16), y,
                                                                             math::pmod(z, 16));
        }

        void setState(const BlockState &state, int x, int y, int z) {
            setState(state, (float) x, (float) y, (float) z);
        }

        Block *getBlock(int x, int y, int z) {
            return getState(x, y, z)->getBlock();
        }

        void setBlock(int block, int x, int y, int z) {
            setState({block, NORTH}, x, y, z);
        }

    };
}

#endif //MINECRAFT_WORLD_H
