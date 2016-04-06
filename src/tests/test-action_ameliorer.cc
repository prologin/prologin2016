#include "../actions.hh"

#include "test-helpers.hh"

// ActionAmeliorer(position position, int player_id)

TEST_F(ActionTest, Ameliorer_InvalidCell)
{
    ActionAmeliorer *act;

    act = new ActionAmeliorer(TEST_PULSAR_POSITION, PLAYER_1);
    EXPECT_EQ(AUCUN_TUYAU, act->check(st));
    delete act;
    
    act = new ActionAmeliorer({0,0}, PLAYER_1);
    EXPECT_EQ(POSITION_INVALIDE, act->check(st));
    delete act;
}


TEST_F(ActionTest, Ameliorer_EmptyCell)
{
    ActionConstruire act({1,1}, PLAYER_1);
    ActionAmeliorer  act2({1,1}, PLAYER_1);
    
    EXPECT_EQ(OK, act.check(st));
    act.apply_on(st);
    EXPECT_EQ(OK, act2.check(st));
    act2.apply_on(st);
    EXPECT_EQ(AMELIORATION_IMPOSSIBLE, act2.check(st));
}

