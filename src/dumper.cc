/*
** prologin2016 is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** prologin2016 is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with prologin2016.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstring>
#include <cstdlib>
#include <sstream>

#include "api.hh"
#include "rules.hh"
#include "constant.hh"
#include "game_state.hh"

/// Put some binary content as a JSON string in the output stream.
static void dump_binary(std::ostream& ss, const uint8_t bytes[], unsigned size)
{
    const char* hextable = "0123456789ABCDEF";

    ss << "\"";
    for (unsigned i = 0; i < size; ++i)
    {
        if (bytes[i] == '"')
            ss << "\\\"";
        else if (bytes[i] == '\\')
            ss << "\\\\";
        else if (0x20 <= bytes[i] && bytes[i] <= 0x7e)
            ss << (char) bytes[i];
        else
            ss << "\\u00"
               << hextable[bytes[i] >> 4]
               << hextable[bytes[i] & 0x0f];
    }
    ss << "\"";
}

/// Dump everything that describe players.
static void dump_players(std::ostream& ss, const GameState& st)
{
    const auto& players = st.get_player_info();
    bool is_first = true;

    ss << "{";
    for (const auto& player_info : players)
    {
        const auto& p = player_info.second;
        if (!is_first)
            ss << ", ";
        is_first = false;
        ss << "\"" << player_info.first << "\": {"
           << "\"name\": \"" << p.get_name() << "\","
           << "\"name\": ";
        dump_binary(
            ss,
            reinterpret_cast<const uint8_t *>(p.get_name().c_str()),
            p.get_name().size()
        );
        ss << ", "
           << "\"collected_plasma\": \"" << p.get_collected_plasma() << "\","
           << "\"score\": " << p.get_score()
           << "}";
    }
    ss << "}";
}

static void dump_map(std::ostream& ss, const GameState& st)
{
    bool is_first = true;
    ss << "[";
    for (int i = 0; i < TAILLE_TERRAIN; ++i)
        for (int j = 0; j < TAILLE_TERRAIN; ++j)
        {
            if (!is_first)
                ss << ", ";
            is_first = false;

            position p{i, j};
            ss << "{"
               << "\"type\": \"" << st.get_cell_type(p) << "\","
               << "\"owner\": " << st.get_cell_owner(p) << ","
               << "\"plasma\": " << st.get_plasma(p)
               << "\"vacuum\": " <<
                   (st.get_cell_type(p) == BASE ? 0 : st.get_vacuum(p))
               << "}";
        }
    ss << "]";
}

static void dump_stream(std::ostream& ss, const GameState& st)
{
    ss << "{";
    // - "turn": [current turn, number of turns]
    ss << "\"turn\": ["
        << st.get_turn() << ", "
        << NB_TOURS
        << "] ";

    ss << ", "
       << "\"players\": ";
    dump_players(ss, st);

    ss << ", "
        << "\"map\": ";
    dump_map(ss, st);

    ss << "}";
}

void Rules::dump_state(std::ostream& ss)
{
    dump_stream(ss, *api_->game_state());
}


// from api.cc
extern Api* api;

extern "C" const char* dump_state_json()
{
    // Warning: everytime this function is called, it invalidates the previous
    // return values by free-ing them.
    static std::string s;
    std::ostringstream ss;
    dump_stream(ss, *api->game_state());
    s = ss.str();
    return s.c_str();
}
