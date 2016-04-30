import api
import sys
from heapq import *

INF = 1000000

def dist(a, b, i, j):
    return(abs(a - i) + abs(b - j))

def dist_base(i, j):
    base = api.ma_base()
    dmin = INF
    for (a, b) in base:
        d = dist(a, b, i, j)
        if d < dmin:
            dmin = d
    return dmin

def partie_init():
    global early_strat, super_tuyau
    early_strat = False
    super_tuyau = True

def partie_fin():
    pass

def print_dist(t):
    N = len(t)
    for i in range(N):
        for j in range(N):
            if t[i][j] < 0:
                print('.', end = '')
            elif t[i][j] < 10:
                print(t[i][j], end = '')
            else:
                print('*', end = '')
        print()
    print()

def print_map():
    N = api.TAILLE_TERRAIN
    for i in range(N):
        for j in range(N):
            c = api.type_case((i,j))
            if api.charges_presentes((i,j)) > 0.0:
                print('*', end = '', file = sys.stdout)
            elif c == api.case_type.TUYAU:
                print('T', end = '', file = sys.stdout)
            elif c == api.case_type.SUPER_TUYAU:
                print('S', end = '', file = sys.stdout)
            elif c == api.case_type.PULSAR:
                print('#', end = '', file = sys.stdout)
            elif c == api.case_type.BASE:
                print('B', end = '', file = sys.stdout)
            elif c == api.case_type.DEBRIS:
                print('D', end = '', file = sys.stdout)
            else:
                print('.', end = '', file = sys.stdout)
        print('', file = sys.stdout)
    print()
    print()
    print()

def jouer_tour():
    # print('###', api.liste_pulsars()[0])
    # print(api.hist_tuyaux_detruits())
    # print('', file = sys.stderr)
    # print_map()
    if api.tour_actuel() % 2 in [0, 1]:
        detruire()
    while api.points_action() >= 10:
        # print(api.points_action(), file = sys.stderr)
        if not coup():
            break
    while api.cout_prochaine_modification_aspiration() <= api.points_action():
        if not bouger_aspiration():
            break
    # print(api.cout_prochaine_modification_aspiration(), api.points_action())
    if api.points_action() >= 30:
        detruire()
def coup():
    # Récupérer les infos sur la grille
    N = api.TAILLE_TERRAIN
    grille = [[0 for _ in range(N)] for _ in range(N)]
    # print("plasma", api.liste_plasmas(), file = sys.stderr)
    # print("pulsars", [api.info_pulsar(i) for i in api.liste_pulsars()], file = sys.stderr)
    for i in range(N):
        for j in range(N):
            grille[i][j] = api.type_case((i,j))
            if api.points_action() != 40:
                pass
            elif False:
                pass
            # elif grille[i][j] == api.case_type.TUYAU: print('T', end = "", file = sys.stderr)
            # elif grille[i][j] == api.case_type.PULSAR: print('#', end = "", file = sys.stderr)
            # else: print('.', end = "", file = sys.stderr)
        # if api.points_action() == 40: print('\n', file = sys.stderr)

    # Trouver les cases connexes à la base
    connected = [[False for _ in range(N)] for _ in range(N)]
    dist_bfs = [[-1 for _ in range(N)] for _ in range(N)]
    base = api.ma_base()
    l = []
    for i, j in base:
        connected[i][j] = True
        dist_bfs[i][j] = 0
        heappush(l, (0, i, j))
    while l != []:
        v, i, j = heappop(l)
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if 0 < a < N - 1 and 0 < b < N - 1:
                if not connected[a][b] and grille[a][b] in [api.case_type.TUYAU, api.case_type.SUPER_TUYAU]:
                    connected[a][b] = True
                    dist_bfs[a][b] = v + 1
                    heappush(l, (v + 1, a, b))
    liste_connected = []
    for i in range(N):
        for j in range(N):
            if connected[i][j]:
                liste_connected.append((i, j))

    # Trouver les cases où il est possible de poser un tuyau ou de déblayer
    poss_tuyau = []
    poss_deblayer = []
    for i, j in liste_connected:
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if 0 < a < N - 1 and 0 < b < N - 1:
                if grille[a][b] == api.case_type.VIDE:
                    poss_tuyau.append((a, b))
                if grille[a][b] == api.case_type.DEBRIS:
                    poss_deblayer.append((a, b))
    poss = poss_tuyau + poss_deblayer
    if poss == []:
        return(False)

    # Trouver les cases vers lesquelles les pulsars émettent et qui ne sont pas reliées à la base
    cases_to_reach = []
    next_emission = []
    for i, j in api.liste_pulsars():
        ip = api.info_pulsar((i, j))
        if ip.nombre_pulsations > 0:
            for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
                if (a, b) not in cases_to_reach and not connected[a][b]:
                    cases_to_reach.append((a, b))
                    next_emission.append((ip.periode - api.tour_actuel()) % ip.periode)
    nb_cases = len(cases_to_reach)
    if nb_cases == 0:
        return(False)

    # Trouver les distances de ces cases à la partie connectée
    distance = [0 for _ in range(nb_cases)]
    for k in range(nb_cases):
        i, j = cases_to_reach[k]
        dist_min = INF
        for a, b in liste_connected:
            dist_min = min(dist_min, dist(i, j, a, b))
        distance[k] = dist_min
    # print("distance", distance, file = sys.stderr)

    # Heuristique déterminant la valeur d'une pose de tuyau ou d'un déblayage
    # print_dist(dist_bfs)
    value = []
    for i, j in poss:
        v = INF
        d_bfs = INF
        for d, e in [(i, j + 1), (i, j - 1), (i + 1, j), (i - 1, j)]:
            if dist_bfs[d][e] >= 0:
                d_bfs = min(d_bfs, dist_bfs[d][e])
        for k in range(nb_cases):
            a, b = cases_to_reach[k]
            di = dist(a, b, i, j)
            new_v = INF
            if di < dist_min:
                new_v = max(di, 4 * next_emission[k])
                new_v = di
                if (i, j) in poss_deblayer:
                    new_v += 1
                new_v += d_bfs / 1.6
            v = min(v, new_v)
        value.append((v, i, j))
    value.sort()
    # print(value)
    # print(value, file = sys.stderr)

    # Poser des super-tuyaux dès le début pour obtenir du plasma au plus vite
    if 6 < api.tour_actuel() < 11:
        parite = -1
        for i, j in api.liste_tuyaux():
            for a, b in api.ma_base():
                if dist(i, j, a, b) < 2:
                    parite = (i + j) % 2
        for i, j in api.liste_tuyaux():
            for a, b in api.ma_base():
                if dist(i, j, a, b) < 13:
                    if (i + j) % 2 == 1 - parite:
                        erreur = api.ameliorer((i, j))
                        if erreur == api.erreur.OK:
                            grille[i][j] = api.case_type.SUPER_TUYAU

    # Poser des super-tuyaux à la fin pour récupérer un maximum de plasma avant la fin de la partie
    if 80 < api.tour_actuel() < 101:
         for _ in range(2):
             a, b, dmin = -1, -1, 1000000
             for i, j in api.liste_tuyaux():
                 dt = 1000000
                 for x, y in api.ma_base():
                     dt = min(dt, dist(x, y, i, j))
                 poss = True
                 if grille[i][j] == api.case_type.SUPER_TUYAU:
                     poss = False
                 for d, e in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
                     if grille[i + d][j + e] == api.case_type.SUPER_TUYAU:
                         poss = False
                 if poss and 1 < dt < dmin:
                     dmin, a, b = dt, i, j
             erreur = api.ameliorer((a, b))
             if erreur == api.erreur.OK:
                 grille[a][b] = api.case_type.SUPER_TUYAU

    while value != []:
        _, i, j = value.pop(0)
        if (i, j) in poss_tuyau:
            if api.tour_actuel() < 5  and early_strat and (i < 16 or i > 24) and (j < 16 or j > 24):
                continue
            # print("construction", i, j, file = sys.stderr)
            erreur = api.construire((i, j))
            if 5 < api.tour_actuel() < 15 and super_tuyau and i != 1 and i != 37 and j != 1 and j != 37:
                poss = True
                for d, e in [(i, j + 1), (i, j - 1), (i + 1, j), (i - 1, j)]:
                    if grille[d][e] == api.case_type.SUPER_TUYAU:
                        poss = False
                # print(dd, api.points_action())
                if poss:
                    api.ameliorer((i, j))
            #if erreur != api.erreur.OK:
            #    print('Construire', erreur, api.type_case((i, j)), file = sys.stderr)
            #else:if (i, j) == (25, 37): print('Construction', i, j, file = sys.stderr)
            return(erreur == api.erreur.OK)
        else:
            if api.points_action() >= 20:
                erreur = api.deblayer((i, j))
                # print('Déblayage', i, j, file = sys.stderr)
                #if erreur != api.erreur.OK:
                #    print("déblayer", erreur, file = sys.stderr)
                return(api.erreur.OK)
    return(False)

def bouger_aspiration():
    # Récupérer les infos sur la grille
    N = api.TAILLE_TERRAIN
    tour = api.tour_actuel()
    grille = [[0 for _ in range(N)] for _ in range(N)]
    for i in range(N):
        for j in range(N):
            grille[i][j] = api.type_case((i,j))

    # Trouver les cases connexes à la base
    connected = [[False for _ in range(N)] for _ in range(N)]
    base = api.ma_base()
    l = []
    for i, j in base:
        connected[i][j] = True
        l.append((i, j))
    while l != []:
        i, j = l.pop()
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if 0 < a < N - 1 and 0 < b < N - 1:
                if not connected[a][b] and grille[a][b] in [api.case_type.TUYAU, api.case_type.SUPER_TUYAU]:
                    connected[a][b] = True
                    l.append((a, b))
    liste_connected = []
    for i in range(N):
        for j in range(N):
            if connected[i][j]:
                liste_connected.append((i, j))

    # Trouver les cases vers lesquelles les pulsars émettent et qui sont reliées à la base
    cases_reached = []
    for i, j in api.liste_pulsars():
        periode, puissance, nb_pulsations = api.info_pulsar((i, j))
        power = puissance / periode
        if nb_pulsations * periode >= api.tour_actuel() - 10:
            for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
                if (a, b) not in cases_reached and connected[a][b]:
                    cases_reached.append((a, b, power))

    # Ordonner les parties de bases par importance
    importance = [0.0] * len(base)
    for i, j, p in cases_reached:
        l = [(i, j, p)]
        while l != []:
            a, b, v = l.pop(0)
            for k in range(len(base)):
                if base[k] == (a, b):
                    importance[k] += v
            if grille[a][b] in [api.case_type.TUYAU, api.case_type.SUPER_TUYAU]:
                next = api.directions_plasma((a, b))
                for x, y in next:
                    l.append((x, y, v / len(next)))
    liste = [(importance[k], k) for k in range(len(base))]
    liste.sort()
    while api.puissance_aspiration(base[liste[0][1]]) == 0:
         liste.pop(0)
    while api.puissance_aspiration(base[liste[len(liste) - 1][1]]) == api.LIMITE_ASPIRATION:
         liste.pop()
    if liste[len(liste)-1][0] > liste[0][0] and len(liste) > 1:
        erreur = api.deplacer_aspiration(base[liste[0][1]], base[liste[len(liste) - 1][1]])
        return(erreur == api.erreur.OK)
    return(False)

def detruire():
    # Choisir le tuyau le plus proche d'une base ennemie et le détruire
    N = api.TAILLE_TERRAIN
    tuyaux = api.liste_tuyaux()
    liste = []
    for i, j in tuyaux:
        poss = False
        nb = 0
        for d, e in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
            if api.charges_presentes((i + d, j + e)):
                poss = True
            if api.est_tuyau((i + d, j + e)) or i + d in [0, 38] or j + e in [0, 38]:
                nb += 1
        if not api.est_super_tuyau((i, j)) and poss and nb == 2:
            min_dist_ma_base = INF
            min_dist_base_ennemie = INF
            for a, b in api.ma_base():
                min_dist_ma_base = min(min_dist_ma_base, dist(i, j, a, b))
            for a, b in api.base_ennemie():
                min_dist_base_ennemie = min(min_dist_base_ennemie, dist(i, j, a, b))
            # value = min_dist_base_ennemie - min_dist_ma_base
            value = min_dist_base_ennemie
            if value < 8:
                liste.append((value, i, j))
    if liste == []:
        return(False)
    liste.sort()
    _, i, j = liste.pop(0)
    # print('Destruction', i, j, api.type_case((i, j)), file = sys.stderr)
    erreur = api.detruire((i, j))
    #if erreur != api.erreur.OK:
    #    print('Détruire', erreur, i, j, file = sys.stderr)
    return(erreur == api.erreur.OK)
