/*
** This file is part of Prologin2016, a rules library for stechec2.
**
** Copyright (c) 2016 Association Prologin <info@prologin.org>
**
** Prologin2016 is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Prologin2016 is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Prologin2016.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAME_STATE_HH
#define GAME_STATE_HH

#include <unordered_map>
#include <rules/game-state.hh>
#include <rules/player.hh>
#include "constant.hh"

/// Information about a player; encapsulate its rules::Player_sptr
class PlayerInfo
{
    public:
        /// Constructor from the rules::Player_sptr to encapsulate
        PlayerInfo(rules::Player_sptr player);

        /// Increase the amount of collected plasma (thus the score)
        void collect_plasma(double plasma);

        /// Get this player's score (floor of total collected plasma)
        int get_score() const { return player_->score; }

        /// Get total collected plasma by this player
        double get_collected_plasma() const { return collected_plasma_; }

    private:
        rules::Player_sptr player_; ///< Encapsulated stechec implementation
        double collected_plasma_; ///< Total collected plasma by this player
};

struct Cell
{
    case_type type;

    // Plasma amount on this cell (>= 0).
    double plasma;

    // ID of either:
    // - the owner of a BASE;
    // - the player who built a TUYAU;
    // - the one who upgraded one to a SUPER_TUYAU.
    // In the last two cases, this information has no effect on gameplay,
    // though it might be useful to AI programmers.
    // 0 if not applicable.
    unsigned owner;
};

namespace std
{
    template<> struct hash<position>
    {
        size_t operator()(const position& pos) const
        {
            return TAILLE_TERRAIN * pos.y + pos.x;
        }
    };
}

inline bool operator==(const position& p1, const position& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

class GameState : public rules::GameState
{
    public:
        // The input stream consists of simply five integers for every pulsar:
        // x y periode puissance plasma_total
        GameState(std::istream&, rules::Players_sptr players);
        rules::GameState* copy() const final override;

        /// Get the type of a case at a given position
        /// Return 'INTERDIT' if the position is invalid
        case_type get_case_type(position) const;

        /// Return pulsar info at a given position (it must contain a pulsar).
        pulsar get_pulsar(position) const;

        void increment_turn() { turn_++; }
        unsigned get_turn() const { return turn_; }

        unsigned opponent(unsigned player) const
        { return (player_ids_[0] == player) ? player_ids_[1] : player_ids_[0]; }

        unsigned get_action_points() const { return action_points_; }
        void decrease_action_points(unsigned delta);
        void reset_action_points();

        bool get_displaced_vacuum() const { return displaced_vacuum_; }
        void set_displaced_vacuum(bool);

        unsigned get_vacuum(position) const;
        void decrement_vacuum(position);
        void increment_vacuum(position);

        void build_pipe(position, unsigned);
        void upgrade_pipe(position, unsigned);
        void destroy_pipe(position);
        void clear_rubble(position);

        double get_plasma(position) const;
        void clear_plasma(position);

        // Should be called on a TUYAU, a SUPER_TUYAU or a BASE. In the last case,
        // the owner collects the plasma.
        void increase_plasma(position, double);

        /// Get a player's score. The id must be valid.
        int get_score(unsigned player_id) const;

        /// Get a player's total collected plasma. The id must be valid
        double get_collected_plasma(unsigned player_id) const;

    private:
        // Must be within bounds.
        Cell& cell(position);
        const Cell& cell(position) const;
        int board_index(position) const;
        bool in_bounds(position) const;

        const unsigned& vacuum_at(position) const;
        unsigned& vacuum_at(position);

        rules::Players_sptr players_;
        std::array<unsigned, 2> player_ids_;
        std::unordered_map<unsigned, PlayerInfo> player_info_;

        std::unordered_map<position, pulsar> pulsars_;
        std::array<Cell, TAILLE_TERRAIN * TAILLE_TERRAIN> board_;
        unsigned turn_;
        unsigned action_points_;

        // True if vacuum was displaced this turn
        bool displaced_vacuum_;

        // Base vacuum on four sides (Top, Bottom, Left, Right)
        std::array<std::array<unsigned, LONGUEUR_BASE>, 4> vacuums_;
};

#endif /* !GAME_STATE_HH */

