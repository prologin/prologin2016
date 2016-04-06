from tkinter import *
# from tkinter import messagebox # Message avant de quitter
# from tkinter import colorchooser # Pour choisir une couleur
# from tkinter import filedialog # Pour ouvrir/sauvegarder une partie
from math import sqrt  # Pour la fonction racine("sqrt")
from random import randint
from heapq import *

# Dimension de la fenêtre
long = 600
larg = 600

# Constantes
N = 39
nombre_pulsars = N // 3

# Grille
grille = [["none" for j in range(N)] for i in range(N)]

# Positionnement du plateau dans la fenêtre
delta = 30
xbas = larg - delta
ybas = long - delta
xhaut = delta
yhaut = delta
longueur = long - 2 * yhaut
largeur = larg - 2 * xhaut
xdelta = (xbas - xhaut) / N
ydelta = (ybas - yhaut) / N

# Variables de jeu
case_selectionnee = (-1, -1)
fichier = 'map_default'


def click_gauche(event):  # Définit ce qu'il se passe lors de l'appui sur le click gauche
    global case_selectionnee
    x, y = event.x, event.y
    for i in range(N):
        for j in range(N):
            xmin, ymin = xhaut + i * xdelta, yhaut + j * ydelta
            xmax, ymax = xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta
            if xmin <= x <= xmax and ymin <= y <= ymax:
                case_selectionnee = (i, j)
    rafraichir()


def appui_clavier(event):
    global case_selectionnee, grille
    lettre = event.char
    i, j = case_selectionnee
    if lettre in ['\r', '\n']:
        if 0 < i < N-1 and 0 < j < N-1:
            if grille[i][j] == 'pulsar':
                grille[i][j] = 'none'
            else:
                grille[i][j] = 'pulsar'
        elif i + j in [0, N - 1, 2 * N - 2]:
            return
        elif i in [0, N - 1]:
            if grille[i][j] == 'base_1':
                grille[i][j] = 'none'
            else:
                grille[i][j] = 'base_1'
        else:
            if grille[i][j] == 'base_2':
                grille[i][j] = 'none'
            else:
                grille[i][j] = 'base_2'
    if lettre == 'q' and i > 0:
        case_selectionnee = (i - 1, j)
    if lettre == 'd' and i < N - 1:
        case_selectionnee = (i + 1, j)
    if lettre == 'z' and j > 0:
        case_selectionnee = (i, j - 1)
    if lettre == 's' and j < N - 1:
        case_selectionnee = (i, j + 1)
    rafraichir()


def remplir_case(i, j, couleur):
    cadre.create_polygon(xhaut + i * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + (j + 1) * ydelta, fill=couleur, outline="black", width=0.5)


def dessiner_selection(i, j):
    cadre.create_polygon(xhaut + i * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + j * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + (i + 1) * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + (j + 1) * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + i * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + j * ydelta, outline="red", width=3.0)


def rafraichir():
    cadre.delete("all")
    for i in range(N + 1):
        cadre.create_polygon(xhaut, yhaut + longueur * i / N, xbas, yhaut + longueur * i / N, fill="#a76726", outline="black", width=0.5)
    for j in range(N + 1):
        cadre.create_polygon(xhaut + largeur * j / N, yhaut, xhaut + largeur * j / N, ybas, fill="#a76726", outline="black", width=0.5)

    for i in range(N):
        for j in range(N):
            if grille[i][j] == "base_1":
                remplir_case(i, j, "#df6d14")
            if grille[i][j] == "base_2":
                remplir_case(i, j, "#5b3c11")
            if grille[i][j] == "pulsar":
                remplir_case(i, j, "#000000")
    a, b = case_selectionnee
    if 0 <= a <= N and 0 <= b <= N:
        dessiner_selection(a, b)


def initialiser():
    global grille, delta, xbas, ybas, xhaut, yhaut, longueur, largeur, xdelta, ydelta, nombre_pulsars
    grille = [["none" for j in range(N)] for i in range(N)]
    for i in range(N // 3, 2 * N // 3):
        grille[0][i] = "base_1"
        grille[N - 1][i] = "base_1"
        grille[i][0] = "base_2"
        grille[i][N - 1] = "base_2"
    delta = 30
    xbas = larg - delta
    ybas = long - delta
    xhaut = delta
    yhaut = delta
    longueur = long - 2 * yhaut
    largeur = larg - 2 * xhaut
    xdelta = (xbas - xhaut) / N
    ydelta = (ybas - yhaut) / N
    nombre_pulsars = N // 3


def free(i, j):
    global grille
    return(grille[i][j] == "none")


def creation_pulsars():
    global grille
    for k in range(nombre_pulsars):
        i = randint(N // 4, 3 * N // 4)
        j = randint(N // 4, 3 * N // 4)
        if free(i - 1, j) and free(i + 1, j) and free(i, j - 1) and free(i, j + 1):
            grille[i][j] = "pulsar"


def gene():
    global N
    N = int(text_n.get('@0,0', END))
    initialiser()
    creation_pulsars()
    rafraichir()


def start():  # Démarrage de la partie
    initialiser()
    creation_pulsars()
    text.delete('@0,0', END)
    text.insert(INSERT, fichier)
    text_n.delete('@0,0', END)
    text_n.insert(INSERT, str(N))
    rafraichir()


def save():
    s = str(N) + '\n'
    for j in range(N):
        for i in range(N):
            if grille[i][j] in ['base_1', 'base_2']:
                s += 'B'
            elif grille[i][j] == 'pulsar':
                s += '#'
            else:
                s += '.'
        s += '\n'
    fichier = text.get('@0,0', END).split('\n')[0]
    f = open(fichier, 'w')
    f.write(s)
    f.close()


def load():
    global N, grille
    fichier = text.get('@0,0', END).split('\n')[0]
    f = open(fichier, 'r')
    s = f.readlines()
    f.close()
    N = int(s[0])
    text_n.delete('@0,0', END)
    text_n.insert(INSERT, str(N))
    initialiser()
    for i in range(N):
        for j in range(N):
            if s[j + 1][i] == '#':
                grille[i][j] = 'pulsar'
            if s[j + 1][i] == '.':
                grille[i][j] = 'none'
            elif i + j not in [-1, N - 2, 2 * N - 3] and s[j + 1][i] == 'B':
                if i in [0, N - 1]:
                    grille[i][j] = 'base_1'
                else:
                    grille[i][j] = 'base_2'
    rafraichir()

# Création de la fenêtre
gui = Tk()
gui.geometry(str(larg) + "x" + str(long + 30) + "+50+50")
gui.title("Prologin 2016 - Map editor")

# Création du Canvas
cadre = Canvas(gui, width = larg, height = long)
cadre.grid(columnspan = 7, row = 1)
cadre.bind("<Button-1>", click_gauche)
cadre.bind("<Key>", appui_clavier)
cadre.focus_set()

label_text = Label(gui, text = 'N')
label_text.grid(row = 0, column = 0)

text_n = Text(gui, height = 1, width = 5)
text_n.grid(row = 0,column = 1)

button_gene = Button(gui, text = 'GENERATION', command = gene)
button_gene.grid(row = 0, column = 2)

label_text = Label(gui, text = 'File')
label_text.grid(row = 0, column = 3)

text = Text(gui, height = 1, width = 25)
text.grid(row = 0,column = 4)

button_save = Button(gui, text = 'SAVE', command = save)
button_save.grid(row = 0, column = 5)

button_save = Button(gui, text = 'LOAD', command = load)
button_save.grid(row = 0, column = 6)

start()

gui.mainloop()  # Mise en boucle de la fenêtre
