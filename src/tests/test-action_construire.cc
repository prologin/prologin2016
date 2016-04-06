#include "../actions.hh"

#include "test-helpers.hh"

// ActionConstruire(position position, int player_id)

TEST_F(ActionTest, Construire_InvalidPosition)
{
    ActionConstruire act({0,0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act.check(st));
}

TEST_F(ActionTest, Construire_ConstructionImpossible)
{
    const int N = TAILLE_TERRAIN;

    // players shouldn't be able to build pipes on bases
    ActionConstruire act({N/2,0}, PLAYER_1);
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act.check(st));

    // players shouldn't be able to build pipes on pulsars
    ActionConstruire act2(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act2.check(st));

    // players should be able to build pipes on empty cells
    ActionConstruire act3({1,1}, PLAYER_1);
    EXPECT_EQ(OK, act3.check(st));
    act3.apply_on(st);

    // players shouldn't be able to build pipes on pipes
    EXPECT_EQ(CONSTRUCTION_IMPOSSIBLE, act3.check(st));
}

TEST_F(ActionTest, Construire_Ok)
{
    ActionConstruire act({1,1}, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));
}

TEST_F(ActionTest, Construire_NoMoreActionPoints)
{
    set_points(st, COUT_CONSTRUCTION_TUYAU - 1);
    ActionConstruire act({1,1}, PLAYER_1);

    EXPECT_EQ(PA_INSUFFISANTS, act.check(st));
}

// Set a given number of action points to a given player

