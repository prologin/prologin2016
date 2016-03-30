// FIXME License notice

#ifndef GAME_STATE_HH
#define GAME_STATE_HH

#include <rules/game-state.hh>
#include <rules/player.hh>
#include "constant.hh"

struct Cell
{
    case_type type;

    // Plasma amount on this cell (>= 0).
    int plasma;

    // Index of a pulsar in the pulsars_info_ list.
    // -1 if not a pulsar.
    int pulsar;
};

class GameState : public rules::GameState
{
    public:
        GameState(int map_size,
                  std::vector<position>& pulsars_pos,
                  std::vector<pulsar>& pulsars_info,
                  rules::Players_sptr players);
        GameState(std::ifstream&, rules::Players_sptr players);
        rules::GameState* copy() const final;

        void increment_turn() { turn_++; }
        int get_turn() const { return turn_; }
        int get_current_player() const;

        // Player to which a base cell belongs to (should be out of bounds).
        // -1 if not a base cell.
        int base_cell(position) const;

    private:
        // Must be within bounds.
        Cell& cell(position);
        int map_index(position) const;
        bool in_bounds(position) const;

        int map_size_;
        std::vector<position> pulsars_pos_;
        std::vector<pulsar> pulsars_info_;
        std::vector<Cell> map_;
        int turn_;

        rules::Players_sptr players_;
};

#endif /* !GAME_STATE_HH */

