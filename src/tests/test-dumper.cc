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

#include <sstream>

#include "../rules.hh"

#include "test-helpers.hh"

// Checks that unicode strings are properly being dumped.
TEST_F(RulesTest, DumpUnicodeString)
{
    std::ostringstream oss;
    std::string out;
    auto& player1_info =
        rules->get_game_state()->get_player_info().find(PLAYER_ID_1)->second;

    player1_info.set_name("\xc3\xa9l\xc3\xa9phant"); // éléphant
    oss.str("");
    rules->dump_state(oss);
    out = oss.str();
    EXPECT_NE(std::string::npos, out.find("\\u00e9l\\u00e9phant")) << out;

    player1_info.set_name("\xf0\x9f\x90\xae"); // U+1f42e COW FACE
    oss.str("");
    rules->dump_state(oss);
    out = oss.str();
    EXPECT_NE(std::string::npos, out.find("\\ud83d\\udc2e")) << out;
}
