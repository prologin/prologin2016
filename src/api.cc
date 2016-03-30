/*
** Stechec project is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** The complete GNU General Public Licence Notice can be found as the
** `NOTICE' file in the root directory.
**
** Copyright (C) 2016 Prologin
*/

#include <stdlib.h>

#include "api.hh"
#include "actions.hh"

// global used in interface.cc
Api* api;

Api::Api(GameState* game_state, rules::Player_sptr player)
    : game_state_(game_state),
      player_(player)
{
    api = this;
}


///
// Construit un tuyau sur une case donnée
//
erreur Api::construire(position position)
{
    rules::IAction_sptr action(new ActionConstruire(position, player_->id));

    erreur err;
    if ((err = static_cast<erreur>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

///
// Améliore un tuyau en super-tuyau.
//
erreur Api::ameliorer(position position)
{
    rules::IAction_sptr action(new ActionAmeliorer(position, player_->id));

    erreur err;
    if ((err = static_cast<erreur>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

///
// Détruit un tuyau sur une case donnée.
//
erreur Api::detruire(position position)
{
    rules::IAction_sptr action(new ActionDetruire(position, player_->id));

    erreur err;
    if ((err = static_cast<erreur>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

///
// Déplace un point d'aspiration d'un point de la base vers l'autre.
//
erreur Api::deplacer_aspiration(position source, position destination)
{
    rules::IAction_sptr action(new ActionDeplacerAspiration(source, destination, player_->id));

    erreur err;
    if ((err = static_cast<erreur>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

///
// Renvoie la liste de tous les pulsars présents.
//
std::vector<position> Api::liste_pulsars()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases contenant du plasma.
//
std::vector<position> Api::liste_plasmas()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases contenant un tuyau (ou super-tuyau).
//
std::vector<position> Api::liste_tuyaux()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases contenant un super-tuyau.
//
std::vector<position> Api::liste_super_tuyaux()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases sous l'effet de radiations.
//
std::vector<position> Api::liste_radiation()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases contenant un tuyau (ou super-tuyau) en cours de destruction.
//
std::vector<position> Api::liste_destruction_tuyaux()
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement si la case contient un pulsar.
//
bool Api::est_pulsar(position position)
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement si la case contient un simple tuyau.
//
bool Api::est_tuyau(position position)
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement si la case contient un super tuyau.
//
bool Api::est_super_tuyau(position position)
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement si la case contient un tuyau en cours de destruction.
//
bool Api::est_en_destruction(position position)
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement si la case est sous l'effet de radiations.
//
bool Api::est_en_radiation(position position)
{
  // TODO
  abort();
}

///
// Renvoie vrai si et seulement s'il est possible de construire sur cette case.
//
bool Api::est_libre(position position)
{
  // TODO
  abort();
}

///
// Renvoie le temps qu'il reste avant la destruction d'une case, et -1 si elle n'est pas en cours de destruction.
//
int Api::temps_destruction(position position)
{
  // TODO
  abort();
}

///
// Renvoie le temps qu'il reste avant la dissipation des radiations sur une case, et -1 si elle n'en subit pas.
//
int Api::temps_radiation(position position)
{
  // TODO
  abort();
}

///
// Renvoie les informations propres au pulsar à la position donnée.
//
pulsar Api::info_pulsar(position position)
{
  // TODO
  abort();
}

///
// Renvoie la quantité de plasma sur une case donnée (0 s'il n'y a pas de plasma).
//
int Api::charges_presentes(position position)
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases composant votre base.
//
std::vector<position> Api::ma_base()
{
  // TODO
  abort();
}

///
// Renvoie la liste des cases composant la base de votre ennemi.
//
std::vector<position> Api::base_ennemie()
{
  // TODO
  abort();
}

///
// Renvoie la puissance d'aspiration d'une case de base donnée.
//
std::vector<position> Api::energie_aspiration(position position)
{
  // TODO
  abort();
}

///
// Renvoie la liste des priorités des directions, dans l'ordre décroissant de priorité, au tour donné.
//
std::vector<direction> Api::priorite_directionnelle(position position, int tour)
{
  // TODO
  abort();
}

///
// Renvoie pour une case donnée la direction de son aspiration (qu'elle contienne un plasma ou non), ou "aucune" si la case n'est pas une case contenant un tuyau ou si elle n'est reliée à aucune base.
//
direction Api::aspiration(position position)
{
  // TODO
  abort();
}

///
// Renvoie la liste des tuyaux construits par votre adversaire au dernier tour.
//
std::vector<position> Api::hist_tuyaux_construits()
{
  // TODO
  abort();
}

///
// Renvoie la liste des tuyaux détruits par votre adversaire au dernier tour.
//
std::vector<position> Api::hist_tuyaux_detruits()
{
  // TODO
  abort();
}

///
// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier tour.
//
std::vector<position> Api::hist_tuyaux_ameliores()
{
  // TODO
  abort();
}

///
// Renvoie la liste des différences d'énergie d'aspiration dans la base de votre adversaire au dernier tour.
//
std::vector<position> Api::hist_base_modification()
{
  // TODO
  abort();
}

///
// Renvoie votre numéro de joueur.
//
int Api::moi()
{
  // TODO
  abort();
}

///
// Renvoie le numéro de votre adversaire.
//
int Api::adversaire()
{
  // TODO
  abort();
}

///
// Indique votre nombre de points d'actions restants pour ce tour-ci.
//
int Api::points_action()
{
  // TODO
  abort();
}

///
// Renvoie le score du joueur désigné par le numéro ``id_joueur``.
//
int Api::score(int id_joueur)
{
  // TODO
  abort();
}

///
// Renvoie le numéro du tour actuel.
//
int Api::tour_actuel()
{
  // TODO
  abort();
}

///
// Annule la dernière action. Renvoie ``false`` quand il n'y a pas d'action à annuler ce tour-ci.
//
bool Api::annuler()
{
  // TODO
  abort();
}

///
// Affiche le contenu d'une valeur de type erreur
//

///
// Affiche le contenu d'une valeur de type direction
//

///
// Affiche le contenu d'une valeur de type position
//

///
// Affiche le contenu d'une valeur de type pulsar
//


