=====================
Prologin 2016 : Sujet
=====================

------------
Introduction
------------

Votre objectif est de récolter le plasma produit par les pulsars, en les
aspirant dans des tuyaux menant à votre base.

-----
Carte
-----

La carte consiste en une grille carrée de ``TAILLE_TERRAIN`` cases de côté. Au
centre de chaque bord se trouve une base de ``LONGUEUR_BASE`` cases, encadrée
de zones interdites de chaque côté. Le reste de la région est constitué de
pulsars et de cases vides.

Base
====

Les bases se trouvant sur deux bords opposés appartiennent au même joueur.
Chaque case possède initialement une unité de puissance d'aspiration,
qui pourra être assignée à d'autres cases en cours de jeu.

Zone interdite
==============

Case inconstructible.

Vide
====

Case constructible.

Pulsar
======

Un pulsar a une position fixe, et possède des caractéristiques qui lui sont
propres :

* une période de pulsation *T*;
* une puissance de pulsation *P*;
* une durée de vie *D*.

Tuyau
=====

Transporte du plasma.
Les effets d'un tuyau (ou d'un Super-Tuyau™) ne dépendent pas du joueur qui l'a
construit.

Super-Tuyau™
============

Transporte du plasma plus rapidement qu'un tuyau et coûte plus cher à détruire.

Débris
======

Restes de la destruction d'un tuyau.

---------------------
Déroulement d'un tour
---------------------

Au début de votre tour, vous recevez ``NB_POINTS_ACTION`` *points d'action*
valables pour ce tour seulement. Ils vous permettent d'effectuer les actions
ci-dessous.

Actions
=======

Construire un tuyau
-------------------

Vous pouvez dépenser ``COUT_CONSTRUCTION_TUYAU`` points d'action pour
construire un tuyau sur une case vide.

Améliorer un tuyau en Super-Tuyau™
----------------------------------

Vous pouvez dépenser ``COUT_AMELIORATION_TUYAU`` points d'action pour améliorer
un tuyau existant en Super-Tuyau™.

Détruire un tuyau
-----------------

Vous pouvez dépenser ``COUT_DESTRUCTION_TUYAU`` points d'action pour détruire
un tuyau, laissant une case de débris à la place.

Déblayer des débris
-------------------

Vous pouvez dépenser ``COUT_DEBLAYAGE`` points d'action pour déblayer des
débris, rendant la case vide.

Modifier la puissance d'aspiration
----------------------------------

Cette action est gratuite une fois par tour, et coûte ensuite
``COUT_MODIFICATION_ASPIRATION`` points d'action à refaire dans le même tour.

Vous déplacez une unité de puissance d'aspiration d'une de vos cases de base à
une autre (éventuellement sur le bord opposé).  Bien sûr, vous ne pouvez
effectuer cette action que si la première case possède au moins une unité.
