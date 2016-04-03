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
