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

class PlayerInfo
{
    public:
        PlayerInfo(rules::Player_sptr& player);
        void collect_plasma(double);
        int get_score() const
        { return player_->score; }
        double get_collected_plasma() const
        { return collected_plasma_; }

    private:
        rules::Player_sptr player_;
        double collected_plasma_;
};

struct Cell
{
    case_type type;

    // Plasma amount on this cell (>= 0).
    double plasma;

    // Index of a pulsar in the pulsars_info_ list.
    // -1 if not a pulsar.
    int pulsar;

    // ID of either:
    // - the owner of a BASE;
    // - the player who built a TUYAU;
    // - the one who upgraded one to a SUPER_TUYAU.
    // In the last two cases, this information has no effect on gameplay,
    // though it might be useful to AI programmers.
    // 0 if not applicable.
    unsigned owner;
};

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

        void increment_turn() { turn_++; }
        int get_turn() const { return turn_; }

        unsigned me(unsigned player) const
        { return (p_[0] == player) ? p_[0] : p_[1]; }

        unsigned opponent(unsigned player) const
        { return (p_[0] == player) ? p_[1] : p_[0]; }

        int get_action_points() const { return action_points_; }
        void decrease_action_points(int delta);
        void reset_action_points();

        bool get_displaced_vacuum() const { return displaced_vacuum_; }
        void set_displaced_vacuum(bool);

        int get_vacuum(position) const;
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

        int get_score(unsigned player) const
        { return player_info_.at(player).get_score(); }

        double get_collected_plasma(unsigned player) const
        { return player_info_.at(player).get_collected_plasma(); }

    private:
        // Must be within bounds.
        Cell& cell(position);
        const Cell& cell(position) const;
        int board_index(position) const;
        bool in_bounds(position) const;

        const int& vacuum_at(position) const;
        int& vacuum_at(position);

        rules::Players_sptr players_;
        std::array<unsigned, 2> p_;
        std::unordered_map<unsigned, PlayerInfo> player_info_;

        std::vector<position> pulsars_pos_;
        std::vector<pulsar> pulsars_info_;
        std::array<Cell, TAILLE_TERRAIN * TAILLE_TERRAIN> board_;
        int turn_;
        int action_points_;

        // True if vacuum was displaced this turn
        bool displaced_vacuum_;

        // Base vacuum on four sides (Top, Bottom, Left, Right)
        std::array<std::array<int, LONGUEUR_BASE>, 4> vacuums_;
};

#endif /* !GAME_STATE_HH */

