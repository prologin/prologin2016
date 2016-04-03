// FIXME License notice

#include "game_state.hh"

GameState::GameState(std::ifstream& map_stream, rules::Players_sptr players)
    : rules::GameState(),
      map_(TAILLE_TERRAIN * TAILLE_TERRAIN, {case_type::VIDE, 0, -1, -1}),
      turn_(0),
      action_points_(0),
      players_(players)
{
    // TODO
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

int GameState::get_current_player() const
{
    return 2 - turn_ % 2;
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
