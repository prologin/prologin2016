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

#include "api.hh"

#include <iostream>
#include <sstream>
#include <vector>

// from api.cc
extern Api* api;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr)
{
    os << "[";
    typename std::vector<T>::const_iterator it;
    for (it = arr.begin(); it != arr.end(); ++it)
    {
        if (it != arr.begin())
            os << ", ";
        os << *it;
    }
    os << "]";
    return os;
}

// todo avoir un ostringstream a la place de std::string

std::string convert_to_string(int i)
{
    std::ostringstream s;
    s << i;
    std::string result = s.str();
    return result;
}
std::string convert_to_string(double i)
{
    std::ostringstream s;
    s << i;
    std::string result = s.str();
    return result;
}
std::string convert_to_string(std::string i)
{
    return i;
}
std::string convert_to_string(bool i)
{
    return i ? "true" : "false";
}
std::string convert_to_string(std::vector<int> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(erreur in)
{
    switch (in)
    {
    case OK:
        return "\"ok\"";
    case PA_INSUFFISANTS:
        return "\"pa_insuffisants\"";
    case AUCUN_TUYAU:
        return "\"aucun_tuyau\"";
    case POSITION_INVALIDE:
        return "\"position_invalide\"";
    case PUISSANCE_INSUFFISANTE:
        return "\"puissance_insuffisante\"";
    case DEPLACEMENT_INVALIDE:
        return "\"deplacement_invalide\"";
    case PAS_DANS_BASE:
        return "\"pas_dans_base\"";
    case AMELIORATION_IMPOSSIBLE:
        return "\"amelioration_impossible\"";
    case CONSTRUCTION_IMPOSSIBLE:
        return "\"construction_impossible\"";
    case DESTRUCTION_IMPOSSIBLE:
        return "\"destruction_impossible\"";
    case PAS_DE_PULSAR:
        return "\"pas_de_pulsar\"";
    case PAS_DE_DEBRIS:
        return "\"pas_de_debris\"";
    case CHARGE_INSUFFISANTE:
        return "\"charge_insuffisante\"";
    case LIMITE_ASPIRATION_ATTEINTE:
        return "\"limite_aspiration_atteinte\"";
    }
    return "bad value";
}
std::string convert_to_string(std::vector<erreur> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(case_type in)
{
    switch (in)
    {
    case VIDE:
        return "\"vide\"";
    case TUYAU:
        return "\"tuyau\"";
    case SUPER_TUYAU:
        return "\"super_tuyau\"";
    case DEBRIS:
        return "\"debris\"";
    case PULSAR:
        return "\"pulsar\"";
    case BASE:
        return "\"base\"";
    case INTERDIT:
        return "\"interdit\"";
    }
    return "bad value";
}
std::string convert_to_string(std::vector<case_type> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(position in)
{
    std::string x = convert_to_string(in.x);
    std::string y = convert_to_string(in.y);
    std::string out = "{";
    out += "x:" + x;
    out += ", ";
    out += "y:" + y;
    return out + "}";
}

std::string convert_to_string(std::vector<position> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
std::string convert_to_string(pulsar_info in)
{
    std::string periode = convert_to_string(in.periode);
    std::string puissance = convert_to_string(in.puissance);
    std::string pulsations_restantes =
        convert_to_string(in.pulsations_restantes);
    std::string pulsations_totales = convert_to_string(in.pulsations_totales);
    std::string out = "{";
    out += "periode:" + periode;
    out += ", ";
    out += "puissance:" + puissance;
    out += ", ";
    out += "pulsations_restantes:" + pulsations_restantes;
    out += ", ";
    out += "pulsations_totales:" + pulsations_totales;
    return out + "}";
}

std::string convert_to_string(std::vector<pulsar_info> in)
{
    if (in.size())
    {
        std::string s = "[" + convert_to_string(in[0]);
        for (int i = 1, l = in.size(); i < l; i++)
        {
            s = s + ", " + convert_to_string(in[i]);
        }
        return s + "]";
    }
    else
    {
        return "[]";
    }
}
/// Construit un tuyau sur une case donnée.
extern "C" erreur api_construire(position pos)
{
    return api->construire(pos);
}

/// Améliore un tuyau en Super Tuyau™.
extern "C" erreur api_ameliorer(position pos)
{
    return api->ameliorer(pos);
}

/// Détruit un tuyau sur une case donnée.
extern "C" erreur api_detruire(position pos)
{
    return api->detruire(pos);
}

/// Déplace une unité de puissance d'aspiration d'une case de votre base vers
/// une autre.
extern "C" erreur api_deplacer_aspiration(position source, position destination)
{
    return api->deplacer_aspiration(source, destination);
}

/// Déblaye une case de débris.
extern "C" erreur api_deblayer(position pos)
{
    return api->deblayer(pos);
}

/// Renvoie le type d'une case donnée.
extern "C" case_type api_type_case(position pos)
{
    return api->type_case(pos);
}

/// Renvoie la liste de tous les pulsars présents.
extern "C" std::vector<position> api_liste_pulsars()
{
    return api->liste_pulsars();
}

/// Renvoie la liste des cases contenant du plasma.
extern "C" std::vector<position> api_liste_plasmas()
{
    return api->liste_plasmas();
}

/// Renvoie la liste des cases contenant un tuyau ou Super Tuyau™.
extern "C" std::vector<position> api_liste_tuyaux()
{
    return api->liste_tuyaux();
}

/// Renvoie la liste des cases contenant un Super Tuyau™.
extern "C" std::vector<position> api_liste_super_tuyaux()
{
    return api->liste_super_tuyaux();
}

/// Renvoie la liste des cases contenant des débris.
extern "C" std::vector<position> api_liste_debris()
{
    return api->liste_debris();
}

/// Renvoie vrai si et seulement si la case contient un pulsar.
extern "C" bool api_est_pulsar(position pos)
{
    return api->est_pulsar(pos);
}

/// Renvoie vrai si et seulement si la case contient un tuyau ou un Super
/// Tuyau™.
extern "C" bool api_est_tuyau(position pos)
{
    return api->est_tuyau(pos);
}

/// Renvoie vrai si et seulement si la case contient un simple tuyau.
extern "C" bool api_est_simple_tuyau(position pos)
{
    return api->est_simple_tuyau(pos);
}

/// Renvoie vrai si et seulement si la case contient un Super Tuyau™.
extern "C" bool api_est_super_tuyau(position pos)
{
    return api->est_super_tuyau(pos);
}

/// Renvoie vrai si et seulement si la case contient un débris.
extern "C" bool api_est_debris(position pos)
{
    return api->est_debris(pos);
}

/// Renvoie vrai si et seulement s'il est possible de construire sur cette case.
extern "C" bool api_est_libre(position pos)
{
    return api->est_libre(pos);
}

/// Renvoie les informations propres au pulsar à la position donnée.
extern "C" pulsar_info api_info_pulsar(position pos)
{
    return api->info_pulsar(pos);
}

/// Renvoie la quantité de plasma sur une case donnée.
extern "C" double api_charges_presentes(position pos)
{
    return api->charges_presentes(pos);
}

/// Renvoie le numéro du joueur ayant dernièrement construit ou amélioré un
/// tuyau sur une case.
extern "C" int api_constructeur_tuyau(position pos)
{
    return api->constructeur_tuyau(pos);
}

/// Renvoie le numéro du propriétaire d'une case de base.
extern "C" int api_proprietaire_base(position pos)
{
    return api->proprietaire_base(pos);
}

/// Renvoie la liste des cases composant votre base.
extern "C" std::vector<position> api_ma_base()
{
    return api->ma_base();
}

/// Renvoie la liste des cases composant la base de votre ennemi.
extern "C" std::vector<position> api_base_ennemie()
{
    return api->base_ennemie();
}

/// Renvoie la puissance d'aspiration d'une case de base donnée. Renvoie -1 si
/// la position n'est pas celle d'une base.
extern "C" int api_puissance_aspiration(position pos)
{
    return api->puissance_aspiration(pos);
}

/// Renvoie pour une case donnée la liste des cases vers lesquelles le plasma se
/// propagera. Renvoie la liste vide si la case n'est pas une case contenant un
/// tuyau ou si elle n'est reliée à aucune base.
extern "C" std::vector<position> api_directions_plasma(position pos)
{
    return api->directions_plasma(pos);
}

/// Renvoie la valeur du coût de la prochaine modification de vos puissances
/// d'aspiration.
extern "C" int api_cout_prochaine_modification_aspiration()
{
    return api->cout_prochaine_modification_aspiration();
}

/// Renvoie la liste des tuyaux construits par votre adversaire au dernier tour.
extern "C" std::vector<position> api_hist_tuyaux_construits()
{
    return api->hist_tuyaux_construits();
}

/// Renvoie la liste des tuyaux détruits par votre adversaire au dernier tour.
extern "C" std::vector<position> api_hist_tuyaux_detruits()
{
    return api->hist_tuyaux_detruits();
}

/// Renvoie la liste des tuyaux améliorés par votre adversaire au dernier tour.
extern "C" std::vector<position> api_hist_tuyaux_ameliores()
{
    return api->hist_tuyaux_ameliores();
}

/// Renvoie la liste des débris déblayés par votre adversaire au dernier tour.
extern "C" std::vector<position> api_hist_debris_deblayes()
{
    return api->hist_debris_deblayes();
}

/// Renvoie la liste des cases de base de votre adversaire qui ont reçu un point
/// d'aspiration (une même case peut apparaître plusieurs fois).
extern "C" std::vector<position> api_hist_points_aspiration_ajoutes()
{
    return api->hist_points_aspiration_ajoutes();
}

/// Renvoie la liste des cases de base de votre adversaire qui ont perdu un
/// point d'aspiration (une même case peut apparaître plusieurs fois).
extern "C" std::vector<position> api_hist_points_aspiration_retires()
{
    return api->hist_points_aspiration_retires();
}

/// Renvoie votre numéro de joueur.
extern "C" int api_moi()
{
    return api->moi();
}

/// Renvoie le numéro de votre adversaire.
extern "C" int api_adversaire()
{
    return api->adversaire();
}

/// Indique votre nombre de points d'actions restants pour ce tour-ci.
extern "C" int api_points_action()
{
    return api->points_action();
}

/// Renvoie le score du joueur désigné par le numéro ``id_joueur``. Renvoie -1
/// si ``id_joueur`` est invalide.
extern "C" int api_score(int id_joueur)
{
    return api->score(id_joueur);
}

/// Renvoie le numéro du tour actuel.
extern "C" int api_tour_actuel()
{
    return api->tour_actuel();
}

/// Annule la dernière action. Renvoie ``false`` quand il n'y a pas d'action à
/// annuler ce tour-ci.
extern "C" bool api_annuler()
{
    return api->cancel();
}

/// Affiche le contenu d'une valeur de type erreur
std::ostream& operator<<(std::ostream& os, erreur v)
{
    switch (v)
    {
    case OK:
        os << "OK";
        break;
    case PA_INSUFFISANTS:
        os << "PA_INSUFFISANTS";
        break;
    case AUCUN_TUYAU:
        os << "AUCUN_TUYAU";
        break;
    case POSITION_INVALIDE:
        os << "POSITION_INVALIDE";
        break;
    case PUISSANCE_INSUFFISANTE:
        os << "PUISSANCE_INSUFFISANTE";
        break;
    case DEPLACEMENT_INVALIDE:
        os << "DEPLACEMENT_INVALIDE";
        break;
    case PAS_DANS_BASE:
        os << "PAS_DANS_BASE";
        break;
    case AMELIORATION_IMPOSSIBLE:
        os << "AMELIORATION_IMPOSSIBLE";
        break;
    case CONSTRUCTION_IMPOSSIBLE:
        os << "CONSTRUCTION_IMPOSSIBLE";
        break;
    case DESTRUCTION_IMPOSSIBLE:
        os << "DESTRUCTION_IMPOSSIBLE";
        break;
    case PAS_DE_PULSAR:
        os << "PAS_DE_PULSAR";
        break;
    case PAS_DE_DEBRIS:
        os << "PAS_DE_DEBRIS";
        break;
    case CHARGE_INSUFFISANTE:
        os << "CHARGE_INSUFFISANTE";
        break;
    case LIMITE_ASPIRATION_ATTEINTE:
        os << "LIMITE_ASPIRATION_ATTEINTE";
        break;
    }
    return os;
}
extern "C" void api_afficher_erreur(erreur v)
{
    std::cerr << v << std::endl;
}

/// Affiche le contenu d'une valeur de type case_type
std::ostream& operator<<(std::ostream& os, case_type v)
{
    switch (v)
    {
    case VIDE:
        os << "VIDE";
        break;
    case TUYAU:
        os << "TUYAU";
        break;
    case SUPER_TUYAU:
        os << "SUPER_TUYAU";
        break;
    case DEBRIS:
        os << "DEBRIS";
        break;
    case PULSAR:
        os << "PULSAR";
        break;
    case BASE:
        os << "BASE";
        break;
    case INTERDIT:
        os << "INTERDIT";
        break;
    }
    return os;
}
extern "C" void api_afficher_case_type(case_type v)
{
    std::cerr << v << std::endl;
}

/// Affiche le contenu d'une valeur de type position
std::ostream& operator<<(std::ostream& os, position v)
{
    os << "{ ";
    os << "x"
       << "=" << v.x;
    os << ", ";
    os << "y"
       << "=" << v.y;
    os << " }";
    return os;
}
extern "C" void api_afficher_position(position v)
{
    std::cerr << v << std::endl;
}

/// Affiche le contenu d'une valeur de type pulsar_info
std::ostream& operator<<(std::ostream& os, pulsar_info v)
{
    os << "{ ";
    os << "periode"
       << "=" << v.periode;
    os << ", ";
    os << "puissance"
       << "=" << v.puissance;
    os << ", ";
    os << "pulsations_restantes"
       << "=" << v.pulsations_restantes;
    os << ", ";
    os << "pulsations_totales"
       << "=" << v.pulsations_totales;
    os << " }";
    return os;
}
extern "C" void api_afficher_pulsar_info(pulsar_info v)
{
    std::cerr << v << std::endl;
}
