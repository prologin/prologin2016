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

PlayerInfo::PlayerInfo(rules::Player_sptr player)
    : player_(std::move(player))
    , collected_plasma_(0)
{
    assert(player_);
    player_->score = 0;
}

void PlayerInfo::collect_plasma(double plasma)
{
    collected_plasma_ += plasma;
    player_->score = collected_plasma_;
}

GameState::GameState(std::istream& board_stream, rules::Players_sptr players)
    : rules::GameState()
    , players_(players)
    , turn_(0)
    , action_points_(NB_POINTS_ACTION)
    , displaced_vacuum_(false)
{
    unsigned pi = 0;
    for (auto& p : players_->players)
    {
        if (p->type == rules::PLAYER)
        {
            player_info_.insert({p->id, p});
            p_[pi++] = p->id;
        }
    }

    board_.fill({case_type::VIDE, 0, -1, 0});
    const int N = TAILLE_TERRAIN;
    for (int i = 0 ; i < N ; i++)
    {
        position wall[] = {{i, 0}, {i, N-1}, {0, i}, {N-1, i}};
        for (unsigned j = 0 ; j < 4 ; j++)
        {
            if (N / 3 <= i && i < N - N / 3)
                cell(wall[j]) = {case_type::BASE, 0, -1, p_[j / 2]};
            else
                cell(wall[j]) = {case_type::INTERDIT, 0, -1, 0};
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

case_type GameState::get_case_type(position pos) const
{
    if (in_bounds(pos))
        return cell(pos).type;
    else
        return case_type::INTERDIT;
}

void GameState::decrease_action_points(unsigned delta)
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

double GameState::get_plasma(position p) const
{
    return cell(p).plasma;
}

void GameState::increase_plasma(position p, double plasma)
{
    auto& c = cell(p);
    if (c.type == BASE)
        player_info_.at(c.owner).collect_plasma(plasma);
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
