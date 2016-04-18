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

#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include "api.hh"
#include "rules.hh"
#include "constant.hh"
#include "game_state.hh"

/// Decodes a UTF-8 string to a list of 32 bit unicode codepoints. Ignores
/// erroneous characters.
static std::u32string utf8_decode(const std::string& s)
{
    std::u32string ret;
    size_t i = 0;
    size_t size = s.size();

    while (i < size)
    {
        if ((s[i] & 0x80) == 0)
        {
            ret.push_back(s[i++]);
        }
        else if ((s[i] & 0xe0) == 0xc0 && (i + 1) < size &&
                 (s[i + 1] & 0xc0) == 0x80)
        {
            ret.push_back(((s[i] & 0x1f) << 6) | (s[i + 1] & 0x3f));
            i += 2;
        }
        else if ((s[i] & 0xf0) == 0xe0 && (i + 2) < size &&
                 (s[i + 1] & 0xc0) == 0x80 && (s[i + 2] & 0xc0) == 0x80)
        {
            ret.push_back(((s[i] & 0x0f) << 12) | ((s[i + 1] & 0x3f) << 6) |
                          (s[i + 2] & 0x3f));
            i += 3;
        }
        else if ((s[i] & 0xf8) == 0xf0 && (i + 3) < size &&
                 (s[i + 1] & 0xc0) == 0x80 && (s[i + 2] & 0xc0) == 0x80 &&
                 (s[i + 1] & 0xc0) == 0x80)
        {
            ret.push_back(((s[i] & 0x07) << 18) | ((s[i + 1] & 0x3f) << 12) |
                          ((s[i + 2] & 0x3f) << 6) | (s[i + 3] & 0x3f));
            i += 4;
        }
        else
        {
            i++;
        }
    }

    return ret;
}

/// Dump a JSON-escaped UTF-8 string
static void dump_string(std::ostream& ss, const std::string& s)
{
    /*
     * RFC4627, 2.5:
     * All Unicode characters may be placed within the quotation marks except
     * for the characters that must be escaped: quotation mark, reverse solidus,
     * and the control characters (U+0000 through U+001F).
     */
    std::ios state(nullptr);
    state.copyfmt(ss);
    std::u32string utf32 = utf8_decode(s);
    ss << "\"";
    for (char32_t& c : utf32)
    {
        if (c == u'"')
        {
            ss << "\\\"";
        }
        else if (c == u'\\')
        {
            ss << "\\\\";
        }
        else if (u'\u0020' <= c && c <= u'\u007E')
        {
            // printable ASCII
            ss << static_cast<char>(c);
        }
        else if (c > u'\uFFFF')
        {
            // surrogate pair
            // http://unicode.org/faq/utf_bom.html#utf16-2
            const unsigned s = c - 0x010000;
            const unsigned lead = (s >> 10) + 0xD800;
            const unsigned trail = (s & 0x3FF) + 0xDC00;
            ss << "\\u" << std::hex << std::setfill('0') << std::setw(4)
               << lead;
            ss.copyfmt(state);
            ss << "\\u" << std::hex << std::setfill('0') << std::setw(4)
               << trail;
            ss.copyfmt(state);
        }
        else
        {
            ss << "\\u" << std::hex << std::setfill('0') << std::setw(4) << c;
            ss.copyfmt(state);
        }
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
           << "\"name\": ";
        dump_string(ss, p.get_name());
        ss << ", \"collected_plasma\": \"" << p.get_collected_plasma() << "\""
           << ", \"score\": " << p.get_score() << "}";
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
               << "\"type\": \"" << st.get_cell_type(p) << "\""
               << ", \"owner\": " << st.get_cell_owner(p)
               << ", \"plasma\": " << st.get_plasma(p) << ", \"vacuum\": "
               << (st.get_cell_type(p) == BASE ? st.get_vacuum(p) : 0) << "}";
        }
    ss << "]";
}

static void dump_pulsars(std::ostream& ss, const GameState& st)
{
    bool is_first = true;
    ss << "[";
    for (position p : st.pulsars_list())
    {
        if (!is_first)
            ss << ", ";
        ss << "{";
        is_first = false;
        pulsar_info pr = st.get_pulsar(p);
        ss << "\"x\": " << p.x;
        ss << ", \"y\": " << p.y;
        ss << ", \"period\": " << pr.periode;
        ss << ", \"power\": " << pr.puissance;
        ss << ", \"n_pulses\": " << pr.nombre_pulsations;
        ss << "}";
    }
    ss << "]";
}

static void dump_stream(std::ostream& ss, const GameState& st)
{
    ss << "{";
    // - "turn": [current turn, number of turns]
    ss << "\"turn\": [" << st.get_turn() << ", " << NB_TOURS << "] ";

    ss << ", \"players\": ";
    dump_players(ss, st);

    ss << ", \"map\": ";
    dump_map(ss, st);

    ss << ", \"pulsars\": ";
    dump_pulsars(ss, st);

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
