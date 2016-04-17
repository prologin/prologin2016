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
* un nombre de pulsations restantes *R*.

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

Restes de la destruction d'un tuyau. Du plasma peut en sortir mais pas y
rentrer.

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

Vous pouvez dépenser ``COUT_DESTRUCTION_TUYAU`` points d'action pour lancer un
*tir de plasma* et détruire un tuyau, ou ``COUT_DESTRUCTION_SUPER_TUYAU``
points d'action pour détruire un Super-Tuyau™. Un tir de plasma vous consomme
de plus ``CHARGE_PLASMA`` charges de plasma que vous avez collecté. La case
visée est remplacée par une case de débris.

Le plasma encore présent dans le tuyau ou Super Tuyau™ détruit persiste dans
les débris.

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

Plasma
======

Les pulsars sur la carte pulsent régulièrement du plasma que vous devez
acheminer à votre base avec des tuyaux pour l'extraire et augmenter votre
score. La quantité de plasma se mesure en *charges*, un nombre réel positif.

À la fin du tour de chaque joueur, le plasma présent sur la carte se déplace
en direction des bases les plus proches.

Le plasma dans des tuyaux qui ne sont reliés à aucune base par d'autres
tuyaux disparaît définitivement. Sinon, les règles ci-dessous s'appliquent.

La *distance effective* entre une case ``c`` et une case de base ``b`` est
égale à ``D(c,b)-A(b)``, où ``D(c,b)`` est la longueur du plus court chemin de
``c`` à ``b`` ne passant que par des tuyaux et ``A(b)`` est la puissance
d'aspiration possédée par la case ``b``. Un Super Tuyau™ est considéré comme un
tuyau dans le calcul des distances. La *distance minimale* d'une case est la
plus petite distance effective entre cette case et n'importe quelle case de
base à laquelle elle est reliée.

À la fin d'un tour, il peut y avoir du plasma dans un tuyau, un Super Tuyau™,
ou des débris. À partir d'une case à distance minimale ``D_min``, le plasma se
déplace vers les cases voisines de base, tuyau ou Super Tuyau™ à distance
minimale ``D_min-1``. Il y en a toujours au moins une. Quand il y en a
plusieurs, le plasma se divise en quantités égales sur chacune de ces cases.
Le plasma qui arrive sur une case de base est immédiatement collecté par le
joueur propriétaire de cette case.

Le plasma avance d'une case s'il se trouve initialement sur un tuyau ou des
débris, deux sur un Super-Tuyau™, sans être affecté par d'autres Super-Tuyaux™
sur son trajet.

Enfin, quand la période d'un pulsar ``T`` est un diviseur du nombre de tours
passés et qu'il lui reste des pulsations (``R > 0``), il pulse, ce qui
décrémente ``R`` et ajoute ``P`` charges de plasma à chacune des quatre cases
adjacentes au pulsar. Ce plasma disparaît immédiatement s'il ne se trouve pas
dans un tuyau relié à une base.

Score
=====

Votre score est la quantité de plasma que vous avez collecté, arrondie
à l'entier inférieur. Détruire un tuyau vous coûte du plasma, ce qui
réduit effectivement votre score.

Format de la carte
==================

La carte est donnée par un fichier texte, où chaque ligne donne les
caractéristiques d'un pulsar sur la carte, sous la forme de cinq entiers :
*abscisse*, *ordonnée*, *période*, *puissance*, *nombre total de pulsations*.

Voici un exemple avec deux pulsars : ::

  11 15 9 5 8
  15 11 9 5 8
