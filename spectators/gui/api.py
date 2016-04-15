# coding: iso-8859-1
from _api import *

# Taille du terrain (longueur et largeur).
TAILLE_TERRAIN = 39

# Longueur de chacune de vos deux bases.
LONGUEUR_BASE = 13

# Nombre de tours à jouer avant la fin de la partie.
NB_TOURS = 100

# Nombre de points d'action par tour.
NB_POINTS_ACTION = 40

# Nombre de points d'action que coûte la construction d'un tuyau.
COUT_CONSTRUCTION_TUYAU = 10

# Nombre de points d'action que coûte l'amélioration d'un tuyau.
COUT_AMELIORATION_TUYAU = 10

# Nombre de points d'action que coûte la destruction d'un tuyau.
COUT_DESTRUCTION_TUYAU = 30

# Nombre de points d'action que coûte la destruction d'un super-tuyau.
COUT_DESTRUCTION_SUPER_TUYAU = 40

# Nombre de points d'action que coûte le déplacement d'un point d'énergie de la base (la première modification de chaque tour est offerte).
COUT_MODIFICATION_ASPIRATION = 10

# Nombre de points d'action que coûte le déblayage d'une case de débris.
COUT_DEBLAYAGE = 20

# Vitesse du plasma dans un tuyau normal, en nombre de cases par tour.
VITESSE_TUYAU = 1

# Multiplicateur de la vitesse du plasma dans un super-tuyau.
MULTIPLICATEUR_VITESSE_SUPER_TUYAU = 2


from enum import IntEnum

# Erreurs possibles
class erreur(IntEnum):
    OK = 0  # <- L'action a été exécutée avec succès.
    PA_INSUFFISANTS = 1  # <- Vous ne possédez pas assez de points d'action pour cette action.
    AUCUN_TUYAU = 2  # <- Il n'y a pas de tuyau à la position spécifiée.
    POSITION_INVALIDE = 3  # <- La position spécifiée est hors de la carte.
    PE_INSUFFISANTS = 4  # <- Vous ne possédez pas assez de points d'énergie sur cette partie de la base.
    PAS_DANS_BASE = 5  # <- Cette case n'appartient pas à votre base.
    AMELIORATION_IMPOSSIBLE = 6  # <- Il y a déjà un super-tuyau sur cette case.
    CONSTRUCTION_IMPOSSIBLE = 7  # <- Il est impossible de construire un tuyau à la position indiquée.
    DESTRUCTION_IMPOSSIBLE = 8  # <- Il n'y a pas de tuyau à la position spécifiée.
    PAS_DE_PULSAR = 9  # <- Il n'y a pas de pulsar à la position spécifiée.
    PAS_DE_DEBRIS = 10  # <- Il n'y a pas de débris à la position spécifiée.


# Types de cases
class case_type(IntEnum):
    VIDE = 0  # <- Case vide
    TUYAU = 1  # <- Case contenant un tuyau
    SUPER_TUYAU = 2  # <- Case contenant un super-tuyau
    DEBRIS = 3  # <- Case contenant des débris à déblayer
    PULSAR = 4  # <- Case contenant un pulsar
    BASE = 5  # <- Case appartenant à une base d'un des joueurs
    INTERDIT = 6  # <- Case où aucune action n'est possible


from collections import namedtuple

# Position sur la carte, donnée par deux coordonnées.

# Représente un pulsar existant.
pulsar_info = namedtuple("pulsar_info",
    'periode ' # <- Période de pulsation du pulsar
    'puissance ' # <- Quantité de plasma émise par chaque pulsation dans chaque direction
    'nombre_pulsations ' # <- Nombre de pulsations restantes
)


