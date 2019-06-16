#include "../actions.hh"

#include "test-helpers.hh"

// ActionDetruire(position position, int player_id)

TEST_F(ActionTest, Detruire_PipeCell)
{
    ActionDetruire act_destroy({1, 1}, PLAYER_1);

    // Not possible to destroy if there's no pipe
    EXPECT_EQ(DESTRUCTION_IMPOSSIBLE, act_destroy.check(*st));

    st->build_pipe({1, 1}, PLAYER_1);
    st->reset_action_points();

    // Not possible to destroy without enough plasma
    EXPECT_EQ(CHARGE_INSUFFISANTE, act_destroy.check(*st));

    st->decrease_plasma(PLAYER_1, -CHARGE_DESTRUCTION);
    EXPECT_EQ(OK, act_destroy.check(*st));
}

TEST_F(ActionTest, Detruire_SuperPipeCell)
{
    st->build_pipe({1, 1}, PLAYER_1);
    st->decrease_plasma(PLAYER_1, -CHARGE_DESTRUCTION);

    ActionAmeliorer act({1, 1}, PLAYER_1);
    EXPECT_EQ(OK, act.check(*st));
    act.apply(st.get());

    // Not possible to destroy a super-pipe with less than 40 points
    ActionDetruire act_destroy({1, 1}, PLAYER_1);
    EXPECT_EQ(PA_INSUFFISANTS, act_destroy.check(*st));

    st->reset_action_points();
    EXPECT_EQ(OK, act_destroy.check(*st));

    // Check that there's no points left after performing a destruction of a
    // super-pipe
    act_destroy.apply(st.get());
    EXPECT_EQ(NB_POINTS_ACTION - COUT_DESTRUCTION_SUPER_TUYAU,
              (int)st->get_action_points());
}

TEST_F(ActionTest, Detruire_BrokenPipeCell)
{
    st->build_pipe({1, 1}, PLAYER_1);
    st->decrease_plasma(PLAYER_1, -CHARGE_DESTRUCTION);

    ActionDetruire act_destroy({1, 1}, PLAYER_1);
    EXPECT_EQ(OK, act_destroy.check(*st));
    act_destroy.apply(st.get());

    // Not possible to destroy a pipe already broken
    st->reset_action_points();
    EXPECT_EQ(DESTRUCTION_IMPOSSIBLE, act_destroy.check(*st));
}

TEST_F(ActionTest, Detruire_InvalidPosition)
{
    // Not possible to destroy anything out of the map
    ActionDetruire act_destroy({0, 0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act_destroy.check(*st));
}
