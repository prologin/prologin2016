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

#include "rules.hh"
#include "actions.hh"
#include <fstream>

Rules::Rules(const rules::Options opt)
    : TurnBasedRules(opt)
    , sandbox_(opt.time)
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

    auto game_state = std::make_unique<GameState>(ifs, opt.players);
    api_ = std::make_unique<Api>(std::move(game_state), opt.player);
    register_actions();
}

void Rules::register_actions()
{
    api_->actions()->register_action(ID_ACTION_CONSTRUIRE, []() {
        return std::make_unique<ActionConstruire>();
    });
    api_->actions()->register_action(ID_ACTION_AMELIORER, []() {
        return std::make_unique<ActionAmeliorer>();
    });
    api_->actions()->register_action(ID_ACTION_DETRUIRE, []() {
        return std::make_unique<ActionDetruire>();
    });
    api_->actions()->register_action(ID_ACTION_DEPLACER_ASPIRATION, []() {
        return std::make_unique<ActionDeplacerAspiration>();
    });
    api_->actions()->register_action(ID_ACTION_DEBLAYER, []() {
        return std::make_unique<ActionDeblayer>();
    });
}

rules::Actions* Rules::get_actions()
{
    return api_->actions();
}

void Rules::apply_action(const rules::IAction& action)
{
    // When receiving an action, the API should have already checked that it
    // is valid. We recheck that for the current gamestate here to avoid weird
    // client/server desynchronizations and make sure the gamestate is always
    // consistent across the clients and the server.

    int err = api_->game_state_check(action);
    if (err)
        FATAL("Synchronization error: received action %d from player %d, but "
              "check() on current gamestate returned %d.",
              action.id(), action.player_id(), err);
    api_->game_state_apply(action);
}

void Rules::at_player_start(rules::ClientMessenger_sptr)
{
    try
    {
        sandbox_.execute(champion_partie_init_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_start: timeout");
    }
}

void Rules::at_spectator_start(rules::ClientMessenger_sptr)
{
    champion_partie_init_();
}

void Rules::at_player_end(rules::ClientMessenger_sptr)
{
    try
    {
        sandbox_.execute(champion_partie_fin_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_end: timeout");
    }
}

void Rules::at_spectator_end(rules::ClientMessenger_sptr)
{
    champion_partie_fin_();
}

void Rules::player_turn()
{
    try
    {
        sandbox_.execute(champion_jouer_tour_);
    }
    catch (utils::SandboxTimeout&)
    {
        FATAL("player_turn: timeout");
    }
}

void Rules::spectator_turn()
{
    champion_jouer_tour_();
}

void Rules::start_of_player_turn(uint32_t player_id)
{
    api_->game_state().reset_action_points();
    api_->game_state().increment_turn();
    api_->game_state().set_vacuum_moved(false);
    api_->game_state().reset_history(player_id);
}

void Rules::end_of_player_turn(uint32_t /* player_id */)
{
    api_->game_state().move_plasma();
    api_->game_state().emit_plasma();

    // Clear the previous game states at the end of each turn (half-round)
    // We need the previous game states only for undo and history, therefore
    // old states are not needed anymore after the turn ends.
    api_->clear_old_game_states();
}

bool Rules::is_finished()
{
    return api_->game_state().get_turn() >= NB_TOURS;
}
