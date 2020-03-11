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

#ifndef CONSTANT_HH_
#define CONSTANT_HH_

/// Taille du terrain (longueur et largeur).
#define TAILLE_TERRAIN 39

/// Longueur de chacune de vos deux bases.
#define LONGUEUR_BASE 13

/// Nombre de tours à jouer avant la fin de la partie.
#define NB_TOURS 100

/// Nombre de points d'action par tour.
#define NB_POINTS_ACTION 4

/// Nombre de points d'action que coûte la construction d'un tuyau.
#define COUT_CONSTRUCTION 1

/// Nombre de points d'action que coûte l'amélioration d'un tuyau.
#define COUT_AMELIORATION 1

/// Nombre de points d'action que coûte la destruction d'un tuyau.
#define COUT_DESTRUCTION 3

/// Nombre de points d'action que coûte la destruction d'un Super Tuyau™.
#define COUT_DESTRUCTION_SUPER_TUYAU 4

/// Charge en plasma nécessaire pour la destruction d'un tuyau ou d'un Super
/// Tuyau™.
#define CHARGE_DESTRUCTION 2

/// Nombre de points d'action que coûte le déblayage d'une case de débris.
#define COUT_DEBLAYAGE 2

/// Nombre de points d'action que coûte le déplacement d'une unité de puissance
/// d'aspiration de la base (la première modification de chaque tour est
/// offerte).
#define COUT_MODIFICATION_ASPIRATION 1

/// Limite de puissance d'aspiration sur une case de base.
#define LIMITE_ASPIRATION 5

/// Vitesse du plasma dans un tuyau normal, en nombre de cases par tour.
#define VITESSE_TUYAU 1

/// Multiplicateur de la vitesse du plasma dans un Super Tuyau™.
#define MULTIPLICATEUR_VITESSE_SUPER_TUYAU 2

/// Erreurs possibles
typedef enum erreur
{
    OK,              /* <- L'action a été exécutée avec succès. */
    PA_INSUFFISANTS, /* <- Vous ne possédez pas assez de points d'action pour
                        cette action. */
    AUCUN_TUYAU,     /* <- Il n'y a pas de tuyau à la position spécifiée. */
    POSITION_INVALIDE, /* <- La position spécifiée est hors de la carte. */
    PUISSANCE_INSUFFISANTE, /* <- Vous ne possédez pas assez de puissance
                               d'asipration sur cette partie de la base. */
    DEPLACEMENT_INVALIDE,   /* <- Vous ne pouvez pas déplacer de la puissance
                               d'aspiration d'une case à elle-même. */
    PAS_DANS_BASE,          /* <- Cette case n'appartient pas à votre base. */
    AMELIORATION_IMPOSSIBLE, /* <- Il y a déjà un Super Tuyau™ sur cette case.
                              */
    CONSTRUCTION_IMPOSSIBLE, /* <- Il est impossible de construire un tuyau à la
                                position indiquée. */
    DESTRUCTION_IMPOSSIBLE, /* <- Il n'y a pas de tuyau à la position spécifiée.
                             */
    PAS_DE_PULSAR, /* <- Il n'y a pas de pulsar à la position spécifiée. */
    PAS_DE_DEBRIS, /* <- Il n'y a pas de débris à la position spécifiée. */
    CHARGE_INSUFFISANTE, /* <- Vous ne possédez pas assez de plasma pour lancer
                            une destruction. */
    LIMITE_ASPIRATION_ATTEINTE, /* <- Vous avez atteint la limite d'aspiration
                                   sur cette case. */
} erreur;

/// Types de cases
typedef enum case_type
{
    VIDE,        /* <- Case vide */
    TUYAU,       /* <- Case contenant un tuyau */
    SUPER_TUYAU, /* <- Case contenant un Super Tuyau™ */
    DEBRIS,      /* <- Case contenant des débris à déblayer */
    PULSAR,      /* <- Case contenant un pulsar */
    BASE_JOUEUR, /* <- Case appartenant à une base d'un des joueurs */
    INTERDIT,    /* <- Case où aucune action n'est possible */
} case_type;

/// Position sur la carte, donnée par deux coordonnées.
typedef struct position
{
    int x; /* <- Coordonnée en X */
    int y; /* <- Coordonnée en Y */
} position;

/// Représente un pulsar existant.
typedef struct pulsar_info
{
    int periode;      /* <- Période de pulsation du pulsar */
    double puissance; /* <- Quantité de plasma émise par chaque pulsation dans
                         chaque direction */
    int pulsations_restantes; /* <- Nombre de pulsations restantes */
    int pulsations_totales;   /* <- Nombre total de pulsations au début de la
                                 partie */
} pulsar_info;

#endif // !CONSTANT_HH_
