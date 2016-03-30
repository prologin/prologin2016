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
    else
        champion_dll_ = nullptr;

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

bool Rules::is_finished()
{
    // FIXME
    return true;
}
