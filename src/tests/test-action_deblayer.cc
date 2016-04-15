#include "../actions.hh"

#include "test-helpers.hh"

// ActionDeblayer(position position, int player_id)

TEST_F(ActionTest, Deblayer_InvalidCell)
{
    ActionDeblayer act(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(PAS_DE_DEBRIS, act.check(st));

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


TEST_F(ActionTest, Deblayer_NotEnoughActionPoints)
{
    st->build_pipe({1,1}, PLAYER_1);
    st->reset_action_points();

    st->destroy_pipe({1,1});
    st->reset_action_points();

    set_points(st, COUT_DEBLAYAGE - 1);

    ActionDeblayer act({1,1}, PLAYER_1);
    EXPECT_EQ(PA_INSUFFISANTS, act.check(st));
}
