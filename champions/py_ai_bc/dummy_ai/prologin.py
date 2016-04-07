import api

INF = 1000000

def dist(a, b, i, j):
    return(abs(a - i) + abs(b - j))

def partie_init():
    pass

def partie_fin():
    pass

def jouer_tour():
    while api.points_action() >= 10:
        if not coup():
            break
    while api.coup_prochaine_modification_aspiration() <= api.points_action():
        if not bouger_aspiration():
            break
    if api.points_action() >= 30:
        detruire()
def coup():
    # Récupérer les infos sur la grille
    N = api.TAILLE_TERRAIN
    grille = [[0 for _ in range(N)] for _ in range(N)]
    for i in range(N):
        for j in range(N):
            grille[i][j] == api.type_case((i,j))

    # Trouver les cases connexes à la base
    connected = [[False for _ in range(N)] for _ in range(N)]
    base = api.ma_base()
    l = []
    for i, j in base:
        connected[i][j] = True
        l.append[(i, j)]
    while l != []:
        i, j = l.pop()
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if 0 < a < N - 1 and 0 < b < N - 1:
                if not connected[a][b] and grille[a][b] in [api.case_type.tuyau, api.case_type.super_tuyau]:
                    connected[a][b] = True
                    l.append((a, b))
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
                if grille[a][b] == api.case_type.vide:
                    poss_tuyau.append((a, b))
                if grille[a][b] == api.case_type.debris:
                    poss_tuyau.append((a, b))
    poss = poss_tuyau + poss_deblayer
    if poss == []:
        return(False)

    # Trouver les cases vers lesquelles les pulsars émettent et qui ne sont pas reliées à la base
    cases_to_reach = []
    for i, j in api.liste_pulsars():
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if (a, b) not in cases_to_reach and not connected[a][b]:
                cases_to_reach.append((a, b))
    nb_cases = len(cases_to_reach)

    # Trouver les distances de ces cases à la partie connectée
    distance = [0 for _ in range(nb_cases)]
    for k in range(nb_cases):
        i, j = cases_to_reach[k]
        dist_min = INF
        for a, b in liste_connected:
            dist_min = min(dist_min, dist(i, j, a, b)
        distance[k] = dist_min

    # Heuristique déterminant la valeur d'une pose de tuyau ou d'un déblayage
    value = []
    for i, j in poss:
        v = INF
        for k in range(nb_cases):
            a, b = cases_to_reach[k]
            d = dist(a, b, i, j)
            if d < distance[k]:
                v = min(v, d)
        if (i, j) in poss_deblayer:
            v += 2
        value.append((v, i, j))
    value.sort()
    while value != []:
        _, i, j = value.pop()
        if (i, j) in poss_tuyau:
            api.construire((i, j))
            return(True)
        else:
            if api.points_action >= 20:
                api.deblayer((i, j))
                return(True)
    return(False)

def bouger_aspiration():
    # Récupérer les infos sur la grille
    N = api.TAILLE_TERRAIN
    tour = api.tour_actuel()
    grille = [[0 for _ in range(N)] for _ in range(N)]
    for i in range(N):
        for j in range(N):
            grille[i][j] == api.type_case((i,j))

    # Trouver les cases connexes à la base
    connected = [[False for _ in range(N)] for _ in range(N)]
    base = api.ma_base()
    l = []
    for i, j in base:
        connected[i][j] = True
        l.append[(i, j)]
    while l != []:
        i, j = l.pop()
        for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
            if 0 < a < N - 1 and 0 < b < N - 1:
                if not connected[a][b] and grille[a][b] in [api.case_type.tuyau, api.case_type.super_tuyau]:
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
        if nb_pulsations * periode >= tour_actuel - 10:
            for a, b in [(i - 1, j), (i + 1, j),(i, j - 1), (i, j + 1)]:
                if (a, b) not in cases_reached and connected[a][b]:
                    cases_reached.append((a, b, power))

    # Ordonner les parties de bases par importance
    importance = [0.0] * len(base)
    for i, j, p in cases_reached:
        l = [(i, j, p)]
        while l != []:
            a, b, v = l.pop()
            for k in range(len(base)):
                if base[k] == (a, b):
                    importance[k] += v
            if grille[a][b] in [api.case_type.tuyau, api.case_type.super_tuyau]:
                next = api.directions_plasma((a, b))
                for x, y in next:
                    l.append[(x, y, v / len(next))]
    liste = [(importance[k], k) for k in range(len(base))]
    liste.sort()
    if liste[len(liste)-1][0] > liste[0][0]:
        api.deplacer_aspiration(base[liste[0][1]], base[liste[len(liste)-1][1]])
        return(True)
    return(False)

def detruire():
    # Choisir le tuyau le plus proche d'une base ennemie et le détruire
    N = api.TAILLE_TERRAIN
    tuyaux = api.liste_tuyaux()
    liste = []
    for i, j in tuyaux:
        if not api.est_super_tuyau((i, j)):
            min_dist_ma_base = INF
            min_dist_base_ennemie = INF
            for a, b in api.ma_base():
                min_dist_ma_base = min(min_dist_ma_base, dist(i, j, a, b)
            for a, b in api.base_ennemie():
                min_dist_base_ennemie = min(min_dist_base_ennemie, dist(i, j, a, b)
            value = min_dist_base_ennemie - min_dist_ma_base
            liste.append((value, i, j))
    if liste == []:
        return(False)
    liste.sort()
    _, i, j = liste.pop()
    api.detruire((i, j))
    return(True)
