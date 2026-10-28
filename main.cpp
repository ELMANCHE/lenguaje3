#include "GameEngine.h"
#include <iostream>

int main() {
    try {
        GameEngine engine(1280, 720);
        engine.init();
        engine.run();
        engine.shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
