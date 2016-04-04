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

#include "game_state.hh"

#include <cmath>

PlayerInfo::PlayerInfo(rules::Player_sptr player)
    : player_(std::move(player))
    , collected_plasma_(0)
{
    assert(player_);
    player_->score = 0;
}

void PlayerInfo::collect_plasma(double plasma)
{
    assert(plasma > 0);
    collected_plasma_ += plasma;
    player_->score = std::floor(collected_plasma_);
}

GameState::GameState(std::istream& board_stream, rules::Players_sptr players)
    : rules::GameState()
    , players_(players)
    , turn_(0)
    , action_points_(NB_POINTS_ACTION)
    , vacuum_moved_(false)
{
    unsigned pi = 0;
    for (auto& p : players_->players)
    {
        if (p->type == rules::PLAYER)
        {
            player_info_.insert({p->id, p});
            player_ids_[pi++] = p->id;
        }
    }

    board_.fill({case_type::VIDE, 0, 0});
    const int N = TAILLE_TERRAIN;
    for (int i = 0 ; i < N ; i++)
    {
        position wall[] = {{i, 0}, {i, N-1}, {0, i}, {N-1, i}};
        for (unsigned j = 0 ; j < 4 ; j++)
        {
            if (N / 3 <= i && i < N - N / 3)
                cell(wall[j]) = {case_type::BASE, 0, player_ids_[j / 2]};
            else
                cell(wall[j]) = {case_type::INTERDIT, 0, 0};
        }
    }

    for (auto& vacuum : vacuums_)
        vacuum.fill(1);

    while (board_stream >> std::ws, !board_stream.eof())
    {
        position pos;
        pulsar pr;

        board_stream >> pos.x >> pos.y >>
            pr.periode >> pr.puissance >> pr.plasma_total;

        cell(pos).type = case_type::PULSAR;

        pulsars_.emplace(pos, pr);
    }
}

rules::GameState* GameState::copy() const
{
    return new GameState(*this);
}

case_type GameState::get_case_type(position pos) const
{
    if (in_bounds(pos))
        return cell(pos).type;
    else
        return case_type::INTERDIT;
}

pulsar GameState::get_pulsar(position pos) const
{
    return pulsars_.at(pos);
}

std::vector<position> GameState::pulsars_list() const
{
    std::vector<position> keys;
    keys.reserve(pulsars_.size());
    for (const auto& pulsar : pulsars_)
        keys.push_back(pulsar.first);
    return keys;
}

std::vector<position> GameState::bases_list(unsigned player_id) const
{
    std::vector<position> bases;
    bases.resize(TAILLE_TERRAIN * 2 / 3);
    for (unsigned side = 0 ; side < 4 ; ++side)
    {
        if (player_ids_[side / 2] != player_id) // This is not our base
            continue;
        for (int i = 0; i < TAILLE_TERRAIN; ++i)
        {
            if (TAILLE_TERRAIN / 3 <= i && i < 2 * TAILLE_TERRAIN / 3)
            {
                position wall[] = {{i, 0}, {i, TAILLE_TERRAIN - 1},
                    {0, i}, {TAILLE_TERRAIN - 1, i}};
                bases.push_back(wall[side]);
            }
        }
    }
    return bases;
}

void GameState::decrease_action_points(unsigned delta)
{
    action_points_ -= delta;
}

void GameState::reset_action_points()
{
    action_points_ = NB_POINTS_ACTION;
}

void GameState::set_vacuum_moved(bool b)
{
    vacuum_moved_ = b;
}

unsigned GameState::get_vacuum(position p) const
{
    return vacuum_at(p);
}

void GameState::increment_vacuum(position p)
{
    vacuum_at(p)++;
}

void GameState::decrement_vacuum(position p)
{
    vacuum_at(p)--;
}

void GameState::build_pipe(position p, unsigned player)
{
    Cell& c = cell(p);
    assert(c.type == VIDE);
    c.type = TUYAU;
    c.owner = player;
}

void GameState::upgrade_pipe(position p, unsigned player)
{
    Cell& c = cell(p);
    assert(c.type == TUYAU);
    c.type = SUPER_TUYAU;
    c.owner = player;
}

void GameState::destroy_pipe(position p)
{
    Cell& c = cell(p);
    assert(c.type == TUYAU || c.type == SUPER_TUYAU);
    assert(c.plasma == 0);
    c.type = DEBRIS;
    c.owner = 0;
}

void GameState::clear_rubble(position p)
{
    Cell& c = cell(p);
    assert(c.type == DEBRIS);
    c.type = VIDE;
}

void GameState::hist_add_build(position p, unsigned player)
{
    player_info_.at(player).add_action({BUILD, p});
}

void GameState::hist_add_upgrade(position p, unsigned player)
{
    player_info_.at(player).add_action({UPGRADE, p});
}

void GameState::hist_add_destroy(position p, unsigned player)
{
    player_info_.at(player).add_action({DESTROY, p});
}

void GameState::hist_add_clear(position p, unsigned player)
{
    player_info_.at(player).add_action({CLEAR, p});
}

void GameState::hist_add_move_vacuum(position src, position dest, unsigned player)
{
    player_info_.at(player).add_action({DECR_VACUUM, src});
    player_info_.at(player).add_action({INCR_VACUUM, dest});
}

void GameState::reset_history(unsigned player)
{
    player_info_.at(player).reset_actions();
}

const std::vector<action>& GameState::get_history(unsigned player) const
{
    return player_info_.at(player).get_actions();
}

double GameState::get_plasma(position p) const
{
    return cell(p).plasma;
}

void GameState::increase_plasma(position p, double plasma)
{
    assert(plasma > 0);
    auto& c = cell(p);
    if (c.type == BASE)
        player_info_.at(c.owner).collect_plasma(plasma);
    else
    {
        assert(c.type == TUYAU || c.type == SUPER_TUYAU);
        cell(p).plasma += plasma;
    }
}

int GameState::get_score(unsigned player_id) const
{
    return player_info_.at(player_id).get_score();
}

double GameState::get_collected_plasma(unsigned player_id) const
{
    return player_info_.at(player_id).get_collected_plasma();
}

void GameState::clear_plasma(position p)
{
    cell(p).plasma = 0;
}

Cell& GameState::cell(position p)
{
    return board_[board_index(p)];
}

const Cell& GameState::cell(position p) const
{
    return board_[board_index(p)];
}

int GameState::board_index(position p) const
{
    assert(in_bounds(p));
    return p.x * TAILLE_TERRAIN + p.y;
}

bool GameState::in_bounds(position p) const
{
    return 0 <= p.x && p.x < TAILLE_TERRAIN &&
        0 <= p.y && p.y < TAILLE_TERRAIN;
}

const unsigned& GameState::vacuum_at(position p) const
{
    assert(cell(p).type == BASE);
    const int offset = TAILLE_TERRAIN / 3;
    if (p.y == 0)
        return vacuums_[0][p.x - offset];
    else if (p.y == TAILLE_TERRAIN - 1)
        return vacuums_[1][p.x - offset];
    else if (p.x == 0)
        return vacuums_[2][p.y - offset];
    else // (p.x == TAILLE_TERRAIN - 1)
        return vacuums_[3][p.y - offset];
}

unsigned& GameState::vacuum_at(position p)
{
    return const_cast<unsigned&>(static_cast<const GameState&>(*this).vacuum_at(p));
}
