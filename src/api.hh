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

#include <rules/actions.hh>
#include <rules/api.hh>
#include <rules/game-state.hh>
#include <rules/player.hh>
#include <vector>

#include "actions.hh"
#include "constant.hh"
#include "game_state.hh"

/*!
** The methods of this class are exported through 'interface.cc'
** to be called by the clients
*/
class Api final : public rules::Api<GameState, erreur>
{
public:
    Api(std::unique_ptr<GameState> game_state,
        std::shared_ptr<rules::Player> player);
    virtual ~Api() {}

    /// Construit un tuyau sur une case donnée.
    ApiActionFunc<ActionConstruire, position> construire;

    /// Améliore un tuyau en Super Tuyau™.
    ApiActionFunc<ActionAmeliorer, position> ameliorer;

    /// Détruit un tuyau sur une case donnée.
    ApiActionFunc<ActionDetruire, position> detruire;

    /// Déplace une unité de puissance d'aspiration d'une case de votre base
    /// vers une autre.
    ApiActionFunc<ActionDeplacerAspiration> deplacer_aspiration;

    /// Déblaye une case de débris.
    ApiActionFunc<ActionDeblayer, position> deblayer;

    /// Renvoie le type d'une case donnée.
    case_type type_case(position pos);

    /// Renvoie la liste de tous les pulsars présents.
    std::vector<position> liste_pulsars();

    /// Renvoie la liste des cases contenant du plasma.
    std::vector<position> liste_plasmas();

    /// Renvoie la liste des cases contenant un tuyau ou Super Tuyau™.
    std::vector<position> liste_tuyaux();

    /// Renvoie la liste des cases contenant un Super Tuyau™.
    std::vector<position> liste_super_tuyaux();

    /// Renvoie la liste des cases contenant des débris.
    std::vector<position> liste_debris();

    /// Renvoie vrai si et seulement si la case contient un pulsar.
    bool est_pulsar(position pos);

    /// Renvoie vrai si et seulement si la case contient un tuyau ou un Super
    /// Tuyau™.
    bool est_tuyau(position pos);

    /// Renvoie vrai si et seulement si la case contient un simple tuyau.
    bool est_simple_tuyau(position pos);

    /// Renvoie vrai si et seulement si la case contient un Super Tuyau™.
    bool est_super_tuyau(position pos);

    /// Renvoie vrai si et seulement si la case contient un débris.
    bool est_debris(position pos);

    /// Renvoie vrai si et seulement s'il est possible de construire sur cette
    /// case.
    bool est_libre(position pos);

    /// Renvoie les informations propres au pulsar à la position donnée.
    pulsar_info info_pulsar(position pos);

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

    /// Renvoie pour une case donnée la liste des cases vers lesquelles le
    /// plasma se propagera. Renvoie la liste vide si la case n'est pas une
    /// case contenant un tuyau ou si elle n'est reliée à aucune base.
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
};

#endif /* !API_HH_ */
