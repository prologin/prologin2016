#ifndef TEST_HELPERS_HH
#define TEST_HELPERS_HH

#include <sstream>

#include <gtest/gtest.h>

#include "../constant.hh"
#include "../game_state.hh"



// the format is: <x> <y> <period> <power> <lifetime>

static const position TEST_PULSAR_POSITION = {18,18};
static const std::string some_map = (
    "18 18 3 5 10\n"
);

static const int N = TAILLE_TERRAIN;
static const position TEST_BASE = {N/2,0};
static const position TEST_EMPTY_CELL = {1,1};

static GameState* make_test_gamestate(std::string map)
{
    std::istringstream map_stream(map);

    /* Create two players (no spectator).  */
    rules::Players_sptr players(new rules::Players {
        std::vector<rules::Player_sptr> {
            rules::Player_sptr(new rules::Player(0, rules::PLAYER)),
	    rules::Player_sptr(new rules::Player(1, rules::PLAYER)),
	}
    });
    return new GameState(map_stream, players);
}

class ActionTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        st = make_test_gamestate(some_map);
    }

    virtual void TearDown()
    {
        delete st;
    }

    GameState* st;

    const int PLAYER_1 = 0;
    const int PLAYER_2 = 1;
};


// Set a given number of action points to a given player
static inline void set_points(GameState* st, unsigned pts)
{
    st->reset_action_points();
    st->decrease_action_points(st->get_action_points() - pts);
}

#endif
