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

#include "constant.hh"
#include <array>
#include <rules/game-state.hh>
#include <rules/player.hh>
#include <unordered_map>

#include "position.hh"

enum action_type
{
    BUILD,
    UPGRADE,
    DESTROY,
    CLEAR,
    INCR_VACUUM,
    DECR_VACUUM,
};

struct action
{
    action_type type;
    position pos;
};

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

    /// Get this player's name
    const std::string& get_name() const { return player_->name; }

    /// Set this player's name (for tests)
    void set_name(const std::string& name) const { player_->name = name; }

    /// Get total collected plasma by this player
    double get_collected_plasma() const { return collected_plasma_; }

    /// Get the list of actions taken by this player last turn
    const std::vector<action>& get_actions() const { return actions_; }

    /// Empty the list of actions at the start of a new turn
    void reset_actions() { actions_.clear(); }

    /// Register a new action
    void add_action(action action) { actions_.push_back(action); }

private:
    rules::Player_sptr player_;   ///< Encapsulated stechec implementation
    double collected_plasma_;     ///< Total collected plasma by this player
    std::vector<action> actions_; ///< Actions taken during last turn
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

template <typename T>
using matrix = std::array<T, TAILLE_TERRAIN * TAILLE_TERRAIN>;

class GameState : public rules::GameState
{
public:
    // The input stream consists of simply five integers for every pulsar:
    // x y periode puissance nombre_pulsations
    GameState(std::istream&, rules::Players_sptr players);
    rules::GameState* copy() const final override;

    const auto& get_player_info() const { return player_info_; }
    auto& get_player_info() { return player_info_; }

    // Decrease the amount of plasma of a player.
    void decrease_plasma(unsigned, double);

    /// Get the type of a case at a given position
    /// Return 'INTERDIT' if the position is invalid
    case_type get_cell_type(position) const;

    /// Get the "owner" of a case.
    unsigned get_cell_owner(position) const;

    /// Return pulsar info at a given position (it must contain a pulsar).
    pulsar_info get_pulsar(position) const;

    /// Return the position of all pulsars
    std::vector<position> pulsars_list() const;

    /// Return the position of all bases owned by a player
    std::vector<position> bases_list(unsigned player_id) const;

    void increment_turn() { turn_++; }
    unsigned get_turn() const { return turn_; }

    unsigned opponent(unsigned player) const
    {
        return (player_ids_[0] == player) ? player_ids_[1] : player_ids_[0];
    }

    unsigned get_action_points() const { return action_points_; }
    void decrease_action_points(unsigned delta);
    void reset_action_points();

    bool get_vacuum_moved() const { return vacuum_moved_; }
    void set_vacuum_moved(bool);

    /// Get the vacuum power of a given position, that must be a BASE
    unsigned get_vacuum(position) const;
    void decrement_vacuum(position);
    void increment_vacuum(position);

    void build_pipe(position, unsigned);
    void upgrade_pipe(position, unsigned);
    void destroy_pipe(position);
    void clear_rubble(position);

    // Update action history
    void hist_add_build(position, unsigned);
    void hist_add_upgrade(position, unsigned);
    void hist_add_destroy(position, unsigned);
    void hist_add_clear(position, unsigned);
    void hist_add_move_vacuum(position, position, unsigned);
    void reset_history(unsigned);

    const std::vector<action>& get_history(unsigned) const;

    double get_plasma(position) const;
    void clear_plasma(position);

    // Should be called on a TUYAU, a SUPER_TUYAU or a BASE. In the last case,
    // the owner collects the plasma.
    void increase_plasma(position, double);

    /// Get a player's score. The id must be valid.
    int get_score(unsigned player_id) const;

    /// Get a player's total collected plasma. The id must be valid.
    double get_collected_plasma(unsigned player_id) const;

    /// Get the shortest distances from pipes to cells.
    const matrix<int>& get_board_distances();

    /// Discard the distance array when an action modifies the board.
    void reset_board_distances();

    /// Get the positions to which the plasma at the given position will
    /// move next turn. Returns an empty vector if the position is not
    /// a pipe connected to a base.
    std::vector<position> direction_plasma(position);

    /// Move plasma at the end of a turn. Plasma that is not connected to
    /// any 'BASE' cell is lost.
    void move_plasma();

    /// Make pulsars produce plasma.
    void emit_plasma();

private:
    // Must be within bounds.
    Cell& cell(position);
    const Cell& cell(position) const;
    int board_index(position) const;
    bool in_bounds(position) const;

    const unsigned& vacuum_at(position) const;
    unsigned& vacuum_at(position);

    void compute_board_distances();

    std::array<unsigned, 2> player_ids_;
    std::unordered_map<unsigned, PlayerInfo> player_info_;

    std::unordered_map<position, pulsar_info> pulsars_;
    matrix<Cell> board_;
    unsigned turn_;
    unsigned action_points_;

    // True if vacuum was moved this turn
    bool vacuum_moved_;

    // Base vacuum on four sides (Top, Bottom, Left, Right)
    std::array<std::array<unsigned, LONGUEUR_BASE>, 4> vacuums_;

    // The lengths of the shortest paths from pipes to 'BASE' cells.
    // MAX_INT represents cells unreachable from bases.
    std::shared_ptr<matrix<int>> board_distances_;
};

#endif /* !GAME_STATE_HH */
