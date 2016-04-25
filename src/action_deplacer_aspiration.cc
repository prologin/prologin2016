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

int ActionDeplacerAspiration::check(const GameState* st) const
{
    case_type source = st->get_cell_type(source_);
    case_type destination = st->get_cell_type(destination_);
    if (source == case_type::INTERDIT || destination == case_type::INTERDIT)
        return POSITION_INVALIDE;
    if (source_ == destination_)
        return POSITION_INVALIDE; // We may want to add a new enum field
    if (source != case_type::BASE || destination != case_type::BASE)
        return PAS_DANS_BASE;
    if (st->get_cell_owner(source_) != static_cast<unsigned>(player_id_) ||
        st->get_cell_owner(destination_) != static_cast<unsigned>(player_id_))
        return PAS_DANS_BASE;
    if (st->get_vacuum(source_) <= 0)
        return PE_INSUFFISANTS;
    if (st->get_vacuum(destination_) >= LIMITE_ASPIRATION)
        return LIMITE_ASPIRATION_ATTEINTE;
    if (st->get_vacuum_moved() &&
        st->get_action_points() < COUT_MODIFICATION_ASPIRATION)
        return PA_INSUFFISANTS;
    return OK;
}

void ActionDeplacerAspiration::apply_on(GameState* st) const
{
    if (st->get_vacuum_moved())
        st->decrease_action_points(COUT_MODIFICATION_ASPIRATION);
    else
        st->set_vacuum_moved(true);
    st->decrement_vacuum(source_);
    st->increment_vacuum(destination_);
    st->hist_add_move_vacuum(source_, destination_, player_id_);
    st->reset_board_distances();
}
