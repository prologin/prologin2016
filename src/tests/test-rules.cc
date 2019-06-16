#include "test-helpers.hh"

/// Test that we finish at the correct time
TEST_F(RulesTest, Rules_Finish)
{
    const unsigned first_turn = 1;
    unsigned turn = first_turn;
    while (!rules->is_finished())
    {
        rules->start_of_player_turn(1);
        EXPECT_EQ(turn, rules->get_game_state().get_turn());
        ++turn;
        rules->end_of_player_turn(1);
        EXPECT_FALSE(rules->is_finished());
        rules->start_of_player_turn(2);
        EXPECT_EQ(turn, rules->get_game_state().get_turn());
        ++turn;
        rules->end_of_player_turn(2);
    }
    EXPECT_EQ(NB_TOURS, static_cast<int>(turn - first_turn));
}
