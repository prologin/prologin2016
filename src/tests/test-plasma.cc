#include <cmath>
#include "../game_state.hh"
#include "test-helpers.hh"

// Not to scale.
// Top-left corner at (0,0)
// Pulsar (O) at (18,18) (it is off-center)
// Rubble (X) at (18,17), (17,19), (19,19)
// Super-pipe (~) at (18,19)
// Pointed base cells have vacuum +2.
//
//         v
//  #####111111#####
//  #      =       #
//  #      =       #
//  #      =       #
//  2      =       2
// >2======X=======2<
//  2     =O=======2
//  2     X~X      2
//  2     ===      2
//  2     ===      2
//  #     ===      #
//  #     ===      #
//  #     ===      #
//  #####111111#####
//        ^ ^

TEST_F(ActionTest, Plasma)
{
    const double pow_2_19 = std::pow(2., 19);

    for (int i = 1 ; i < 18 ; i++)
    {
        st->build_pipe({18,i}, PLAYER_1);
        st->build_pipe({i,17}, PLAYER_2);
    }
    for (int i = 19 ; i < TAILLE_TERRAIN-1 ; i++)
    {
        for (int j = -1 ; j < 2 ; j++)
            st->build_pipe({18+j,i}, PLAYER_1);
        for (int j = 0 ; j < 2 ; j++)
            st->build_pipe({i,17+j}, PLAYER_2);
    }
    st->build_pipe({17,18}, PLAYER_1);
    st->upgrade_pipe({18,19}, PLAYER_1);
    st->destroy_pipe({18,17});
    st->destroy_pipe({17,19});
    st->destroy_pipe({19,19});

    for (int i = 0 ; i < 2 ; i++)
    {
        st->increment_vacuum({TAILLE_TERRAIN-1,17});
        st->increment_vacuum({17,TAILLE_TERRAIN-1});
        st->increment_vacuum({19,TAILLE_TERRAIN-1});
        st->increment_vacuum({18,0});
    }

    while (st->get_turn() < 25)
    {
        st->increment_turn();
        st->move_plasma();
        st->emit_plasma();

        if (3 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(0, st->get_plasma({18,17}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({19,18}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({18,19}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({17,18}));
        }
        else if (4 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(5./2, st->get_plasma({19,17}));
            EXPECT_FLOAT_EQ(5./2, st->get_plasma({20,18}));
            EXPECT_FLOAT_EQ(5./3, st->get_plasma({17,20}));
            EXPECT_FLOAT_EQ(5./3, st->get_plasma({19,20}));
            EXPECT_FLOAT_EQ(5./3, st->get_plasma({18,21}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({17,17}));
        }
        else if (5 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(0, st->get_plasma({18,17}));
            EXPECT_FLOAT_EQ(15./4, st->get_plasma({20,17}));
            EXPECT_FLOAT_EQ(5./4, st->get_plasma({21,18}));
            EXPECT_FLOAT_EQ(20./9, st->get_plasma({17,21}));
            EXPECT_FLOAT_EQ(20./9, st->get_plasma({19,21}));
            EXPECT_FLOAT_EQ(5./9, st->get_plasma({18,22}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({16,17}));
        }
        else if (19 == st->get_turn())
        {
            st->destroy_pipe({2, 17});
            st->destroy_pipe({TAILLE_TERRAIN-2,17});
            st->reset_board_distances();
            EXPECT_FLOAT_EQ(5, st->get_plasma({2,17}));
            EXPECT_FLOAT_EQ(5, st->get_plasma({5,17}));
            EXPECT_NEAR(5.-5./pow_2_19, st->get_plasma({34,17}), pow_2_19);
        }
        else if (20 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(5, st->get_plasma({1,17}));
            EXPECT_FLOAT_EQ(0, st->get_plasma({4,17}));
            EXPECT_NEAR(5.-5./pow_2_19, st->get_plasma({34,18}), pow_2_19);
        }
        else if (21 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(5, st->get_collected_plasma(PLAYER_2));
        }
        else if (22 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(5, st->get_collected_plasma(PLAYER_1));
        }
        else if (24 == st->get_turn())
        {
            EXPECT_FLOAT_EQ(10, st->get_collected_plasma(PLAYER_2));
        }
    }
}
