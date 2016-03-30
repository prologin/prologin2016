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

#ifndef API_HH_
#define API_HH_

#include <vector>
#include <rules/game-state.hh>
#include <rules/player.hh>
#include <rules/actions.hh>

#include "game_state.hh"
#include "constant.hh"

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api
{

public:
    Api(GameState* game_state, rules::Player_sptr player);
    virtual ~Api() { }

    const rules::Player_sptr player() const { return player_; }
    void player_set(rules::Player_sptr player) { player_ = player; }

    rules::Actions* actions() { return &actions_; }

    const GameState* game_state() const { return game_state_; }
    GameState* game_state() { return game_state_; }
    void game_state_set(rules::GameState* gs) {
        game_state_ = dynamic_cast<GameState*>(gs);
    }

private:
    GameState* game_state_;
    rules::Player_sptr player_;
    rules::Actions actions_;

public:

///
// Construit un tuyau sur une case donnée
//
   erreur construire(position position);
///
// Améliore un tuyau en super-tuyau.
//
   erreur ameliorer(position position);
///
// Détruit un tuyau sur une case donnée.
//
   erreur detruire(position position);
///
// Déplace un point d'aspiration d'un point de la base vers l'autre.
//
   erreur deplacer_aspiration(position source, position destination);
///
// Renvoie la liste de tous les pulsars présents.
//
   std::vector<position> liste_pulsars();
///
// Renvoie la liste des cases contenant du plasma.
//
   std::vector<position> liste_plasmas();
///
// Renvoie la liste des cases contenant un tuyau (ou super-tuyau).
//
   std::vector<position> liste_tuyaux();
///
// Renvoie la liste des cases contenant un super-tuyau.
//
   std::vector<position> liste_super_tuyaux();
///
// Renvoie la liste des cases sous l'effet de radiations.
//
   std::vector<position> liste_radiation();
///
// Renvoie la liste des cases contenant un tuyau (ou super-tuyau) en cours de destruction.
//
   std::vector<position> liste_destruction_tuyaux();
///
// Renvoie vrai si et seulement si la case contient un pulsar.
//
   bool est_pulsar(position position);
///
// Renvoie vrai si et seulement si la case contient un simple tuyau.
//
   bool est_tuyau(position position);
///
// Renvoie vrai si et seulement si la case contient un super tuyau.
//
   bool est_super_tuyau(position position);
///
// Renvoie vrai si et seulement si la case contient un tuyau en cours de destruction.
//
   bool est_en_destruction(position position);
///
// Renvoie vrai si et seulement si la case est sous l'effet de radiations.
//
   bool est_en_radiation(position position);
///
// Renvoie vrai si et seulement s'il est possible de construire sur cette case.
//
   bool est_libre(position position);
///
// Renvoie le temps qu'il reste avant la destruction d'une case, et -1 si elle n'est pas en cours de destruction.
//
   int temps_destruction(position position);
///
// Renvoie le temps qu'il reste avant la dissipation des radiations sur une case, et -1 si elle n'en subit pas.
//
   int temps_radiation(position position);
///
// Renvoie les informations propres au pulsar à la position donnée.
//
   pulsar info_pulsar(position position);
///
// Renvoie la quantité de plasma sur une case donnée (0 s'il n'y a pas de plasma).
//
   int charges_presentes(position position);
///
// Renvoie la liste des cases composant votre base.
//
   std::vector<position> ma_base();
///
// Renvoie la liste des cases composant la base de votre ennemi.
//
   std::vector<position> base_ennemie();
///
// Renvoie la puissance d'aspiration d'une case de base donnée.
//
   std::vector<position> energie_aspiration(position position);
///
// Renvoie la liste des priorités des directions, dans l'ordre décroissant de priorité, au tour donné.
//
   std::vector<direction> priorite_directionnelle(position position, int tour);
///
// Renvoie pour une case donnée la direction de son aspiration (qu'elle contienne un plasma ou non), ou "aucune" si la case n'est pas une case contenant un tuyau ou si elle n'est reliée à aucune base.
//
   direction aspiration(position position);
///
// Renvoie la liste des tuyaux construits par votre adversaire au dernier tour.
//
   std::vector<position> hist_tuyaux_construits();
///
// Renvoie la liste des tuyaux détruits par votre adversaire au dernier tour.
//
   std::vector<position> hist_tuyaux_detruits();
///
// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier tour.
//
   std::vector<position> hist_tuyaux_ameliores();
///
// Renvoie la liste des différences d'énergie d'aspiration dans la base de votre adversaire au dernier tour.
//
   std::vector<position> hist_base_modification();
///
// Renvoie votre numéro de joueur.
//
   int moi();
///
// Renvoie le numéro de votre adversaire.
//
   int adversaire();
///
// Indique votre nombre de points d'actions restants pour ce tour-ci.
//
   int points_action();
///
// Renvoie le score du joueur désigné par le numéro ``id_joueur``.
//
   int score(int id_joueur);
///
// Renvoie le numéro du tour actuel.
//
   int tour_actuel();
///
// Annule la dernière action. Renvoie ``false`` quand il n'y a pas d'action à annuler ce tour-ci.
//
   bool annuler();
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

};


#endif /* !API_HH_ */
