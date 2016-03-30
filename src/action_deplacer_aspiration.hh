#ifndef ACTION_DEPLACER_ASPIRATION_HH
#define ACTION_DEPLACER_ASPIRATION_HH

#include <rules/action.hh>

#include "actions.hh"
#include "game_state.hh"
#include "constant.hh"

class ActionDeplacerAspiration : public rules::Action<GameState>
{
public:
    ActionDeplacerAspiration(position source, position destination, int player_id) : source_(source), destination_(destination), player_id_(player_id) {}
    ActionDeplacerAspiration() {} // for register_action()

    virtual int check(const GameState* st) const;
    virtual void apply_on(GameState* st) const;

    virtual void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(source_);
        buf.handle(destination_);
        buf.handle(player_id_);
    }

    uint32_t player_id() const { return player_id_; };
    uint32_t id() const { return ID_ACTION_DEPLACER_ASPIRATION; }

private:
    position source_;
    position destination_;
    int player_id_;
};

#endif // !ACTION_DEPLACER_ASPIRATION_HH
