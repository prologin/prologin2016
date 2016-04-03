// FIXME License notice

#include "game_state.hh"

GameState::GameState(std::istream& map_stream, rules::Players_sptr players)
    : rules::GameState(),
      map_(TAILLE_TERRAIN * TAILLE_TERRAIN, {case_type::VIDE, 0, -1, -1}),
      turn_(0),
      action_points_(0)
{
    int p_count = 0;
    for (auto& p : players->players)
    {
        if (p->type == rules::PLAYER)
        {
            if (p_count >= 2)
                FATAL("Too many players. This is a two-player game.");
            p->score = 0;
            players_[p_count++] = p;
        }
    }
    if (p_count < 2)
        FATAL("Not enough players. This is a two-player game.");

    const int N = TAILLE_TERRAIN;
    for (int i = 0 ; i < N ; i++)
    {
        position wall[] = {{i, 0}, {i, N-1}, {0, i}, {N-1, i}};
        for (int j = 0 ; j < 4 ; j++)
        {
            if (N / 3 <= i && i < N - N / 3)
                cell(wall[j]) = {case_type::BASE, 0, -1, j / 2};
            else
                cell(wall[j]) = {case_type::INTERDIT, 0, -1, -1};
        }
    }

    while (map_stream >> std::ws, !map_stream.eof())
    {
        position pos;
        pulsar pr;

        map_stream >> pos.x >> pos.y >>
            pr.periode >> pr.puissance >> pr.plasma_total;

        pulsars_pos_.push_back(pos);

        auto& c = cell(pos);
        c.type = case_type::PULSAR;
        c.pulsar = pulsars_info_.size();

        pulsars_info_.push_back(pr);
    }
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

rules::Player_sptr GameState::get_current_player() const
{
    return players_[turn_ % 2];
}

void GameState::decrease_action_points(int delta)
{
    action_points_ -= delta;
}

void GameState::reset_action_points()
{
    action_points_ = NB_POINTS_ACTION;
}

int GameState::base_cell(position p) const
{
    const Cell& c = cell(p);
    return (c.type == BASE) ? c.owner : -1;
}

Cell& GameState::cell(position p)
{
    return map_[map_index(p)];
}

Cell GameState::cell(position p) const
{
    return map_[map_index(p)];
}

int GameState::map_index(position p) const
{
    return p.x * TAILLE_TERRAIN + p.y;
}

bool GameState::in_bounds(position p) const
{
    return 0 <= p.x && p.x < TAILLE_TERRAIN &&
        0 <= p.y && p.y < TAILLE_TERRAIN;
}
