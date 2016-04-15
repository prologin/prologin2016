#ifndef TEST_HELPERS_HH
#define TEST_HELPERS_HH

#include <sstream>
#include <fstream>

#include <gtest/gtest.h>

#include "../api.hh"
#include "../constant.hh"
#include "../game_state.hh"
#include "../rules.hh"

// the format is: <x> <y> <period> <power> <lifetime>

static const position TEST_PULSAR_POSITION = {18, 18};
static const std::string some_map = ("18 18 3 5 10\n");

static const int N = TAILLE_TERRAIN;
static const position TEST_BASE = {N / 2, 0};
static const position TEST_BASE_ALT = {N / 2 + 1, 0};
static const position TEST_EMPTY_CELL = {1, 1};

static rules::Players_sptr make_players(int id1, int id2)
{
    /* Create two players (no spectator).  */
    return rules::Players_sptr(
        new rules::Players{std::vector<rules::Player_sptr>{
            rules::Player_sptr(new rules::Player(id1, rules::PLAYER)),
            rules::Player_sptr(new rules::Player(id2, rules::PLAYER)),
        }});
}

static GameStateWrapper make_test_gamestate(std::string map,
                                            const rules::Players_sptr& players)
{
    std::istringstream map_stream(map);
    return GameStateWrapper(new GameState(map_stream, players));
}

class ActionTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        st = make_test_gamestate(some_map, make_players(PLAYER_1, PLAYER_2));
    }

    virtual void TearDown() { delete st; }

    GameState* st;

    const int PLAYER_1 = 0;
    const int PLAYER_2 = 1;
};

class ApiTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        // Players values are not 0 and 1, because that would be too simple
        int player_id_1 = 1337;
        int player_id_2 = 42;
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        auto players_ptr = make_players(player_id_1, player_id_2);
        st = make_test_gamestate(some_map, players_ptr);
        players[0].id = player_id_1;
        players[0].api = new Api(st, players_ptr->players[0]);
        players[1].id = player_id_2;
        players[1].api = new Api(st, players_ptr->players[1]);
    }

    virtual void TearDown()
    {
        delete players[0].api;
        delete players[1].api;
    }

    GameStateWrapper st = GameStateWrapper(nullptr);

    struct Player
    {
        int id;
        Api* api;
    };
    std::array<Player, 2> players;
};

class RulesTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        int player_id_1 = 1;
        int player_id_2 = 2;
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        auto players_ptr = make_players(player_id_1, player_id_2);
        rules::Options opt;
        if (!std::ifstream("map.txt").good())
        {
            std::ofstream map("map.txt");
            map << some_map;
        }
        opt.map_file = "map.txt";
        opt.players = std::move(players_ptr);
        rules = new Rules(opt);
    }

    virtual void TearDown() { delete rules; }

    Rules* rules;
};

// Set a given number of action points to a given player
static inline void set_points(GameState* st, unsigned pts)
{
    st->reset_action_points();
    st->decrease_action_points(st->get_action_points() - pts);
}

#endif
