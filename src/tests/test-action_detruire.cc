#include "../actions.hh"

#include "test-helpers.hh"

// ActionDetruire(position position, int player_id)

TEST_F(ActionTest, Detruire_InvalidCell)
{
    ActionDetruire act_destroy({1,1}, PLAYER_1);
    
    EXPECT_EQ(DESTRUCTION_IMPOSSIBLE, act_destroy.check(st));
    st->build_pipe({1,1}, PLAYER_1);
    st->reset_action_points();
    EXPECT_EQ(OK, act_destroy.check(st));
}

TEST_F(ActionTest, Detruire_EmptyCell)
{
    st->build_pipe({1,1}, PLAYER_1);
    
    ActionAmeliorer  act({1,1}, PLAYER_1);
    act.apply_on(st);

    EXPECT_EQ(AMELIORATION_IMPOSSIBLE, act.check(st));
}

