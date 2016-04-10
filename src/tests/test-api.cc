#include "../api.hh"

#include "test-helpers.hh"

TEST_F(ApiTest, Api_HistTuyauxConstruits)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_tuyaux_construits());
        auto build = [&](position pos) {
            set_points(st, COUT_CONSTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->construire(pos));
            expected.push_back(pos);
            EXPECT_EQ(expected, other.api->hist_tuyaux_construits());
        };
        build({1, 1 + player_index * 2});
        build({2, 1 + player_index * 2});
        build({1, 2 + player_index * 2});
    }
}

TEST_F(ApiTest, Api_HistTuyauxDetruits)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_tuyaux_detruits());
        auto build_destroy = [&](position pos) {
            set_points(st, COUT_CONSTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->construire(pos));
            set_points(st, COUT_DESTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->detruire(pos));
            expected.push_back(pos);
            EXPECT_EQ(expected, other.api->hist_tuyaux_detruits());
        };
        build_destroy({1, 1 + player_index * 2});
        build_destroy({2, 1 + player_index * 2});
        build_destroy({1, 2 + player_index * 2});
    }
}

TEST_F(ApiTest, Api_HistTuyauxAmeliores)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_tuyaux_ameliores());
        auto build_upgrade = [&](position pos) {
            set_points(st, COUT_CONSTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->construire(pos));
            set_points(st, COUT_AMELIORATION_TUYAU);
            EXPECT_EQ(OK, player.api->ameliorer(pos));
            expected.push_back(pos);
            EXPECT_EQ(expected, other.api->hist_tuyaux_ameliores());
        };
        build_upgrade({1, 1 + player_index * 2});
        build_upgrade({2, 1 + player_index * 2});
        build_upgrade({1, 2 + player_index * 2});
    }
}

TEST_F(ApiTest, Api_HistDebrisDeblayes)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_debris_deblayes());
        auto build_destroy_clear = [&](position pos) {
            set_points(st, COUT_CONSTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->construire(pos));
            set_points(st, COUT_DESTRUCTION_TUYAU);
            EXPECT_EQ(OK, player.api->detruire(pos));
            set_points(st, COUT_DEBLAYAGE);
            EXPECT_EQ(OK, player.api->deblayer(pos));
            expected.push_back(pos);
            EXPECT_EQ(expected, other.api->hist_debris_deblayes());
        };
        build_destroy_clear({1, 1});
        build_destroy_clear({2, 1});
        build_destroy_clear({1, 2});
    }
}

TEST_F(ApiTest, Api_HistPointAspirationAjoutes)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_points_aspiration_ajoutes());
        auto bases = player.api->ma_base();
        auto move = [&](int base_id) {
            set_points(st, COUT_MODIFICATION_ASPIRATION);
            expected.push_back(bases[base_id]);
            st->increment_vacuum(bases[0]); // let's cheat a bit
            return player.api->deplacer_aspiration(bases[0], bases[base_id]);
        };
        EXPECT_EQ(OK, move(1));
        EXPECT_EQ(OK, move(2));
        EXPECT_EQ(OK, move(5));
        EXPECT_EQ(expected, other.api->hist_points_aspiration_ajoutes());
    }
}

TEST_F(ApiTest, Api_HistPointAspirationRetires)
{
    for (int player_index : {0, 1})
    {
        auto& player = players[player_index];
        auto& other = players[(player_index + 1) % 2];
        std::vector<position> expected;
        EXPECT_EQ(expected, other.api->hist_points_aspiration_retires());
        auto bases = player.api->ma_base();
        auto move = [&](int base_id, int to = 0) {
            set_points(st, COUT_MODIFICATION_ASPIRATION);
            expected.push_back(bases[base_id]);
            return player.api->deplacer_aspiration(bases[base_id], bases[to]);
        };
        EXPECT_EQ(OK, move(1));
        EXPECT_EQ(OK, move(0, 1));
        EXPECT_EQ(OK, move(2));
        EXPECT_EQ(OK, move(5));
        EXPECT_EQ(OK, move(0, 1));
        EXPECT_EQ(expected, other.api->hist_points_aspiration_retires());
    }
}

TEST_F(ApiTest, Api_Moi)
{
    for (auto& player : players)
    {
        EXPECT_EQ(player.id, player.api->moi());
    }
}

TEST_F(ApiTest, Api_Adversaire)
{
    for (int player_index : {0, 1})
    {
        int expected = players[(player_index + 1) % 2].id;
        EXPECT_EQ(expected, players[player_index].api->adversaire());
    }
}

TEST_F(ApiTest, Api_PointsAction)
{
    for (auto& player : players)
    {
        for (unsigned value : {0, 4, 10, 30, 40})
        {
            set_points(st, value);
            ASSERT_EQ(true, player.api->points_action() >= 0);
            EXPECT_EQ(value, (unsigned)player.api->points_action());
        }
    }
}

TEST_F(ApiTest, Api_Score)
{
    auto check_score = [this](int value, int index) {
        int player_id = players[index].id;
        EXPECT_EQ(value, players[index].api->score(player_id));
        EXPECT_EQ(value, players[(index + 1) % 2].api->score(player_id));
    };
    for (int player_index : {0, 1})
    {
        PlayerInfo info = st->get_player_info().at(players[player_index].id);
        check_score(0, player_index);
        info.collect_plasma(0.3);
        check_score(0, player_index);
        info.collect_plasma(0.7);
        check_score(1, player_index);
        info.collect_plasma(7.8);
        check_score(8, player_index);
        info.collect_plasma(15.3);
        check_score(24, player_index);
        info.collect_plasma(24.7);
        check_score(48, player_index);
        info.collect_plasma(0.25);
        check_score(49, player_index);
    }
}

TEST_F(ApiTest, Api_TourActuel)
{
    for (int turn = 1; turn < NB_TOURS + 1; ++turn)
    {
        for (auto& player : players)
        {
            EXPECT_EQ(turn, player.api->tour_actuel());
        }
        st->increment_turn();
    }
}

TEST_F(ApiTest, Api_Annuler)
{
    for (auto& player : players)
    {
        EXPECT_FALSE(player.api->annuler());
        position pos{1, 1};
        EXPECT_TRUE(player.api->est_libre(pos));
        set_points(st, COUT_CONSTRUCTION_TUYAU);
        EXPECT_EQ(OK, player.api->construire(pos));
        EXPECT_TRUE(player.api->est_tuyau(pos));
        EXPECT_TRUE(player.api->annuler());
        EXPECT_TRUE(player.api->est_libre(pos));
    }
}
