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
# define CONSTANT_HH_

///
// Taille du terrain (longueur et largeur).
//
# define TAILLE_TERRAIN            33

///
// Longueur de chacune de vos deux bases.
//
# define LONGUEUR_BASE             11

///
// Nombre de tours à jouer avant la fin de la partie.
//
# define NB_TOURS                  100

///
// Nombre de points d'action par tour.
//
# define NB_POINTS_ACTION          42

///
// Nombre de points d'énergie d'aspiration dans chacune de vos bases.
//
# define NB_POINTS_ENERGIE_ASPIRATION 11

///
// Nombre de points d'action que coûte la construction d'un tuyau.
//
# define COUT_CONSTRUCTION_TUYAU   10

///
// Nombre de points d'action que coûte l'amélioration d'un tuyau.
//
# define COUT_AMELIORATION_TUYAU   25

///
// Nombre de points d'action que coûte la destruction d'un tuyau.
//
# define COUT_DESTRUCTION_TUYAU    25

///
// Nombre de points d'action que coûte la destruction d'un super-tuyau.
//
# define COUT_DESTRUCTION_SUPER_TUYAU 40

///
// Nombre de points d'action que coûte le déplacement d'un point d'énergie de la base.
//
# define COUT_MODIFICATION_ASPIRATION 2

///
// Vitesse du plasma dans un tuyau normal, en nombre de case par tour.
//
# define VITESSE_TUYAU             1

///
// Multiplicateur de la vitesse du plasma lors de leur présence dans un super-tuyau.
//
# define MULTIPLICATEUR_VITESSE_SUPER_TUYAU 2

///
// Nombre de tours d'impossibilité de reconstruire un tuyau dans une zone après la destruction d'un autre.
//
# define TEMPS_DISSIPATION_RADIOACTIVE 3

///
// Rayon définissant une zone autour d'une case où un tuyau a été détruit dans laquelle il n'est pas possible de construire pendant un certain temps.
//
# define RAYON_ZONE_RADIOACTIVE    2

///
// Nombre de tours que prend la destruction d'un tuyau.
//
# define TEMPS_DESTRUCTION_TUYAU   3

///
// Erreurs possibles
//
typedef enum erreur {
  OK, /* <- L'action a été exécutée avec succès. */
  PA_INSUFFISANTS, /* <- Vous ne possédez pas assez de points d'action pour cette action. */
  AUCUN_TUYAU, /* <- Il n'y a pas de tuyau à la position spécifiée. */
  POSITION_INVALIDE, /* <- La position spécifiée est hors de la carte. */
  PE_INSUFFISANTS, /* <- Vous ne possédez pas assez de points d'énergie sur cette partie de la base. */
  PAS_DANS_BASE, /* <- Cette case n'appartient pas à votre base. */
  AMELIORATION_IMPOSSIBLE, /* <- Il y a déjà un super-tuyau sur cette case. */
  CONSTRUCTION_IMPOSSIBLE, /* <- Il est impossible de construire un tuyau à la position indiquée. */
  DESTRUCTION_IMPOSSIBLE, /* <- Il n'y a pas de tuyau à la position spécifiée. */
  PAS_DE_PULSAR, /* <- Il n'y a pas de pulsar à la position spécifiée. */
} erreur;


///
// Directions possibles
//
typedef enum direction {
  NORD, /* <- Nord. */
  SUD, /* <- Sud. */
  OUEST, /* <- Ouest. */
  EST, /* <- Est. */
  AUCUNE, /* <- Aucune direction. */
} direction;


///
// Position sur la carte, donnée par deux coordonnées.
//
typedef struct position {
  int x;  /* <- Coordonnée en X */
  int y;  /* <- Coordonnée en Y */
} position;


///
// Représente un pulsar existant.
//
typedef struct pulsar {
  int periode;  /* <- Période de pulsation du pulsar. */
  int puissance;  /* <- Quantité de plasma émise par chaque pulsation. */
  int plasma_total;  /* <- Quantité de plasma restante. */
} pulsar;



#endif // !CONSTANT_HH_
