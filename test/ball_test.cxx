#include "ball.hxx"
#include <catch.hxx>

Block const The_Paddle {100, 400, 100, 20};

TEST_CASE("Ball::Ball")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    CHECK(ball.center.x == 150);
    CHECK(ball.center.y == 394);

}

TEST_CASE("Ball::top_left")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    //test top_left()
    CHECK(ball.top_left().x == 145);
    CHECK(ball.top_left().y == 389);
}

TEST_CASE("Ball::hits_side")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    CHECK_FALSE(ball.hits_side(config));
    ball.center.x = 1;
    CHECK(ball.hits_side(config));
    ball.center.x = config.scene_dims.width - 1;
    CHECK(ball.hits_side(config));
    ball.center.x = config.scene_dims.width + 1;
    CHECK(ball.hits_side(config));
    ball.center.x = - 1;
    CHECK(ball.hits_side(config));
    ball.center.x = config.scene_dims.width / 2;
    CHECK_FALSE(ball.hits_side(config));
}

TEST_CASE("Ball::hits_bottom")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    CHECK_FALSE(ball.hits_bottom(config));
    ball.center.y = config.scene_dims.height - 1;
    CHECK(ball.hits_bottom(config));
    ball.center.y = config.scene_dims.height + 1;
    CHECK(ball.hits_bottom(config));
    ball.center.y = config.scene_dims.height / 2;
    CHECK_FALSE(ball.hits_bottom(config));

}

TEST_CASE("Ball::hits_top")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    CHECK_FALSE(ball.hits_top(config));
    ball.center.y = 1;
    CHECK(ball.hits_top(config));
    ball.center.y = -1;
    CHECK(ball.hits_top(config));
    ball.center.y = config.scene_dims.height / 2;
    CHECK_FALSE(ball.hits_top(config));

}

TEST_CASE("Ball::hits_top && hits_side")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    CHECK_FALSE((ball.hits_top(config) && ball.hits_side(config)));
    ball.center.y = 1;
    ball.center.x = 1;
    CHECK((ball.hits_top(config) && ball.hits_side(config)));

    ball.center.x = config.scene_dims.width - 1;
    CHECK((ball.hits_top(config) && ball.hits_side(config)));

    ball.center.y = 23;
    CHECK_FALSE((ball.hits_top(config) && ball.hits_side(config)));

}

TEST_CASE("Ball::next")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    //time 0
    CHECK(ball.next(0).center.x == 150);
    CHECK(ball.next(0).center.y == 394);

    //different time not off screen
    CHECK(ball.next(0.5).center.x == 225);
    CHECK(ball.next(0.5).center.y == 94);

    //different time off screen (y)
    CHECK(ball.next(1).center.x == 300);
    CHECK(ball.next(1).center.y == -206);

    //different time off screen (x)
    ball.center.x = 1000;
    CHECK(ball.next(0.5).center.x == 1075);
    CHECK(ball.next(0.5).center.y == 94);

    //different time off screen both x and y
    CHECK(ball.next(1).center.x == 1150);
    CHECK(ball.next(1).center.y == -206);

}

TEST_CASE("Ball::hits_block")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    Block b(100, 100, 100, 50);

    // doesn't hit block
    CHECK_FALSE(ball.hits_block(b));

    //case 1 hit
    ball.center.x = 95;
    ball.center.y = 150;
    CHECK(ball.hits_block(b));
    ball.center.x = 94;
    CHECK_FALSE(ball.hits_block(b));

    //case 2 hit
    ball.center.x = 105;
    CHECK(ball.hits_block(b));
    ball.center.x = 210;
    CHECK_FALSE(ball.hits_block(b));

    //case 3 hit
    ball.center.x = 150;
    ball.center.y = 95;
    CHECK(ball.hits_block(b));
    ball.center.y = 90;
    CHECK_FALSE(ball.hits_block(b));


    //case 4 hit
    ball.center.x = 150;
    ball.center.y = 155;
    CHECK(ball.hits_block(b));
    ball.center.y = 160;
    CHECK_FALSE(ball.hits_block(b));

}

TEST_CASE("Ball::destroy_brick")
{
    Game_config config;
    Ball ball(The_Paddle, config);

    Block b1(100, 100, 100, 50);
    Block b2(100, 200, 100, 50);
    Block b3(250, 100, 100, 50);


    std::vector<Block> bricks = {b1, b2, b3};

    ball.center.x = 95;
    ball.center.y = 125;

    // brick b1 was hit
    ball.destroy_brick(bricks);

    // check b1 was replaced by b3
    CHECK(bricks[0].x == 250);

    // check back popped off
    CHECK(bricks.size() == 2);


    // brick b2 was hit (hit bricks is the last brick)
    ball.center.x = 100;
    ball.center.y = 200;
    ball.destroy_brick(bricks);

    // check back popped off (b2)
    CHECK(bricks[0].x == 250);
    CHECK(bricks[0].y == 100);
    CHECK(bricks.size() == 1);

}

TEST_CASE("Ball::operator==")
{
    Game_config config;

    Ball ball1(The_Paddle, config);
    Ball ball2(The_Paddle, config);
    CHECK(ball1 == ball2);

    ball2.radius = 10;
    CHECK_FALSE(ball1 == ball2);

    ball2. radius = 5;
    ball2.live = true;
    CHECK_FALSE(ball1 == ball2);

    ball2.live = false;
    CHECK(ball1 == ball2);

    ball2.center.x = 400;
    CHECK_FALSE(ball1 == ball2);

    ball2.center.x = 150;
    CHECK(ball1 == ball2);

    ball2.velocity.width = 300;
    CHECK_FALSE(ball1 == ball2);

}


