// FIXME License notice

#include "game_state.hh"

GameState::GameState(std::ifstream& map_stream, rules::Players_sptr players)
    : rules::GameState(),
      map_(TAILLE_TERRAIN * TAILLE_TERRAIN, {case_type::VIDE, 0, -1}),
      turn_(0),
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

int GameState::base_cell(position p) const
{
    // Player 1's base cells are on the left and right borders.
    if ((p.x == -1 || p.x == map_size_) &&
        (map_size_ / 3 <= p.y && p.y < (2 * map_size_ + 2) / 3))
        return 1;
    // Player 2's base cells are on the top and bottom borders.
    else if ((p.y == -1 || p.y == map_size_) &&
        (map_size_ / 3 <= p.x && p.x < (2 * map_size_ + 2) / 3))
        return 2;
    else
        return -1;
}

Cell& GameState::cell(position p)
{
    return map_[map_index(p)];
}

int GameState::map_index(position p) const
{
    return p.x * map_size_ + p.y;
}

bool GameState::in_bounds(position p) const
{
    return 0 <= p.x && p.x < map_size_ &&
        0 <= p.y && p.y < map_size_;
}
