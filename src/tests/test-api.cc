#include "../api.hh"

#include "test-helpers.hh"

TEST_F(ApiTest, Api_Moi)
{
    for (auto& player : players)
    {
        EXPECT_EQ(player.id, player.api->moi());
    }
}

TEST_F(ApiTest, Api_Adversaire)
{
    for (int player_index : {0, 1})
    {
        int expected = players[(player_index + 1) % 2].id;
        EXPECT_EQ(expected, players[player_index].api->adversaire());
    }
}
