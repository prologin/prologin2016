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

int ActionDetruire::check(const GameState* st) const
{
    case_type ct = st->get_cell_type(position_);
    if (ct == case_type::INTERDIT)
        return POSITION_INVALIDE;
    if (ct != case_type::TUYAU && ct != case_type::SUPER_TUYAU)
        return DESTRUCTION_IMPOSSIBLE;
    if (st->get_plasma(position_) > 0)
        return DESTRUCTION_IMPOSSIBLE; // We could have a better enum
    unsigned points = st->get_action_points();
    if (points < COUT_DESTRUCTION_TUYAU)
        return PA_INSUFFISANTS;
    if (ct == case_type::SUPER_TUYAU && points < COUT_DESTRUCTION_SUPER_TUYAU)
        return PA_INSUFFISANTS;
    return OK;
}

void ActionDetruire::apply_on(GameState* st) const
{
    if (st->get_cell_type(position_) == case_type::SUPER_TUYAU)
        st->decrease_action_points(COUT_DESTRUCTION_SUPER_TUYAU);
    else
        st->decrease_action_points(COUT_DESTRUCTION_TUYAU);
    st->destroy_pipe(position_);
    st->hist_add_destroy(position_, player_id_);
    st->reset_board_distances();
}
