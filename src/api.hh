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

#ifndef API_HH_
#define API_HH_

#include <vector>
#include <rules/game-state.hh>
#include <rules/player.hh>
#include <rules/actions.hh>

#include "game_state.hh"
#include "constant.hh"

/// A shared pointer to the current game state
/**
 * Stechec2 works with the game state as a chained link. Only the head is really
 * relevant, but the rest of the chain is used for undo.
 * The system is made so that the Api class owns the GameState, but the pointed
 * GameState object will keep changing since at each change we will get a new
 * head for the list.
 * But with this it is impossible to maintain, outside of the Api class, a
 * pointer to the current GameState. Or to have several Api classes sharing the
 * same GameState. This is annoying for unit tests for example.
 * This class encapsulate the GameState for that purpose, with minimum of code
 * changed.
 * The ideal solution would be a good build in Stechec2 ownership semantics.
 */
class GameStateWrapper
{
    struct SharedState
    {
        GameState* game_state;
        SharedState(GameState* game_state)
            : game_state(game_state)
        {
        }
        ~SharedState() { delete game_state; }
    };
    std::shared_ptr<SharedState> ptr;

public:
    explicit GameStateWrapper(GameState* game_state)
        : ptr(std::make_shared<SharedState>(game_state))
    {
    }
    operator GameState*() const { return ptr->game_state; }
    GameStateWrapper& operator=(GameState* g)
    {
        ptr->game_state = g;
        return *this;
    }
    GameState* operator->() const { return ptr->game_state; }
    GameState& operator*() const { return *ptr->game_state; }
};

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api
{
public:
    Api(const GameStateWrapper& game_state, rules::Player_sptr player);
    virtual ~Api() {}

    const rules::Player_sptr player() const { return player_; }
    void player_set(rules::Player_sptr player) { player_ = player; }

    rules::Actions* actions() { return &actions_; }

    const GameState* game_state() const { return game_state_; }
    GameState* game_state() { return game_state_; }
    void game_state_set(rules::GameState* gs)
    {
        game_state_ = dynamic_cast<GameState*>(gs);
    }

private:
    GameStateWrapper game_state_;
    rules::Player_sptr player_;
    rules::Actions actions_;

public:
    /// Construit un tuyau sur une case donnée.
    erreur construire(position position);

    /// Améliore un tuyau en super-tuyau.
    erreur ameliorer(position position);

    /// Détruit un tuyau sur une case donnée.
    erreur detruire(position position);

    /// Déplace une unité de puissance d'aspiration d'une case de votre base vers une autre.
    erreur deplacer_aspiration(position source, position destination);

    /// Déblaye une case de débris.
    erreur deblayer(position position);

    /// Renvoie le type d'une case donnée.
    case_type type_case(position position);

    /// Renvoie la liste de tous les pulsars présents.
    std::vector<position> liste_pulsars();

    /// Renvoie la liste des cases contenant du plasma.
    std::vector<position> liste_plasmas();

    /// Renvoie la liste des cases contenant un tuyau (ou super-tuyau).
    std::vector<position> liste_tuyaux();

    /// Renvoie la liste des cases contenant un super-tuyau.
    std::vector<position> liste_super_tuyaux();

    /// Renvoie la liste des cases contenant des débris.
    std::vector<position> liste_debris();

    /// Renvoie vrai si et seulement si la case contient un pulsar.
    bool est_pulsar(position position);

    /// Renvoie vrai si et seulement si la case contient un tuyau ou un Super Tuyau™.
    bool est_tuyau(position pos);

    /// Renvoie vrai si et seulement si la case contient un simple tuyau.
    bool est_simple_tuyau(position pos);

    /// Renvoie vrai si et seulement si la case contient un super tuyau.
    bool est_super_tuyau(position position);

    /// Renvoie vrai si et seulement si la case contient un tuyau débris.
    bool est_debris(position position);

    /// Renvoie vrai si et seulement s'il est possible de construire sur cette
    /// case.
    bool est_libre(position position);

    /// Renvoie les informations propres au pulsar à la position donnée.
    pulsar_info info_pulsar(position position);

    /// Renvoie la quantité de plasma sur une case donnée.
    double charges_presentes(position pos);

    /// Renvoie le numéro du joueur ayant dernièrement construit ou amélioré un
    /// tuyau sur une case.
    int constructeur_tuyau(position pos);

    /// Renvoie le numéro du propriétaire d'une case de base.
    int proprietaire_base(position pos);

    /// Renvoie la liste des cases composant votre base.
    std::vector<position> ma_base();

    /// Renvoie la liste des cases composant la base de votre ennemi.
    std::vector<position> base_ennemie();

    /// Renvoie la puissance d'aspiration sur une case de base.
    /// Renvoie -1 si la position n'est pas celle d'une base.
    int puissance_aspiration(position pos);

    /// Renvoie pour une case donnée la liste des cases vers
    /// lesquelles se propagerait un plasma. Renvoie la liste vide
    /// si la case n'est pas une case contenant un tuyau ou si elle
    /// n'est reliée à aucune base.
    std::vector<position> directions_plasma(position position);

    /// Renvoie la valeur du coût de la prochaine modification de vos
    /// puissances d'aspiration.
    int cout_prochaine_modification_aspiration();

    /// Renvoie la liste des tuyaux construits par votre adversaire au dernier
    /// tour.
    std::vector<position> hist_tuyaux_construits();

    /// Renvoie la liste des tuyaux détruits par votre adversaire au dernier
    /// tour.
    std::vector<position> hist_tuyaux_detruits();

    /// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier
    /// tour.
    std::vector<position> hist_tuyaux_ameliores();

    /// Renvoie la liste des débris déblayés par votre adversaire au dernier
    /// tour.
    std::vector<position> hist_debris_deblayes();

    /// Renvoie la liste des cases de base de votre adversaire qui ont reçu un
    /// point d'aspiration (une même case peut apparaître plusieurs fois).
    std::vector<position> hist_points_aspiration_ajoutes();

    /// Renvoie la liste des cases de base de votre adversaire qui ont perdu un
    /// point d'aspiration (une même case peut apparaître plusieurs fois).
    std::vector<position> hist_points_aspiration_retires();

    /// Renvoie votre numéro de joueur.
    int moi();

    /// Renvoie le numéro de votre adversaire.
    int adversaire();

    /// Indique votre nombre de points d'actions restants pour ce tour-ci.
    int points_action();

    /// Renvoie le score du joueur désigné par le numéro ``id_joueur``.
    /// Renvoie -1 si ``id_joueur`` est invalide.
    int score(int id_joueur);

    /// Renvoie le numéro du tour actuel.
    int tour_actuel();

    /// Annule la dernière action. Renvoie ``false`` quand il n'y a pas d'action
    /// à annuler ce tour-ci.
    bool annuler();
};

#endif /* !API_HH_ */
