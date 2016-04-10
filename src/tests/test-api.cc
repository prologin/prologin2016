#include "../api.hh"

#include "test-helpers.hh"

#include <algorithm>

TEST_F(ApiTest, Api_ListeDebris)
{
    std::vector<position> expected;
    EXPECT_EQ(expected, players[0].api->liste_debris());
    auto build_destroy = [&](position pos) {
        st->build_pipe(pos, players[0].id);
        EXPECT_TRUE(std::is_permutation(expected.begin(), expected.end(),
                    players[0].api->liste_debris().begin()));
        st->destroy_pipe(pos);
        expected.push_back(pos);
        EXPECT_TRUE(std::is_permutation(expected.begin(), expected.end(),
                    players[0].api->liste_debris().begin()));
    };
    build_destroy({1, 1});
    build_destroy({2, 1});
    build_destroy({1, 2});
}

TEST_F(ApiTest, Api_EstPulsar)
{
    EXPECT_FALSE(players[0].api->est_pulsar({1, 1}));
    EXPECT_TRUE(players[0].api->est_pulsar(TEST_PULSAR_POSITION));
}

TEST_F(ApiTest, Api_EstTuyau)
{
    EXPECT_FALSE(players[0].api->est_tuyau({1, 1}));
    st->build_pipe({1, 1}, players[0].id);
    EXPECT_TRUE(players[0].api->est_tuyau({1, 1}));
    st->upgrade_pipe({1, 1}, players[0].id);
    EXPECT_FALSE(players[0].api->est_tuyau({1, 1}));
}

TEST_F(ApiTest, Api_EstSuperTuyau)
{
    EXPECT_FALSE(players[0].api->est_super_tuyau({1, 1}));
    st->build_pipe({1, 1}, players[0].id);
    EXPECT_FALSE(players[0].api->est_super_tuyau({1, 1}));
    st->upgrade_pipe({1, 1}, players[0].id);
    EXPECT_TRUE(players[0].api->est_super_tuyau({1, 1}));
}

TEST_F(ApiTest, Api_EstDebris)
{
    EXPECT_FALSE(players[0].api->est_debris({1, 1}));
    st->build_pipe({1, 1}, players[0].id);
    EXPECT_FALSE(players[0].api->est_debris({1, 1}));
    st->destroy_pipe({1, 1});
    EXPECT_TRUE(players[0].api->est_debris({1, 1}));
}

TEST_F(ApiTest, Api_EstLibre)
{
    EXPECT_TRUE(players[0].api->est_libre({1, 1}));
    EXPECT_FALSE(players[0].api->est_libre({0, TAILLE_TERRAIN / 2}));
    st->build_pipe({1, 1}, players[0].id);
    EXPECT_FALSE(players[0].api->est_libre({1, 1}));
}

TEST_F(ApiTest, Api_InfoPulsar)
{
    auto pulsar = st->get_pulsar(TEST_PULSAR_POSITION);
    for (auto& player : players)
    {
        auto p = player.api->info_pulsar(TEST_PULSAR_POSITION);
        EXPECT_EQ(pulsar.puissance, p.puissance);
        EXPECT_EQ(pulsar.periode, p.periode);
        EXPECT_EQ(pulsar.nombre_pulsations, p.nombre_pulsations);
    }
}

TEST_F(ApiTest, Api_ChargesPresentes)
{
    for (int i = 1; i < TEST_PULSAR_POSITION.x; ++i)
        st->build_pipe({i, TEST_PULSAR_POSITION.y}, players[0].id);
    auto pulsar = st->get_pulsar(TEST_PULSAR_POSITION);
    while ((int)st->get_turn() != pulsar.periode)
        st->increment_turn();
    position pos{TEST_PULSAR_POSITION.x - 1, TEST_PULSAR_POSITION.y};
    EXPECT_EQ(0, players[0].api->charges_presentes(pos));
    st->emit_plasma();
    EXPECT_EQ(pulsar.puissance, players[0].api->charges_presentes(pos));
}

TEST_F(ApiTest, Api_BasesListes)
{
    EXPECT_EQ(players[0].api->ma_base(), players[1].api->base_ennemie());
    EXPECT_EQ(players[1].api->ma_base(), players[0].api->base_ennemie());
    std::vector<position> b1 = players[0].api->ma_base();
    std::vector<position> b2 = players[1].api->ma_base();
    for (int i = 0; i < TAILLE_TERRAIN; ++i)
    {
        if (!(i >= TAILLE_TERRAIN / 3 && i < TAILLE_TERRAIN * 2 / 3))
            continue;
        position borders[4] = {{0, i}, {i, 0},
            {TAILLE_TERRAIN - 1, i}, {i, TAILLE_TERRAIN - 1}};
        for (position pos : borders)
        {
            EXPECT_TRUE(std::find(b1.begin(), b1.end(), pos) != b1.end() ||
                    std::find(b2.begin(), b2.end(), pos) != b2.end());
        }
    }
}

TEST_F(ApiTest, Api_PuissanceApiration)
{
    for (int i = 0; i < TAILLE_TERRAIN; ++i)
    {
        int p = i >= TAILLE_TERRAIN / 3 && i < TAILLE_TERRAIN * 2 / 3 ? 1 : -1;
        EXPECT_EQ(p, players[0].api->puissance_aspiration({0, i}));
    }
    position pos{TAILLE_TERRAIN / 2, 0};
    EXPECT_EQ(1, players[0].api->puissance_aspiration(pos));
    st->increment_vacuum(pos);
    EXPECT_EQ(2, players[0].api->puissance_aspiration(pos));
    st->increment_vacuum(pos);
    EXPECT_EQ(3, players[0].api->puissance_aspiration(pos));
    pos.x += 1;
    EXPECT_EQ(1, players[0].api->puissance_aspiration(pos));
}

TEST_F(ApiTest, Api_DirectionsPlasma)
{
    // TODO
}

TEST_F(ApiTest, Api_CoutProchaineModificationAspiration)
{
    for (auto& player : players)
    {
        EXPECT_EQ(0, player.api->cout_prochaine_modification_aspiration());
        st->set_vacuum_moved(true);
        EXPECT_EQ(COUT_MODIFICATION_ASPIRATION,
                player.api->cout_prochaine_modification_aspiration());
        st->set_vacuum_moved(false);
        EXPECT_EQ(0, player.api->cout_prochaine_modification_aspiration());
        auto bases = player.api->ma_base();
        player.api->deplacer_aspiration(bases[0], bases[1]);
        EXPECT_EQ(COUT_MODIFICATION_ASPIRATION,
                player.api->cout_prochaine_modification_aspiration());
        st->set_vacuum_moved(false);
    }
}

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
