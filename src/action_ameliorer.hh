#ifndef ACTION_AMELIORER_HH
#define ACTION_AMELIORER_HH

#include <rules/action.hh>

#include "actions.hh"
#include "game_state.hh"
#include "constant.hh"

class ActionAmeliorer : public rules::Action<GameState>
{
public:
    ActionAmeliorer(position position, int player_id) : position_(position), player_id_(player_id) {}
    ActionAmeliorer() {} // for register_action()

    virtual int check(const GameState* st) const;
    virtual void apply_on(GameState* st) const;

    virtual void handle_buffer(utils::Buffer& buf)
    {
        buf.handle(position_);
        buf.handle(player_id_);
    }

    uint32_t player_id() const { return player_id_; };
    uint32_t id() const { return ID_ACTION_AMELIORER; }

private:
    position position_;
    int player_id_;
};

#endif // !ACTION_AMELIORER_HH
