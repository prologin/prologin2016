#include "../actions.hh"

#include "test-helpers.hh"

// ActionConstruire(position position, int player_id)

TEST_F(ActionTest, Construire_InvalidPosition)
{
    ActionConstruire act({0, 0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act.check(*st));
}

TEST_F(ActionTest, Construire_ConstructionImpossible)
{
    // players shouldn't be able to build pipes on bases
    ActionConstruire act(TEST_BASE, PLAYER_1);
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act.check(*st));
    EXPECT_EQ(BASE_JOUEUR, st->get_cell_type(TEST_BASE));

    // players shouldn't be able to build pipes on pulsars
    ActionConstruire act2(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act2.check(*st));
    EXPECT_EQ(PULSAR, st->get_cell_type(TEST_PULSAR_POSITION));
}

TEST_F(ActionTest, Construire_Ok)
{
    ActionConstruire act(TEST_EMPTY_CELL, PLAYER_1);
    EXPECT_EQ(OK, act.check(*st));
    EXPECT_EQ(VIDE, st->get_cell_type(TEST_EMPTY_CELL));

    act.apply(st.get());

    EXPECT_EQ(TUYAU, st->get_cell_type(TEST_EMPTY_CELL));

    // players shouldn't be able to build pipes on pipes
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act.check(*st));
}

TEST_F(ActionTest, Construire_NoMoreActionPoints)
{
    set_points(st.get(), COUT_CONSTRUCTION - 1);
    ActionConstruire act({1, 1}, PLAYER_1);

    EXPECT_EQ(PA_INSUFFISANTS, act.check(*st));
}

// Set a given number of action points to a given player
