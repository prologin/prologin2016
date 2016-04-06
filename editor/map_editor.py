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
couleur_pulsars = ['black', 'red', 'yellow', 'blue', 'green']

# Grille
grille = [["none" for j in range(N)] for i in range(N)]
type_pulsars = [[0 for j in range(N)] for i in range(N)]

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
fichier = 'map_default'
types = [(40, 85, 25), (20, 55, 10), (10, 100, 4), (6, 40, 5), (3, 100, 1)]


def click_gauche(event):  # Définit ce qu'il se passe lors de l'appui sur le click gauche
    case_selectionnee = (-1, -1)
    x, y = event.x, event.y
    for i in range(N):
        for j in range(N):
            xmin, ymin = xhaut + i * xdelta, yhaut + j * ydelta
            xmax, ymax = xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta
            if xmin <= x <= xmax and ymin <= y <= ymax:
                case_selectionnee = (i, j)
    if case_selectionnee != (-1, -1):
        i, j = case_selectionnee
        if 0 < i < N-1 and 0 < j < N-1:
            if grille[i][j] == 'pulsar':
                type_p = 0
                if a_v.get() == 1:
                    type_p = 0
                if b_v.get() == 1:
                    type_p = 1
                if c_v.get() == 1:
                    type_p = 2
                if d_v.get() == 1:
                    type_p = 3
                if e_v.get() == 1:
                    type_p = 4
                if type_p == type_pulsars[i][j]:
                    grille[i][j] = 'none'
                else:
                    type_pulsars[i][j] = type_p
            else:
                grille[i][j] = 'pulsar'
                type_p = 0
                if a_v.get() == 1:
                    type_p = 0
                if b_v.get() == 1:
                    type_p = 1
                if c_v.get() == 1:
                    type_p = 2
                if d_v.get() == 1:
                    type_p = 3
                if e_v.get() == 1:
                    type_p = 4
                type_pulsars[i][j] = type_p
    rafraichir()


def remplir_case(i, j, couleur):
    cadre.create_polygon(xhaut + i * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + j * ydelta, xhaut + (i + 1) * xdelta, yhaut + (j + 1) * ydelta, xhaut + i * xdelta, yhaut + (j + 1) * ydelta, fill=couleur, outline="black", width=0.5)


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
                remplir_case(i, j, couleur_pulsars[type_pulsars[i][j]])
    a_l1.delete('@0,0', END)
    a_l2.delete('@0,0', END)
    a_l3.delete('@0,0', END)
    b_l1.delete('@0,0', END)
    b_l2.delete('@0,0', END)
    b_l3.delete('@0,0', END)
    c_l1.delete('@0,0', END)
    c_l2.delete('@0,0', END)
    c_l3.delete('@0,0', END)
    d_l1.delete('@0,0', END)
    d_l2.delete('@0,0', END)
    d_l3.delete('@0,0', END)
    e_l1.delete('@0,0', END)
    e_l2.delete('@0,0', END)
    e_l3.delete('@0,0', END)
    a_l1.insert('@0,0', types[0][0])
    a_l2.insert('@0,0', types[0][1])
    a_l3.insert('@0,0', types[0][2])
    b_l1.insert('@0,0', types[1][0])
    b_l2.insert('@0,0', types[1][1])
    b_l3.insert('@0,0', types[1][2])
    c_l1.insert('@0,0', types[2][0])
    c_l2.insert('@0,0', types[2][1])
    c_l3.insert('@0,0', types[2][2])
    d_l1.insert('@0,0', types[3][0])
    d_l2.insert('@0,0', types[3][1])
    d_l3.insert('@0,0', types[3][2])
    e_l1.insert('@0,0', types[4][0])
    e_l2.insert('@0,0', types[4][1])
    e_l3.insert('@0,0', types[4][2])


def initialiser():
    global grille, delta, xbas, ybas, xhaut, yhaut, longueur, largeur, xdelta, ydelta, nombre_pulsars
    grille = [["none" for j in range(N)] for i in range(N)]
    type_pulsars = [[0 for j in range(N)] for i in range(N)]
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
    a_v.set(1)
    b_v.set(0)
    c_v.set(0)
    d_v.set(0)
    e_v.set(0)


def free(i, j):
    global grille
    return(grille[i][j] == "none")


def creation_pulsars():
    global grille
    for k in range(nombre_pulsars):
        i = randint(N // 4, 3 * N // 4)
        j = randint(N // 4, 3 * N // 4)
        type_p = randint(0, 4)
        if free(i - 1, j) and free(i + 1, j) and free(i, j - 1) and free(i, j + 1):
            grille[i][j] = "pulsar"
            type_pulsars[i][j] = type_p


def gene():
    global N
    N = int(text_n.get('@0,0', END))
    initialiser()
    creation_pulsars()
    rafraichir()


def clear():
    global N
    N = int(text_n.get('@0,0', END))
    initialiser()
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
            if grille[i][j] == 'pulsar':
                type_p = type_pulsars[i][j]
                a, b, c = types[type_p]
                s += str(j) + ' ' + str(i) + ' ' + str(a) + ' ' + str(b) + ' ' + str(c) + '\n'
    fichier = text.get('@0,0', END).split('\n')[0]
    f = open(fichier, 'w')
    f.write(s)
    print(s)
    f.close()


def load():
    global N, grille
    fichier = text.get('@0,0', END).split('\n')[0]
    f = open(fichier, 'r')
    s = f.readlines()
    f.close()
    N = int(s[0])
    types_found = {}
    text_n.delete('@0,0', END)
    text_n.insert(INSERT, str(N))
    initialiser()
    for k in range(1,len(s)):
        _, _, a, b, c = map(int,s[k].split('\n')[0].split())
        if (a, b, c) in types_found:
            types_found[(a, b, c)] += 1
        else:
            types_found[(a, b, c)] = 1
    liste = []
    for (a, b, c) in types_found:
        liste += [(-types_found[(a, b, c)], a, b, c)]
    liste.sort()
    for k in range(min(5,len(liste))):
        _, a, b, c = liste[k]
        types[k] = (a, b, c)
    for k in range(1,len(s)):
        j, i, a, b, c = map(int,s[k].split('\n')[0].split())
        for type_p in range(5):
            if (a, b, c) == types[type_p]:
                grille[i][j] = 'pulsar'
                type_pulsars[i][j] = type_p
    rafraichir()

def a_c():
    a_v.set(1)
    b_v.set(0)
    c_v.set(0)
    d_v.set(0)
    e_v.set(0)

def b_c():
    a_v.set(0)
    b_v.set(1)
    c_v.set(0)
    d_v.set(0)
    e_v.set(0)

def c_c():
    a_v.set(0)
    b_v.set(0)
    c_v.set(1)
    d_v.set(0)
    e_v.set(0)

def d_c():
    a_v.set(0)
    b_v.set(0)
    c_v.set(0)
    d_v.set(1)
    e_v.set(0)

def e_c():
    a_v.set(0)
    b_v.set(0)
    c_v.set(0)
    d_v.set(0)
    e_v.set(1)

def remplir(cadre_d, couleur):
    cadre_d.create_polygon(0, 0, 20, 0, 20, 20, 0, 20, fill = couleur)

def load_types():
    global types
    f = open('TYPES', 'r')
    s = f.readlines()
    f.close()
    for i in range(5):
        a, b, c = s[i].split('\n')[0].split()
        types[i] = (a, b, c)
    rafraichir()

def update():
    a = int(a_l1.get('@0,0', END))
    b = int(a_l2.get('@0,0', END))
    c = int(a_l3.get('@0,0', END))
    types[0] = (a, b, c)
    a = int(b_l1.get('@0,0', END))
    b = int(b_l2.get('@0,0', END))
    c = int(b_l3.get('@0,0', END))
    types[1] = (a, b, c)
    a = int(c_l1.get('@0,0', END))
    b = int(c_l2.get('@0,0', END))
    c = int(c_l3.get('@0,0', END))
    types[2] = (a, b, c)
    a = int(d_l1.get('@0,0', END))
    b = int(d_l2.get('@0,0', END))
    c = int(d_l3.get('@0,0', END))
    types[3] = (a, b, c)
    a = int(e_l1.get('@0,0', END))
    b = int(e_l2.get('@0,0', END))
    c = int(e_l3.get('@0,0', END))
    types[4] = (a, b, c)

# Création de la fenêtre
gui = Tk()
gui.geometry(str(larg + 190) + "x" + str(long + 30) + "+50+50")
gui.title("Prologin 2016 - Map editor")

# Création du Canvas
cadre = Canvas(gui, width = larg, height = long)
cadre.grid(columnspan = 8, row = 1, rowspan = 6)
cadre.bind("<Button-1>", click_gauche)
cadre.focus_set()

label_text = Label(gui, text = 'N')
label_text.grid(row = 0, column = 0)

text_n = Text(gui, height = 1, width = 5)
text_n.grid(row = 0,column = 1)

button_gene = Button(gui, text = 'CLEAR', command = clear)
button_gene.grid(row = 0, column = 2)

button_gene = Button(gui, text = 'GENERATION', command = gene)
button_gene.grid(row = 0, column = 3)

label_text = Label(gui, text = 'File')
label_text.grid(row = 0, column = 4)

text = Text(gui, height = 1, width = 25)
text.grid(row = 0,column = 5)

button_save = Button(gui, text = 'SAVE', command = save)
button_save.grid(row = 0, column = 6)

button_save = Button(gui, text = 'LOAD', command = load)
button_save.grid(row = 0, column = 7)

label_l = Label(gui, text = "PERIOD TIME POWER")
label_l.grid(row = 1, column = 8, columnspan = 5)

a_v = IntVar()
a = Checkbutton(gui, text='A', variable=a_v, command = a_c)
a.grid(row = 2, column = 8)

b_v = IntVar()
b = Checkbutton(gui, text='B', variable=b_v, command = b_c)
b.grid(row = 3, column = 8)

c_v = IntVar()
c = Checkbutton(gui, text='C', variable=c_v, command = c_c)
c.grid(row = 4, column = 8)

d_v = IntVar()
d = Checkbutton(gui, text='D', variable=d_v, command = d_c)
d.grid(row = 5, column = 8)

e_v = IntVar()
e = Checkbutton(gui, text='E', variable=e_v, command = e_c)
e.grid(row = 6, column = 8)

a_d = Canvas(gui, width = 20, height = 20)
a_d.grid(column = 9, row = 2)
remplir(a_d, 'black')

a_d = Canvas(gui, width = 20, height = 20)
a_d.grid(column = 9, row = 3)
remplir(a_d, 'red')

a_d = Canvas(gui, width = 20, height = 20)
a_d.grid(column = 9, row = 4)
remplir(a_d, 'yellow')

a_d = Canvas(gui, width = 20, height = 20)
a_d.grid(column = 9, row = 5)
remplir(a_d, 'blue')

a_d = Canvas(gui, width = 20, height = 20)
a_d.grid(column = 9, row = 6)
remplir(a_d, 'green')

a_l1 = Text(gui, width = 5, height = 1)
a_l1.grid(row = 2, column = 10)
a_l2 = Text(gui, width = 5, height = 1)
a_l2.grid(row = 2, column = 11)
a_l3 = Text(gui, width = 5, height = 1)
a_l3.grid(row = 2, column = 12)

b_l1 = Text(gui, width = 5, height = 1)
b_l1.grid(row = 3, column = 10)
b_l2 = Text(gui, width = 5, height = 1)
b_l2.grid(row = 3, column = 11)
b_l3 = Text(gui, width = 5, height = 1)
b_l3.grid(row = 3, column = 12)

c_l1 = Text(gui, width = 5, height = 1)
c_l1.grid(row = 4, column = 10)
c_l2 = Text(gui, width = 5, height = 1)
c_l2.grid(row = 4, column = 11)
c_l3 = Text(gui, width = 5, height = 1)
c_l3.grid(row = 4, column = 12)

d_l1 = Text(gui, width = 5, height = 1)
d_l1.grid(row = 5, column = 10)
d_l2 = Text(gui, width = 5, height = 1)
d_l2.grid(row = 5, column = 11)
d_l3 = Text(gui, width = 5, height = 1)
d_l3.grid(row = 5, column = 12)

e_l1 = Text(gui, width = 5, height = 1)
e_l1.grid(row = 6, column = 10)
e_l2 = Text(gui, width = 5, height = 1)
e_l2.grid(row = 6, column = 11)
e_l3 = Text(gui, width = 5, height = 1)
e_l3.grid(row = 6, column = 12)

button_types = Button(gui, text = "LOAD TYPES", command = load_types)
button_types.grid(row = 0, column = 8, columnspan = 3)

button_types = Button(gui, text = "UPDATE", command = update)
button_types.grid(row = 0, column = 11, columnspan = 2)

start()

gui.mainloop()  # Mise en boucle de la fenêtre
