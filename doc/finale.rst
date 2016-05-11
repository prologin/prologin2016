=====================
Prologin 2016 : Sujet
=====================

------------
Introduction
------------

Maintenant que vous avez bien compris la raison de votre présence, nous allons vous présenter le détail de votre mission.
Nous avons besoin d'un maximum de productivité et nous ne pourront pas nous permettre de garder ceux d'entre vous qui ne seront pas à la hauteur.
Pour tester vos capacités, nous allons vous mettre en situation sur des sites de récolte. Vous vous affronterez par groupes de 2.
L'énergie que vous nous apporterez nous sera de plus grande utilité, si bien que c'est uniquement par cette quantité que vous serez évalués.
Les sites se situent aux alentours 14 142 km du sol et prennent la forme de carrés en 2 dimensions. Ces sites sont remarquables par la présence de pulsars qui émettent séquentiellement du plasma autour d'eux. Le plasma émit apporte l'énergie dont nous avons besoin. Cependant il est fortement instable et ne peut se maintenir seul dans l'espace. Le seul moyen de le récolter est de le maintenir enfermé jusque dans votre base.
Des bases sont disponibles sur la bordure de ces sites et permettent de récupérer l'énergie du plasma. Sur un site donné, chacun des deux opposants disposera de la moitié des bases, l'un occupera les bases au Nord et au Sud tandis que l'autre disposera de celles d'Est et Ouest.
Pour permettre le transport du plasma des pulsars vers vos bases vous disposerez d'une quantité importante de tuyaux.
Chaque base possède une puissance d'aspiration électro-magnétique permettant de piéger le plasma et de l'attirer vers elle. Cette aspiration se propage le long des tuyaux avec une atténuation constante. Lorsqu'un plasma est relié à une base de part le réseau de tuyaux, il est soumis à cette attractivité. Comme il peut être attiré vers plusieurs directions à la fois, il se déplace à chaque instant dans la direction où l'attractivité est la plus forte, et il se divise en parts uniformes s'il fait face à plusieurs forces d'attraction identiques provenant de directions différentes.
Chaque base est initialement chargée d'une unité de puissance d'aspiration, vous aurez la possibilité d'en déplacer des unités d'une case de base vers une autre, en faisant attention de ne pas dépasser une certaine limite qui pourrait déclencher une surcharge d'énergie.
Pour corser la partie nous vous autorisons à détruire les tuyaux présents sur le site, que ce soit vous ou vous adversaire qu'il l'ait construit. Cependant, une fois détruit, un tuyau laisse des débris empêchant de construire par dessus sans déblayer le lieu au préalable.
En guise de défense, il vous sera possible d'améliorer les tuyaux existants. Un super tuyau sera plus coûteux à détruire et tout plasma qui passe dedans verra sa vitesse doubler temporairement.
Vous serez évalués sur un temps fixé à l'issue duquel un vainqueur sera désigné. Initialement le site est vide et ne contient que les pulsars, certains d'entre eux pourront cesser d'émettre au cours de la partie, car ils disposent d'une quantité de plasma finie. À vous de vous débrouiller pour en récolter un maximum.


----
Site
----

Chaque site consiste en une grille carrée de ``TAILLE_TERRAIN`` cases de côté. Au
centre de chaque bord se trouve une rangée de bases de ``LONGUEUR_BASE`` cases, encadrée
de zones interdites de chaque côté. Le reste de la région est constitué de
pulsars et de cases vides.

Base
====

Les bases se trouvant sur deux bords opposés appartiennent au même joueur.
Chaque case possède initialement une unité de puissance d'aspiration,
qui pourra être assignée à d'autres cases en cours de jeu, dans la limite de
``LIMITE_ASPIRATION`` unités par case.

Zone interdite
==============

Ce sont les cases du bord des sites qui ne sont pas des cases de base, il n'est pas
possible de construire par dessus.

Vide
====

Ce sont des cases qui ne contiennent rien, la seule action possible est de construire
par dessus.

Pulsar
======

Un pulsar a une position fixe, et possède des caractéristiques qui lui sont
propres :

* une période de pulsation *T*;
* une puissance de pulsation *P*;
* un nombre de pulsations restantes *R*.

Tuyau
=====

Le tuyau est un composant qui permet de transporter le plasma.
Les effets d'un tuyau (ou d'un Super-Tuyau™) ne dépendent pas du joueur qui l'a
construit.

Super-Tuyau™
============

Le super tuyau transporte du plasma plus rapidement qu'un tuyau et coûte plus cher à détruire.

Débris
======

Ce sont les restes de la destruction d'un tuyau. Du plasma peut en sortir mais pas y
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

Voici un exemple avec deux pulsars :

  11 15 9 5 8
  15 11 9 5 8
