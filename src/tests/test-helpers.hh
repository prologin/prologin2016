#ifndef TEST_HELPERS_HH
#define TEST_HELPERS_HH

#include <fstream>
#include <sstream>

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

static rules::Players make_players(int id1, int id2)
{
    /* Create two players (no spectator).  */
    rules::Players players;
    players.add(std::make_shared<rules::Player>(id1, rules::PLAYER));
    players.add(std::make_shared<rules::Player>(id2, rules::PLAYER));
    return players;
}

static std::unique_ptr<GameState>
make_test_gamestate(std::string map, const rules::Players& players)
{
    std::istringstream map_stream(map);
    return std::make_unique<GameState>(map_stream, players);
}

class ActionTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        st = make_test_gamestate(some_map, make_players(PLAYER_1, PLAYER_2));
    }

    std::unique_ptr<GameState> st;

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
        auto gs_players = make_players(player_id_1, player_id_2);
        auto st = make_test_gamestate(some_map, gs_players);
        players[0].id = player_id_1;
        players[0].api = std::make_unique<Api>(
            std::unique_ptr<GameState>(st->copy()), gs_players[0]);
        players[1].id = player_id_2;
        players[1].api = std::make_unique<Api>(
            std::unique_ptr<GameState>(st->copy()), gs_players[1]);
    }

    struct Player
    {
        int id;
        std::unique_ptr<Api> api;
    };
    std::array<Player, 2> players;

    GameState* st(Player& player) { return &player.api->game_state(); }

    void send_actions(Player* src, Player* dst)
    {
        // Apply action from src to dst's game state, then clear src's actions
        for (const auto& action : src->api->actions()->all())
            dst->api->game_state().apply(*action);
        src->api->actions()->clear();
    }
};

class RulesTest : public ::testing::Test
{
protected:
    constexpr static int PLAYER_ID_1 = 1;
    constexpr static int PLAYER_ID_2 = 2;

    virtual void SetUp()
    {
        utils::Logger::get().level() = utils::Logger::DEBUG_LEVEL;
        auto players_ptr = make_players(PLAYER_ID_1, PLAYER_ID_2);
        rules::Options opt;
        if (!std::ifstream("map.txt").good())
        {
            std::ofstream map("map.txt");
            map << some_map;
        }
        opt.map_file = "map.txt";
        opt.players = std::move(players_ptr);
        rules.reset(new Rules(opt));
    }

    std::unique_ptr<Rules> rules;
};

// Set a given number of action points
static inline void set_points(GameState* st, unsigned pts)
{
    st->reset_action_points();
    st->decrease_action_points(st->get_action_points() - pts);
}

#endif
