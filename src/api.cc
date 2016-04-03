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


/// Construit un tuyau sur une case donnée.
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

/// Améliore un tuyau en super-tuyau.
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

/// Détruit un tuyau sur une case donnée.
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

/// Déplace un point d'aspiration d'un point de la base vers l'autre.
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

/// Déblaye une case de débris.
erreur Api::deblayer(position position)
{
    rules::IAction_sptr action(new ActionDeblayer(position, player_->id));

    erreur err;
    if ((err = static_cast<erreur>(action->check(game_state_))) != OK)
        return err;

    actions_.add(action);
    game_state_set(action->apply(game_state_));
    return OK;
}

/// Renvoie le type d'une case donnée.
case_type Api::type_case(position position)
{
    // TODO
    abort();
}

/// Renvoie la liste de tous les pulsars présents.
std::vector<position> Api::liste_pulsars()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases contenant du plasma.
std::vector<position> Api::liste_plasmas()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases contenant un tuyau (ou super-tuyau).
std::vector<position> Api::liste_tuyaux()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases contenant un super-tuyau.
std::vector<position> Api::liste_super_tuyaux()
{
    // TODO
    abort();
}

/// Renvoie la liste des tuyaux détruits, non encore déblayés.
std::vector<position> Api::liste_tuyaux_detruits()
{
    // TODO
    abort();
}

/// Renvoie vrai si et seulement si la case contient un pulsar.
bool Api::est_pulsar(position position)
{
    // TODO
    abort();
}

/// Renvoie vrai si et seulement si la case contient un simple tuyau.
bool Api::est_tuyau(position position)
{
    // TODO
    abort();
}

/// Renvoie vrai si et seulement si la case contient un super tuyau.
bool Api::est_super_tuyau(position position)
{
    // TODO
    abort();
}

/// Renvoie vrai si et seulement si la case contient un tuyau détruit non encore déblayé
bool Api::est_detruit(position position)
{
    // TODO
    abort();
}

/// Renvoie vrai si et seulement s'il est possible de construire sur cette case.
bool Api::est_libre(position position)
{
    // TODO
    abort();
}

/// Renvoie les informations propres au pulsar à la position donnée.
pulsar Api::info_pulsar(position position)
{
    // TODO
    abort();
}

/// Renvoie la quantité de plasma sur une case donnée.
double Api::charges_presentes(position position)
{
    return game_state_->get_plasma(position);
}

/// Renvoie la liste des cases composant votre base.
std::vector<position> Api::ma_base()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases composant la base de votre ennemi.
std::vector<position> Api::base_ennemie()
{
    // TODO
    abort();
}

/// Renvoie la puissance d'aspiration d'une case de base donnée.
std::vector<position> Api::energie_aspiration(position position)
{
    // TODO
    abort();
}

/// Renvoie pour une case donnée la direction de son aspiration (qu'elle
/// contienne un plasma ou non), ou "aucune" si la case n'est pas une case
/// contenant un tuyau ou si elle n'est reliée à aucune base.
direction Api::aspiration(position position)
{
    // TODO
    abort();
}

/// Renvoie la valeur du coût de la prochaine modification de vos points d'aspiration.
int Api::cout_prochaine_modification_aspiration()
{
    if (game_state_->get_displaced_vacuum()) // Already used, full cost
        return COUT_MODIFICATION_ASPIRATION;
    else // The first one is free
        return 0;
}

/// Renvoie la liste des tuyaux construits par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_construits()
{
    // TODO
    abort();
}

/// Renvoie la liste des tuyaux détruits par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_detruits()
{
    // TODO
    abort();
}

/// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_ameliores()
{
    // TODO
    abort();
}

/// Renvoie la liste des débris déblayés par votre adversaire au dernier tour.
std::vector<position> Api::hist_debris_deblayes()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases de base de votre adversaire qui ont reçu un point d'aspiration (une même case peut apparaître plusieurs fois).
std::vector<position> Api::hist_points_aspiration_ajoutes()
{
    // TODO
    abort();
}

/// Renvoie la liste des cases de base de votre adversaire qui ont perdu un point d'aspiration (une même case peut apparaître plusieurs fois).
std::vector<position> Api::hist_points_aspiration_retires()
{
    // TODO
    abort();
}

/// Renvoie votre numéro de joueur.
int Api::moi()
{
    return game_state_->get_current_player()->id;
}

/// Renvoie le numéro de votre adversaire.
int Api::adversaire()
{
    // TODO
    abort();
}

/// Indique votre nombre de points d'actions restants pour ce tour-ci.
int Api::points_action()
{
    return game_state_->get_action_points();
}

/// Renvoie le score du joueur désigné par le numéro ``id_joueur``.
/// Renvoie -1 si ``id_joueur`` est invalide.
int Api::score(int id_joueur)
{
    if (id_joueur == moi())
        return game_state_->get_score_current_player();
    else if (id_joueur == adversaire())
        return game_state_->get_score_opponent();
    else
        return -1;
}

/// Renvoie le numéro du tour actuel.
int Api::tour_actuel()
{
    return game_state_->get_turn();
}

/// Annule la dernière action. Renvoie ``false`` quand il n'y a pas d'action à annuler ce tour-ci.
bool Api::annuler()
{
    // TODO
    abort();
}


