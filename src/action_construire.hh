#ifndef ACTION_CONSTRUIRE_HH
#define ACTION_CONSTRUIRE_HH

#include <rules/action.hh>

#include "actions.hh"
#include "game_state.hh"
#include "constant.hh"

class ActionConstruire : public rules::Action<GameState>
{
public:
    ActionConstruire(position position, int player_id) : position_(position), player_id_(player_id) {}
    ActionConstruire() {} // for register_action()

    int check(const GameState* st) const override;
    void apply_on(GameState* st) const override;

    void handle_buffer(utils::Buffer& buf) override
    {
        buf.handle(position_);
        buf.handle(player_id_);
    }

    uint32_t player_id() const override { return player_id_; };
    uint32_t id() const override { return ID_ACTION_CONSTRUIRE; }

private:
    position position_;
    int player_id_;
};

#endif // !ACTION_CONSTRUIRE_HH
