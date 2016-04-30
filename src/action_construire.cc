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

#include "actions.hh"

int ActionConstruire::check(const GameState* st) const
{
    case_type ct = st->get_cell_type(position_);
    if (ct == case_type::INTERDIT)
        return POSITION_INVALIDE;
    if (ct != case_type::VIDE)
        return CONSTRUCTION_IMPOSSIBLE;
    if (st->get_action_points() < COUT_CONSTRUCTION)
        return PA_INSUFFISANTS;
    return OK;
}

void ActionConstruire::apply_on(GameState* st) const
{
    st->decrease_action_points(COUT_CONSTRUCTION);
    st->build_pipe(position_, player_id_);
    st->hist_add_build(position_, player_id_);
    st->reset_board_distances();
}
