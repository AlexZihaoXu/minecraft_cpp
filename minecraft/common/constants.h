//
// Created by Alex on 9/7/2022.
//

#ifndef MINECRAFT_CONSTANTS_H
#define MINECRAFT_CONSTANTS_H

#include <string>
#include <glm.hpp>

namespace minecraft {
    const int DONT_CARE = -1;
    const int FORWARD = 0;
    const int RIGHT = 1;
    const int BACKWARD = 2;
    const int LEFT = 3;
    const int UP = 4;
    const int DOWN = 5;
    const int NORTH = FORWARD;
    const int EAST = RIGHT;
    const int SOUTH = BACKWARD;
    const int WEST = LEFT;

    const glm::vec3 direction2Vec[] = {
            {0.0, 0.0, -1.0},
            {+1.0, 0.0, 0.0},
            {0.0, 0.0, +1.0},
            {-1.0, 0.0, 0.0},
            {0.0, +1.0, 0.0},
            {0.0, -1.0, 0.0},
    };

}

#endif //MINECRAFT_CONSTANTS_H
