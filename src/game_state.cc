// FIXME License notice

#include "game_state.hh"

GameState::GameState(int map_size,
                     std::vector<position>& pulsars_pos,
                     std::vector<pulsar>& pulsars_info,
                     rules::Players_sptr players)
    : rules::GameState(),
      map_size_(map_size),
      pulsars_pos_(pulsars_pos),
      pulsars_info_(pulsars_info),
      map_(std::vector<Cell>(map_size * map_size, kEmptyCell)),
      turn_(0),
      players_(players)
{
    for (int i = 0 ; i < (int) pulsars_pos.size() ; i++)
    {
        auto& c = cell(pulsars_pos[i]);
        c.type_ = CellType::PULSAR;
        c.pulsar_ = i;
    }
}

GameState::GameState(const GameState& st)
    : rules::GameState(st)
    , players_(st.players_)
{
    // FIXME
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

GameState::~GameState()
{
    // FIXME
}

void GameState::next_turn()
{
    turn_++;
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

const Cell GameState::kEmptyCell =
{
  .type_ = CellType::EMPTY,
  .plasma_ = 0,
  .irradiation_time_ = -1,
  .destruction_time_ = -1,
  .pulsar_ = -1,
};
