#include "../actions.hh"

#include "test-helpers.hh"

// ActionDeblayer(position position, int player_id)

TEST_F(ActionTest, Deblayer_InvalidCell)
{
    ActionDeblayer act(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(PAS_DE_TUYAU_DETRUIT, act.check(st));
    
    ActionDeblayer act2({0,0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act2.check(st));
}

TEST_F(ActionTest, Deblayer_Ok)
{
    st->build_pipe({1,1}, PLAYER_1);
    st->reset_action_points();
    
    st->destroy_pipe({1,1});
    st->reset_action_points();
    
    ActionDeblayer act({1,1}, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));
}
