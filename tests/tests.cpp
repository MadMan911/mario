#include "doctest.h"
#include <iostream>
#include "../src/context.hpp"

context_t ctx;

#include "../src/Game.hpp"


TEST_CASE("Test load map function") {


    ctx.init();
    Map map(ctx.textures["tiles.png"]);
    map.load("assets/levels/0.txt");
    std::cout << map.at(-1,10);

    REQUIRE(map.at(-1, 10) != 0);
    REQUIRE(map.at(0, 0) == 0);
    REQUIRE(map.at(5, 4) != 7);
}

