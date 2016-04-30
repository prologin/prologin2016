#include "../actions.hh"
#include "test-helpers.hh"

// ActionDeplacerAspiration(position source, position destination, int
// player_id)

TEST_F(ActionTest, DeplacerAspiration_InvalidPosition)
{
    ActionDeplacerAspiration act(TEST_BASE, {1, 1}, PLAYER_1);
    EXPECT_EQ(PAS_DANS_BASE, act.check(st));

    ActionDeplacerAspiration act2({1, 1}, TEST_BASE, PLAYER_1);
    EXPECT_EQ(PAS_DANS_BASE, act2.check(st));

    ActionDeplacerAspiration act3(TEST_BASE, TEST_BASE, PLAYER_2);
    EXPECT_EQ(DEPLACEMENT_INVALIDE, act3.check(st));

    ActionDeplacerAspiration act4(TEST_BASE, TEST_BASE_ALT, PLAYER_2);
    EXPECT_EQ(PAS_DANS_BASE, act4.check(st));
}

TEST_F(ActionTest, DeplacerAspiration_Ok)
{
    ActionDeplacerAspiration act(TEST_BASE, TEST_BASE_ALT, PLAYER_1);
    EXPECT_EQ(OK, act.check(st));
}

TEST_F(ActionTest, DeplacerAspiration_ActionPoints)
{
    ActionDeplacerAspiration act(TEST_BASE, TEST_BASE_ALT, PLAYER_1);
    ActionDeplacerAspiration act2(TEST_BASE_ALT, TEST_BASE, PLAYER_1);

    set_points(st, COUT_MODIFICATION_ASPIRATION);
    EXPECT_EQ(OK, act.check(st));

    // first move should be free
    unsigned orig_action_points = st->get_action_points();
    act.apply_on(st);
    EXPECT_EQ(orig_action_points, st->get_action_points());

    set_points(st, COUT_MODIFICATION_ASPIRATION - 1);
    EXPECT_EQ(PA_INSUFFISANTS, act2.check(st));
    act2.apply_on(st);

    for (unsigned i = 0; i < st->get_vacuum(TEST_BASE); i++)
    {
        set_points(st, COUT_MODIFICATION_ASPIRATION);
        EXPECT_EQ(OK, act.check(st));
        act.apply_on(st);
        EXPECT_EQ((unsigned)0, st->get_action_points());
    }
    EXPECT_EQ(PUISSANCE_INSUFFISANTE, act.check(st));
}

TEST_F(ActionTest, DeplacerAspiration_AspirationLimit)
{
    position destination_base = {N / 2 - 4, 0};
    position source_base;
    ActionDeplacerAspiration act;
    for(int i = 0; i <  LIMITE_ASPIRATION - 1; i++)
    {
        source_base = {N / 2 - 3 + i, 0};
        st->reset_action_points();
        ActionDeplacerAspiration act(source_base, destination_base, PLAYER_1);
        EXPECT_EQ(OK, act.check(st));
        act.apply_on(st);
    }
    source_base = {N / 2 - 3 + LIMITE_ASPIRATION, 0};
    st->reset_action_points();
    ActionDeplacerAspiration act2(source_base, destination_base, PLAYER_1);
    EXPECT_EQ(LIMITE_ASPIRATION, (int) st->get_vacuum(destination_base));
    EXPECT_EQ(LIMITE_ASPIRATION_ATTEINTE, act2.check(st));
    EXPECT_EQ(LIMITE_ASPIRATION, (int) st->get_vacuum(destination_base));
}
