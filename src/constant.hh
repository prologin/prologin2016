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

#ifndef CONSTANT_HH_
# define CONSTANT_HH_

///
// Taille du terrain (longueur et largeur).
//
# define TAILLE_TERRAIN            39

///
// Longueur de chacune de vos deux bases.
//
# define LONGUEUR_BASE             13

///
// Nombre de tours à jouer avant la fin de la partie.
//
# define NB_TOURS                  100

///
// Nombre de points d'action par tour.
//
# define NB_POINTS_ACTION          40

///
// Nombre de points d'action que coûte la construction d'un tuyau.
//
# define COUT_CONSTRUCTION_TUYAU   10

///
// Nombre de points d'action que coûte l'amélioration d'un tuyau.
//
# define COUT_AMELIORATION_TUYAU   10

///
// Nombre de points d'action que coûte la destruction d'un tuyau.
//
# define COUT_DESTRUCTION_TUYAU    30

///
// Nombre de points d'action que coûte la destruction d'un super-tuyau.
//
# define COUT_DESTRUCTION_SUPER_TUYAU 40

///
// Nombre de points d'action que coûte le déplacement d'un point d'énergie de la base (la première modification de chaque tour est offerte).
//
# define COUT_MODIFICATION_ASPIRATION 10

///
// Nombre de points d'action que coûte le déblayage d'une case de débris.
//
# define COUT_DEBLAYAGE            20

///
// Vitesse du plasma dans un tuyau normal, en nombre de cases par tour.
//
# define VITESSE_TUYAU             1

///
// Multiplicateur de la vitesse du plasma dans un super-tuyau.
//
# define MULTIPLICATEUR_VITESSE_SUPER_TUYAU 2

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
  PAS_DE_TUYAU_DETRUIT, /* <- Il n'y a pas de tuyau détruit à déblayer à la position spécifiée. */
} erreur;

///
// Types de cases
//
typedef enum case_type {
  VIDE, /* <- Case vide */
  TUYAU, /* <- Case contenant un tuyau */
  SUPER_TUYAU, /* <- Case contenant un super-tuyau */
  DEBRIS, /* <- Case contenant des débris à déblayer */
  PULSAR, /* <- Case contenant un pulsar */
  BASE, /* <- Case appartenant à une base d'un des joueurs */
  INTERDIT, /* <- Case où aucune action n'est possible */
} case_type;


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
  int periode;  /* <- Période de pulsation du pulsar */
  int puissance;  /* <- Quantité de plasma émise par chaque pulsation */
  int plasma_total;  /* <- Quantité de plasma restante */
} pulsar;



#endif // !CONSTANT_HH_
