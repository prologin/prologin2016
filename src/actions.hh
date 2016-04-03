/*
** This file is part of Prologin2016, a rules library for stechec2, a rules library for stechec2.
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

#ifndef ACTIONS_HH
#define ACTIONS_HH

enum action_id {
    ID_ACTION_CONSTRUIRE,
    ID_ACTION_AMELIORER,
    ID_ACTION_DETRUIRE,
    ID_ACTION_DEPLACER_ASPIRATION,
    ID_ACTION_DEBLAYER
};

#include "action_construire.hh"
#include "action_ameliorer.hh"
#include "action_detruire.hh"
#include "action_deplacer_aspiration.hh"
#include "action_deblayer.hh"

#endif // !ACTIONS_HH
