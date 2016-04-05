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

#include <fstream>
#include "rules.hh"
#include "actions.hh"

Rules::Rules(const rules::Options opt)
    : TurnBasedRules(opt), sandbox_(opt.time)
{
    if (!opt.champion_lib.empty())
    {
        champion_dll_ = std::make_unique<utils::DLL>(opt.champion_lib);
        champion_partie_init_ =
            champion_dll_->get<f_champion_partie_init>("partie_init");
        champion_jouer_tour_ =
            champion_dll_->get<f_champion_jouer_tour>("jouer_tour");
        champion_partie_fin_ =
            champion_dll_->get<f_champion_partie_fin>("partie_fin");
    }

    std::ifstream ifs(opt.map_file);
    if (!ifs.is_open())
        FATAL("Cannot open file: %s", opt.map_file.c_str());

    GameState* game_state = new GameState(ifs, opt.players);
    api_ = std::make_unique<Api>(game_state, opt.player);
    register_actions();
}

void Rules::register_actions()
{
    api_->actions()->register_action(
        ID_ACTION_CONSTRUIRE,
        []() -> rules::IAction* { return new ActionConstruire(); }
        );
    api_->actions()->register_action(
        ID_ACTION_AMELIORER,
        []() -> rules::IAction* { return new ActionAmeliorer(); }
        );
    api_->actions()->register_action(
        ID_ACTION_DETRUIRE,
        []() -> rules::IAction* { return new ActionDetruire(); }
        );
    api_->actions()->register_action(
        ID_ACTION_DEPLACER_ASPIRATION,
        []() -> rules::IAction* { return new ActionDeplacerAspiration(); }
        );
    api_->actions()->register_action(
        ID_ACTION_DEBLAYER,
        []() -> rules::IAction* { return new ActionDeblayer(); }
        );
}

rules::Actions* Rules::get_actions()
{
    return api_->actions();
}

void Rules::apply_action(const rules::IAction_sptr& action)
{
    // When receiving an action, the API should have already checked that it
    // is valid. We recheck that for the current gamestate here to avoid weird
    // client/server desynchronizations and make sure the gamestate is always
    // consistent across the clients and the server.

    int err = action->check(api_->game_state());
    if (err)
        FATAL("Synchronization error: received action %d from player %d, but "
              "check() on current gamestate returned %d.",
              action->id(), action->player_id(), err);
    api_->game_state_set(action->apply(api_->game_state()));
}

void Rules::at_start()
{
    // TODO
}

void Rules::at_player_start(rules::ClientMessenger_sptr)
{
    try {
        sandbox_.execute(champion_partie_init_);
    }
    catch (utils::SandboxTimeout)
    {
        FATAL("partie_init: timeout");
    }
}

void Rules::at_spectator_start(rules::ClientMessenger_sptr)
{
    champion_partie_init_();
}

void Rules::at_player_end(rules::ClientMessenger_sptr)
{
    try {
        sandbox_.execute(champion_partie_fin_);
    }
    catch (utils::SandboxTimeout)
    {
        FATAL("partie_fin: timeout");
    }
}

void Rules::at_spectator_end(rules::ClientMessenger_sptr)
{
    champion_partie_fin_();
}

void Rules::player_turn()
{
    sandbox_.execute(champion_jouer_tour_);
}

void Rules::spectator_turn()
{
    try {
        champion_jouer_tour_();
    }
    catch (utils::SandboxTimeout)
    {
        FATAL("partie_fin: timeout");
    }
}

void Rules::start_of_round()
{
    // TODO (print)
}

void Rules::end_of_round()
{
    // TODO
}

void Rules::start_of_player_turn(uint32_t player_id)
{
    // TODO (reset AP)
}

void Rules::end_of_player_turn(uint32_t player_id)
{
    // TODO

    // Clear the list of game states at the end of each turn (half-round)
    // We need the linked list of game states only for undo and history,
    // therefore old states are not needed anymore after the turn ends.
    api_->game_state()->clear_old_version();
}

void Rules::dump_state(std::ostream& out)
{
    // TODO
    // char* line = api_->get_dump();
    // out << line << std::endl;
    // free(line);
}

bool Rules::is_finished()
{
    // FIXME
    return true;
}
