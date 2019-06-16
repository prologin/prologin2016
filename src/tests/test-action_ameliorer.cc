#include "../actions.hh"

#include "test-helpers.hh"

// ActionAmeliorer(position position, int player_id)

TEST_F(ActionTest, Ameliorer_InvalidCell)
{
    ActionAmeliorer* act;

    act = new ActionAmeliorer(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(AUCUN_TUYAU, act->check(*st));
    EXPECT_EQ(PULSAR, st->get_cell_type(TEST_PULSAR_POSITION));
    delete act;

    act = new ActionAmeliorer({0, 0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act->check(*st));
    EXPECT_EQ(INTERDIT, st->get_cell_type({0, 0}));
    delete act;
}

TEST_F(ActionTest, Ameliorer_EmptyCell)
{
    ActionConstruire act(TEST_EMPTY_CELL, PLAYER_1);
    ActionAmeliorer act2(TEST_EMPTY_CELL, PLAYER_1);

    EXPECT_EQ(OK, act.check(*st));
    EXPECT_EQ(VIDE, st->get_cell_type(TEST_EMPTY_CELL));
    act.apply(st.get());
    EXPECT_EQ(OK, act2.check(*st));
    EXPECT_EQ(TUYAU, st->get_cell_type(TEST_EMPTY_CELL));
    act2.apply(st.get());
    EXPECT_EQ(AMELIORATION_IMPOSSIBLE, act2.check(*st));
    EXPECT_EQ(SUPER_TUYAU, st->get_cell_type(TEST_EMPTY_CELL));
}

TEST_F(ActionTest, Ameliorer_NotEnoughActionPoints)
{
    set_points(st.get(), COUT_AMELIORATION - 1);
    ActionConstruire act(TEST_EMPTY_CELL, PLAYER_1);
    ActionAmeliorer act2(TEST_EMPTY_CELL, PLAYER_1);

    EXPECT_EQ(PA_INSUFFISANTS, act.check(*st));
}
