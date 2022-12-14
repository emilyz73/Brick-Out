#include "model.hxx"
#include <catch.hxx>

// We'll use this same game config throughout.
Game_config const config;

TEST_CASE("ball moving freely")
{
    Model m(config);

    // Get rid of all the bricks and make the ball move slowly upward so
    // we don't hit anything:
    m.bricks.clear();
    m.ball.velocity = {8, -24};
    m.ball.live = true;

    // Make a copy of the ball's position, which we'll use to compute
    // where we expect the ball to go.
    Position expected_center = m.ball.center;

    // If the model advances by 0.5 s then we expect the center to move
    // accordingly:
    m.on_frame(0.5);
    expected_center += 0.5 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);

    // Again, but only 0.25 s this time.
    m.on_frame(0.25);
    expected_center += 0.25 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);

    // Again, but 0.125 s this time.
    m.on_frame(0.125);
    expected_center += 0.125 * m.ball.velocity;
    CHECK(m.ball.center == expected_center);
}

TEST_CASE("destroy one brick")
{
    Model m(config);

    // Let's run at 1 fps.
    double const dt = 1;

    // We're going to stub the model's random number generator so it
    // always returns 14:
    float const boost = 14;

    // Stub it:
    m.random_boost_source.stub_with(boost);

    // Make there be only one brick:
    m.bricks.clear();
    m.bricks.push_back({250, 200, 100, 20});

    // The ball is headed toward the brick and should hit it in
    // the 4th frame if we simulate 1 frame per second:
    m.ball.live = true;
    m.ball.center = {300, 400};
    m.ball.velocity = {0, -50};

    // Make a copy of the ball, which we'll use to computer what we
    // expect the ball to do:
    Ball expected_ball(m.ball);

    // Simulate the passage of 1 frame (1 s):
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame:
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame:
    m.on_frame(dt);
    expected_ball = expected_ball.next(dt);
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.size() == 1);

    // Simulate the passage of another frame, in which the ball
    // destroys a brick. The model should reflect and boost the ball
    // before moving it, so we need to do the same with our expected
    // ball.
    m.on_frame(dt);
    expected_ball.velocity.height *= -1;
    expected_ball.velocity.width += boost;
    expected_ball = expected_ball.next(dt);  //boost added checked here
    CHECK(m.ball == expected_ball);
    CHECK(m.bricks.empty());
}

TEST_CASE("bounce off paddle")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    // Let's run at 8 fps.
    double const dt = 0.125;

    // The ball is headed toward the paddle at 50 px/s from 75 px away,
    // so it should it it in 1.5 s. If we simulate 8 fps then it should
    // hit in the 12th frame.
    m.paddle = {50, 400, 600, 50};
    m.ball.live = true;
    m.ball.velocity = {0, 50};
    m.ball.center.x = 300;
    m.ball.center.y = m.paddle.y - 75 - m.ball.radius / 2;

    // Make a copy of the ball for computing what we expect it to do.
    Ball expected_ball(m.ball);

    // Local helper function (lambda) that runs both the expected ball
    // and the model for a frame and then checks that they agree.
    auto check_frame = [&] {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    };

    // Simulate 11 frames (1.375 s).
    for (int i = 0; i < 11; ++i) {
        check_frame();
    }

    // Simulate one frame in which the ball finally hits the paddle,
    // which means that it reflects vertically.
    expected_ball.velocity.height *= -1;
    check_frame();

    // Simulate 8 more frames (1 s), now with the ball headed away from
    // the paddle.
    for (int i = 0; i < 8; ++i) {
        check_frame();
    }
}

TEST_CASE("test paddle_to")
{
    Model m(config);
    m.paddle = {50, 400, 600, 50};
    m.ball.center.x = 50 + 0.5 * m.paddle.width;
    m.ball.center.y = 400 - m.ball.radius;

    //ball is dead
    m.paddle_to(70);
    CHECK(m.paddle.x == 70);
    CHECK(m.ball.center.x == 70 + 0.5 * m.paddle.width);

    //ball is live
    m.ball.live = true;
    m.paddle_to(123);
    CHECK(m.paddle.x == 123);
    CHECK_FALSE(m.ball.center.x == 123 + 0.5*m.paddle.width);
    CHECK(m.ball.center.x == 70 + 0.5*m.paddle.width); //original location -
    // doesn't follow paddle

}

TEST_CASE("hit bottom")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    double const dt = 1;

    m.paddle = {10, 400, 600, 50};
    m.ball.live = true;

    //scene_dims {1024, 768},
    m.ball.velocity = {0, 2};
    m.ball.center.x = 300;
    m.ball.center.y = 767;

    // Make a copy of the ball for computing what we expect it to do.
    Ball expected_ball(m.ball);


    //ball hits bottom of screen
    expected_ball.live = false;
    expected_ball.center.x = m.paddle.x + 0.5 * m.paddle.width;
    expected_ball.center.y = m.paddle.y - expected_ball.radius - 1;
    expected_ball.velocity.width = config.ball_velocity_0.width;
    expected_ball.velocity.height = config.ball_velocity_0.height;


     m.on_frame(dt);
     CHECK(m.ball == expected_ball);
}

TEST_CASE("hit corner")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    double const dt = 1;

    m.paddle = {10, 400, 600, 50};
    m.ball.live = true;

    m.ball.velocity = {50, -50};
    // scene_dims {1024, 768},
    m.ball.center.x = 1022;
    m.ball.center.y = 2;


    Ball expected_ball(m.ball);

    auto check_frame = [&] {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    };

    //ball hits corner
    expected_ball.velocity.width = -50;
    expected_ball.velocity.height = 50;
    check_frame();

}

TEST_CASE("hit top")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    double const dt = 1;

    m.paddle = {10, 400, 600, 50};
    m.ball.live = true;

    m.ball.velocity = {0, -50};
    // scene_dims {1024, 768},
    m.ball.center.x = 500;
    m.ball.center.y = 2;


    Ball expected_ball(m.ball);

    auto check_frame = [&] {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    };

    //ball hits top
    expected_ball.velocity.height = 50;
    check_frame();

}

TEST_CASE("hit side")
{
    Model m(config);

    // No bricks, please.
    m.bricks.clear();

    double const dt = 1;

    m.paddle = {10, 400, 600, 50};
    m.ball.live = true;

    m.ball.velocity = {-50, 1};
    // scene_dims {1024, 768},
    m.ball.center.x = 30;
    m.ball.center.y = 50;


    Ball expected_ball(m.ball);

    auto check_frame = [&] {
        expected_ball = expected_ball.next(dt);
        m.on_frame(dt);
        CHECK(m.ball == expected_ball);
    };

    //ball hits side
    expected_ball.velocity.width = 50;
    check_frame();

}