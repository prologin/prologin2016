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

#include "actions.hh"
#include "api.hh"

// global used in interface.cc
Api* api;

Api::Api(std::unique_ptr<GameState> game_state,
         std::shared_ptr<rules::Player> player)
    : rules::Api<GameState, erreur>(std::move(game_state), player)
    , construire(this)
    , ameliorer(this)
    , detruire(this)
    , deplacer_aspiration(this)
    , deblayer(this)
{
    api = this;
}

/// Renvoie le type d'une case donnée.
case_type Api::type_case(position position)
{
    return game_state_->get_cell_type(position);
}

/// Renvoie la liste de tous les pulsars présents.
std::vector<position> Api::liste_pulsars()
{
    return game_state_->pulsars_list();
}

/// Renvoie la liste des cases contenant du plasma.
std::vector<position> Api::liste_plasmas()
{
    std::vector<position> ret;
    for (int x = 0; x < TAILLE_TERRAIN; ++x)
    {
        for (int y = 0; y < TAILLE_TERRAIN; ++y)
        {
            position pos{x, y};
            if (game_state_->get_plasma(pos) > 0)
                ret.push_back(pos);
        }
    }
    return ret;
}

/// Renvoie la liste des cases contenant un tuyau ou Super Tuyau™.
std::vector<position> Api::liste_tuyaux()
{
    std::vector<position> ret;
    for (int x = 0; x < TAILLE_TERRAIN; ++x)
    {
        for (int y = 0; y < TAILLE_TERRAIN; ++y)
        {
            position pos{x, y};
            case_type type = game_state_->get_cell_type(pos);
            if (type == TUYAU || type == SUPER_TUYAU)
                ret.push_back(pos);
        }
    }
    return ret;
}

/// Renvoie la liste des cases contenant un Super Tuyau™.
std::vector<position> Api::liste_super_tuyaux()
{
    std::vector<position> ret;
    for (int x = 0; x < TAILLE_TERRAIN; ++x)
    {
        for (int y = 0; y < TAILLE_TERRAIN; ++y)
        {
            position pos{x, y};
            if (game_state_->get_cell_type(pos) == SUPER_TUYAU)
                ret.push_back(pos);
        }
    }
    return ret;
}

/// Renvoie la liste des cases contenant un débris.
std::vector<position> Api::liste_debris()
{
    std::vector<position> ret;
    for (int x = 0; x < TAILLE_TERRAIN; ++x)
    {
        for (int y = 0; y < TAILLE_TERRAIN; ++y)
        {
            position pos{x, y};
            if (game_state_->get_cell_type(pos) == DEBRIS)
                ret.push_back(pos);
        }
    }
    return ret;
}

/// Renvoie vrai si et seulement si la case contient un pulsar.
bool Api::est_pulsar(position position)
{
    return game_state_->get_cell_type(position) == case_type::PULSAR;
}

/// Renvoie vrai si et seulement si la case contient un tuyau ou un Super
/// Tuyau™.
bool Api::est_tuyau(position position)
{
    case_type type = game_state_->get_cell_type(position);
    return type == case_type::TUYAU || type == case_type::SUPER_TUYAU;
}

/// Renvoie vrai si et seulement si la case contient un simple tuyau.
bool Api::est_simple_tuyau(position position)
{
    return game_state_->get_cell_type(position) == case_type::TUYAU;
}

/// Renvoie vrai si et seulement si la case contient un Super Tuyau™.
bool Api::est_super_tuyau(position position)
{
    return game_state_->get_cell_type(position) == case_type::SUPER_TUYAU;
}

/// Renvoie vrai si et seulement si la case contient un débris.
bool Api::est_debris(position position)
{
    return game_state_->get_cell_type(position) == case_type::DEBRIS;
}

/// Renvoie vrai si et seulement s'il est possible de construire sur cette case.
bool Api::est_libre(position position)
{
    return game_state_->get_cell_type(position) == case_type::VIDE;
}

/// Renvoie les informations propres au pulsar à la position donnée.
pulsar_info Api::info_pulsar(position position)
{
    if (est_pulsar(position))
        return game_state_->get_pulsar(position);
    else
        return {-1, -1, -1, -1};
}

/// Renvoie la quantité de plasma sur une case donnée.
double Api::charges_presentes(position position)
{
    return game_state_->get_plasma(position);
}

/// Renvoie le numéro du joueur ayant dernièrement construit ou amélioré un
/// tuyau sur une case.
int Api::constructeur_tuyau(position pos)
{
    return game_state_->get_cell_owner(pos);
}

/// Renvoie le numéro du propriétaire d'une case de base.
int Api::proprietaire_base(position pos)
{
    return game_state_->get_cell_owner(pos);
}

/// Renvoie la liste des cases composant votre base.
std::vector<position> Api::ma_base()
{
    return game_state_->bases_list(moi());
}

/// Renvoie la liste des cases composant la base de votre ennemi.
std::vector<position> Api::base_ennemie()
{
    return game_state_->bases_list(adversaire());
}

/// Renvoie la puissance d'aspiration d'une case de base donnée.
/// Renvoie -1 si la position n'est pas celle d'une base.
int Api::puissance_aspiration(position position)
{
    if (game_state_->get_cell_type(position) == BASE)
        return game_state_->get_vacuum(position);
    else
        return -1;
}

/// Renvoie pour une case donnée la liste des cases vers
/// lesquelles se propagerait un plasma. Renvoie la liste vide
/// si la case n'est pas une case contenant un tuyau ou si elle
/// n'est reliée à aucune base.
std::vector<position> Api::directions_plasma(position position)
{
    if (1 <= position.x && position.x < TAILLE_TERRAIN - 1 && 1 <= position.y &&
        position.y < TAILLE_TERRAIN - 1)
        return game_state_->direction_plasma(position);
    else
        return {};
}

/// Renvoie la valeur du coût de la prochaine modification de vos puissances
/// d'aspiration.
int Api::cout_prochaine_modification_aspiration()
{
    if (game_state_->get_vacuum_moved()) // Already used, full cost
        return COUT_MODIFICATION_ASPIRATION;
    else // The first one is free
        return 0;
}

/// Renvoie la liste des tuyaux construits par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_construits()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == BUILD)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie la liste des tuyaux détruits par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_detruits()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == DESTROY)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier tour.
std::vector<position> Api::hist_tuyaux_ameliores()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == UPGRADE)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie la liste des débris déblayés par votre adversaire au dernier tour.
std::vector<position> Api::hist_debris_deblayes()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == CLEAR)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie la liste des cases de base de votre adversaire qui ont reçu un point
/// d'aspiration (une même case peut apparaître plusieurs fois).
std::vector<position> Api::hist_points_aspiration_ajoutes()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == INCR_VACUUM)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie la liste des cases de base de votre adversaire qui ont perdu un
/// point d'aspiration (une même case peut apparaître plusieurs fois).
std::vector<position> Api::hist_points_aspiration_retires()
{
    std::vector<position> ret;
    for (const auto& action : game_state_->get_history(adversaire()))
        if (action.type == DECR_VACUUM)
            ret.push_back(action.pos);
    return ret;
}

/// Renvoie votre numéro de joueur.
int Api::moi()
{
    return player_->id;
}

/// Renvoie le numéro de votre adversaire.
int Api::adversaire()
{
    return game_state_->opponent(player_->id);
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
    if (id_joueur == moi() || id_joueur == adversaire())
        return game_state_->get_score(id_joueur);
    else
        return -1;
}

/// Renvoie le numéro du tour actuel.
int Api::tour_actuel()
{
    return game_state_->get_turn();
}
