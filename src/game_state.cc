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

GameState::GameState(std::istream& map_stream, rules::Players_sptr players)
    : rules::GameState(),
      map_(TAILLE_TERRAIN * TAILLE_TERRAIN, {case_type::VIDE, 0, -1, -1}),
      turn_(0),
      action_points_(NB_POINTS_ACTION),
      displaced_vacuum_(false),
      score_{0, 0}
{
    int p_count = 0;
    for (auto& p : players->players)
    {
        if (p->type == rules::PLAYER)
        {
            p->score = 0;
            players_[p_count++] = p;
        }
    }

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

    for (auto& vacuum : vacuums_)
        vacuum = std::vector<int>(N - 2 * (N / 3), 1);

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


void GameState::set_displaced_vacuum(bool b)
{
    displaced_vacuum_ = b;
}

int GameState::get_vacuum(position p) const
{
    return base_cell(p) == -1 ? -1 : vacuum_at(p);
}

void GameState::increment_vacuum(position p)
{
    vacuum_at(p)++;
}

void GameState::decrement_vacuum(position p)
{
    vacuum_at(p)--;
}

void GameState::build_pipe(position p)
{
    Cell& c = cell(p);
    assert(c.type == VIDE);
    c.type = TUYAU;
    c.owner = turn_ % 2;
}

void GameState::upgrade_pipe(position p)
{
    Cell& c = cell(p);
    assert(c.type == TUYAU);
    c.type = SUPER_TUYAU;
    c.owner = turn_ % 2;
}

void GameState::destroy_pipe(position p)
{
    Cell& c = cell(p);
    assert(c.type == TUYAU || c.type == SUPER_TUYAU);
    assert(c.plasma == 0);
    c.type = DEBRIS;
    c.owner = -1;
}

void GameState::clear_rubble(position p)
{
    Cell& c = cell(p);
    assert(c.type == DEBRIS);
    c.type = VIDE;
}

int GameState::base_cell(position p) const
{
    const Cell& c = cell(p);
    return (c.type == BASE) ? c.owner : -1;
}

double GameState::get_plasma(position p) const
{
    return cell(p).plasma;
}

void GameState::increase_plasma(position p, double plasma)
{
    auto& c = cell(p);
    if (c.type == BASE)
        score_[c.owner] += plasma;
    else
    {
        assert(c.type == TUYAU || c.type == SUPER_TUYAU);
        cell(p).plasma += plasma;
    }
}

void GameState::clear_plasma(position p)
{
    cell(p).plasma = 0;
}

double GameState::get_score_current_player() const
{
    return score_[turn_ % 2];
}

double GameState::get_score_opponent() const
{
    return score_[(1+turn_) % 2];
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
    assert(in_bounds(p));
    return p.x * TAILLE_TERRAIN + p.y;
}

bool GameState::in_bounds(position p) const
{
    return 0 <= p.x && p.x < TAILLE_TERRAIN &&
        0 <= p.y && p.y < TAILLE_TERRAIN;
}

const int& GameState::vacuum_at(position p) const
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

int& GameState::vacuum_at(position p)
{
    return const_cast<int&>(static_cast<const GameState&>(*this).vacuum_at(p));
}
