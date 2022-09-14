#include "minecraft/client/client.h"

int main() {

    minecraft::client::MinecraftClientWindow::launch();
    minecraft::client::MinecraftClientWindow::cleanup();

    return 0;
}
