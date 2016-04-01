from tkinter import *
# from tkinter import messagebox # Message avant de quitter
# from tkinter import colorchooser # Pour choisir une couleur
# from tkinter import filedialog # Pour ouvrir/sauvegarder une partie
from math import sqrt  # Pour la fonction racine("sqrt")
from random import randint
from heapq import *

# Dimension de la fenêtre
long = 600
larg = 1000

# Constantes
N = 15
nombre_pulsars = N // 3
max_aspiration = 3
temps_destruction = 3

# Grille
grille = [["none" for j in range(N)] for i in range(N)]
energie = [[0.0 for j in range(N)] for i in range(N)]
aspiration = [[0 for j in range(N)] for i in range(N)]
destruction = [[0 for j in range(N)] for i in range(N)]
change = False

# Positionnement du plateau dans la fenêtre
delta = 60
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
mode = "none"
points_action = 0
points = [0, 0]


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


def generation_distance_base():
    global grille
    distance_base = [[1000 for j in range(N)] for i in range(N)]
    pq = []
    for i in range(N):
        for j in range(N):
            if grille[i][j] in ["base_1", "base_2"]:
                distance_base[i][j] = -aspiration[i][j]
                heappush(pq, (distance_base[i][j], i, j))
    while pq != []:
        v, i, j = heappop(pq)
        for d, e in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]:
            if 0 <= d < N and 0 <= e < N and grille[d][e] in ["tuyau", "super_tuyau"]:
                if distance_base[d][e] == 1000:
                    distance_base[d][e] = v + 1
                    heappush(pq, (v + 1, d, e))
    return(distance_base)


def move():
    global grille, energie, points, destruction, grille
    for i in range(N):
        for j in range(N):
            if destruction[i][j] > 0:
                destruction[i][j] -= 1
                if destruction[i][j] == 0:
                    grille[i][j] = "none"
    energie2 = energie.copy()
    energie = [[0.0 for j in range(N)] for i in range(N)]
    energie3 = [[0.0 for j in range(N)] for i in range(N)]
    distance_base = generation_distance_base()
    for i in range(N):
        for j in range(N):
            if energie2[i][j] > 0:
                dir = []
                pcc = 1000000000
                for d, e in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]:
                    lc = distance_base[d][e]
                    if lc != 1000:
                        if lc < pcc:
                            dir = [(d, e)]
                            pcc = lc
                        elif lc == pcc:
                            dir.append((d, e))
                if grille[i][j] == "tuyau":
                    for d, e in dir:
                        energie[d][e] += energie2[i][j] / len(dir)
                else:
                    for d, e in dir:
                        energie3[d][e] += energie2[i][j] / len(dir)
    for i in range(N):
        for j in range(N):
            if energie3[i][j] > 0:
                dir = []
                pcc = 1000000000
                for d, e in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]:
                    lc = distance_base[d][e]
                    if lc != 1000:
                        if lc < pcc:
                            dir = [(d, e)]
                            pcc = lc
                        elif lc == pcc:
                            dir.append((d, e))
                for d, e in dir:
                    energie[d][e] += energie3[i][j] / len(dir)
    for i in range(N):
        for j in range(N):
            if grille[i][j] == "pulsar":
                for d, e in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]:
                    if grille[d][e] in ["tuyau", "super_tuyau"]:
                        energie[d][e] += 1.0
    for i in range(N):
        for j in range(N):
            if grille[i][j] == "base_1":
                points[0] += energie[i][j]
                energie[i][j] = 0
            if grille[i][j] == "base_2":
                points[1] += energie[i][j]
                energie[i][j] = 0


def appui_clavier(event):
    global points_action, case_selectionnee, grille, mode, change, max_aspiration, destruction, temps_destruction
    lettre = event.char
    i, j = case_selectionnee
    if change:
        if lettre == 'a':
            if (mode, grille[i][j]) in [("Joueur 1", "base_1"), ("Joueur 2", "base_2")]:
                if aspiration[i][j] < max_aspiration:
                    aspiration[i][j] += 1
                    change = False
        rafraichir()
        return
    if lettre == 'c' and points_action >= 10 and 0 < i < N - 1 and 0 < j < N - 1:
        if grille[i][j] == "none":
            grille[i][j] = "tuyau"
            points_action -= 10
    if lettre == 'i' and points_action >= 25:
        if grille[i][j] == "tuyau":
            grille[i][j] = "super_tuyau"
            points_action -= 25
    if lettre == 'a' and points_action >= 5:
        print(mode, grille[i][j])
        if (mode, grille[i][j]) in [("Joueur 1", "base_1"), ("Joueur 2", "base_2")]:
            if aspiration[i][j] > 0:
                aspiration[i][j] -= 1
                change = True
                points_action -= 5
    if lettre == 'o':
        if destruction[i][j] == 0:
            if grille[i][j] == "tuyau" and points_action >= 25:
                destruction[i][j] = temps_destruction
                points_action -= 25
            if grille[i][j] == "super_tuyau" and points_action >= 40:
                destruction[i][j] = temps_destruction
                points_action -= 40
    if lettre in ['\r', '\n']:
        if mode == "Joueur 1":
            mode = "Joueur 2"
        else:
            mode = "Joueur 1"
        points_action = 42
        move()
    if lettre == 'q' and i > 0:
        case_selectionnee = (i - 1, j)
    if lettre == 'd' and i < N - 1:
        case_selectionnee = (i + 1, j)
    if lettre == 'z' and j > 0:
        case_selectionnee = (i, j - 1)
    if lettre == 's' and j < N - 1:
        case_selectionnee = (i, j + 1)
    rafraichir()


def click_droit(event):  # Définit ce qu'il se passe lors de l'appui sur le click gauche
    print("2")


def remplir_case(i, j, couleur):
    cadre.create_polygon(xhaut + i * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + (j + 1) * ydelta, fill=couleur, outline="black", width=0.5)


def dessiner_energie(i, j):
    couleur = "#f0c300"
    dy = 1 / 5
    dx = 1 / 5
    cadre.create_oval(xhaut + (i + dx) * xdelta, yhaut + (j + dy) * ydelta, xhaut + (i + 1 - dx) * xdelta, yhaut + (j + 1 - dy) * ydelta, fill=couleur, outline="black", width=0.5)


def dessiner_selection(i, j):
    cadre.create_polygon(xhaut + i * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + j * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + (i + 1) * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + (j + 1) * ydelta, outline="red", width=3.0)
    cadre.create_polygon(xhaut + i * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + j * ydelta, outline="red", width=3.0)


def representer_aspiration(i, j):
    global max_aspiration, aspiration
    x1, y1 = xhaut + i * xdelta, yhaut + j * ydelta
    x2, y2 = xhaut + (i + 1 / max_aspiration) * xdelta, yhaut + j * ydelta
    x3, y3 = xhaut + (i + 1 / max_aspiration) * xdelta, yhaut + (j + 1 / max_aspiration) * ydelta
    x4, y4 = xhaut + i * xdelta, yhaut + (j + 1 / max_aspiration) * ydelta
    idelta = xdelta / max_aspiration
    jdelta = ydelta / max_aspiration
    for k in range(aspiration[i][j]):
        cadre.create_polygon(x1 + k * idelta, y1, x2 + k * idelta, y2, x3 + k * idelta, y3, x4 + k * idelta, y4, fill="red", outline="black", width=0.5)


def representer_destruction(i, j):
    global destruction
    x1, y1 = xhaut + i * xdelta, yhaut + j * ydelta
    x2, y2 = xhaut + (i + 1 / max_aspiration) * xdelta, yhaut + j * ydelta
    x3, y3 = xhaut + (i + 1 / max_aspiration) * xdelta, yhaut + (j + 1 / max_aspiration) * ydelta
    x4, y4 = xhaut + i * xdelta, yhaut + (j + 1 / max_aspiration) * ydelta
    idelta = xdelta / max_aspiration
    jdelta = ydelta / max_aspiration
    for k in range(destruction[i][j]):
        cadre.create_polygon(x1 + k * idelta, y1, x2 + k * idelta, y2, x3 + k * idelta, y3, x4 + k * idelta, y4, fill="black", outline="black", width=0.5)


def rafraichir():
    global case_selectionnee, points_action, mode, energie
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
            if grille[i][j] == "tuyau":
                remplir_case(i, j, "#357ab7")
            if grille[i][j] == "super_tuyau":
                remplir_case(i, j, "#79f8f8")
            if grille[i][j] == "pulsar":
                remplir_case(i, j, "#000000")
            if energie[i][j] > 0.0:
                dessiner_energie(i, j)
    for i in range(N):
        for j in range(N):
            representer_aspiration(i, j)
            representer_destruction(i, j)
    a, b = case_selectionnee
    if 0 <= a <= N and 0 <= b <= N:
        dessiner_selection(a, b)
    p1 = str(int(points[0])) + "." + str(int(points[0] * 10) - 10 * int(points[0]))
    p2 = str(int(points[1])) + "." + str(int(points[1] * 10) - 10 * int(points[1]))
    couleur = "#df6d14" if mode == "Joueur 1" else "#5b3c11"
    cadre.create_text(larg // 2, yhaut // 2, text=mode + "               Points d'action restant : " + str(points_action), fill=couleur, font="Arial 30 italic")
    cadre.create_text(larg // 2, long - yhaut // 2, text="Joueur 1     " + p1 + " - " + p2 + "     Joueur 2", font="Arial 30 italic")


def initialiser():
    global grille, aspiration, energie, destruction, change
    grille = [["none" for j in range(N)] for i in range(N)]
    energie = [[0.0 for j in range(N)] for i in range(N)]
    aspiration = [[0 for j in range(N)] for i in range(N)]
    destruction = [[0 for j in range(N)] for i in range(N)]
    change = False
    for i in range(N // 3, 2 * N // 3):
        grille[0][i] = "base_1"
        grille[N - 1][i] = "base_1"
        grille[i][0] = "base_2"
        grille[i][N - 1] = "base_2"
        aspiration[0][i] = 1
        aspiration[N - 1][i] = 1
        aspiration[i][0] = 1
        aspiration[i][N - 1] = 1


def free(i, j):
    global grille
    return(grille[i][j] == "none")


def creation_pulsars():
    global nombre_pulsars, grille
    for k in range(nombre_pulsars):
        i = randint(N // 4, 3 * N // 4)
        j = randint(N // 4, 3 * N // 4)
        if free(i - 1, j) and free(i + 1, j) and free(i, j - 1) and free(i, j + 1):
            grille[i][j] = "pulsar"


def start():  # Démarrage de la partie
    global mode, points_action
    initialiser()
    mode = "Joueur 1"
    points_action = 42
    creation_pulsars()
    rafraichir()

# Création de la fenêtre
gui = Tk()
gui.geometry(str(larg) + "x" + str(long) + "+50+50")
gui.title("Prologin Finale 2016 - Prototype")

# Création du Canvas
cadre = Canvas(gui, width=larg, height=long)  # ,bg="#ffffff")
cadre.pack()
cadre.bind("<Button-1>", click_gauche)
# cadre.bind("<Button-3>",click_droit)
cadre.bind("<Key>", appui_clavier)
# cadre.bind("<Motion>",position_souris)
cadre.focus_set()
#cadre.grid(row=1,columnspan=1)

menubar = Menu(gui)

filemenu = Menu(menubar, tearoff=0)
filemenu.add_command(label="Nouvelle Partie", command=start)
menubar.add_cascade(label="Fichier", menu=filemenu)

gui.config(menu=menubar)

start()

gui.mainloop()  # Mise en boucle de la fenêtre
