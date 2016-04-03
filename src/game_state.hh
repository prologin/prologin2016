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

    // Owner of a base, or ID of the player who built a pipe. The latter has
    // no incidence on gameplay, though it might be useful to AI programmers.
    // -1 if not applicable.
    int owner;
};

class GameState : public rules::GameState
{
    public:
        // The input stream consists of simply five integers for every pulsar:
        // x y periode puissance plasma_total
        GameState(std::istream&, rules::Players_sptr players);
        rules::GameState* copy() const final override;

        void increment_turn() { turn_++; }
        int get_turn() const { return turn_; }
        rules::Player_sptr get_current_player() const;

        int get_action_points() const { return action_points_; }
        void decrease_action_points(int delta);
        void reset_action_points();

        // Player to which a base cell belongs to.
        // -1 if not a base cell.
        int base_cell(position) const;

    private:
        // Must be within bounds.
        Cell& cell(position);
        Cell cell(position) const;
        int map_index(position) const;
        bool in_bounds(position) const;

        std::vector<position> pulsars_pos_;
        std::vector<pulsar> pulsars_info_;
        std::vector<Cell> map_;
        int turn_;
        int action_points_;

        rules::Player_sptr players_[2];
};

#endif /* !GAME_STATE_HH */

