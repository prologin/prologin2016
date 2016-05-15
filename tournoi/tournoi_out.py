#!/usr/bin/python

import django
import os
import sys
import time
from collections import defaultdict

if len(sys.argv) < 2:
    print('Usage: {} tournament_id'.format(sys.argv[0]))
    sys.exit(1)

os.environ['DJANGO_SETTINGS_MODULE'] = 'prologin.concours.settings'
django.setup()

from django.contrib.auth.models import User
from prologin.concours.stechec.models import Tournament, Match, MatchPlayer, Champion, TournamentPlayer, Map

from tournoi_common import get_champions

chs = get_champions()

tournoi = Tournament.objects.get(id=int(sys.argv[1]))
matches = Match.objects.filter(tournament=tournoi)

score = defaultdict(int)
indice = defaultdict(int)
matrix = defaultdict(list)
match_ids = defaultdict(list)
for m in matches:
    c1, c2 = tuple(MatchPlayer.objects.filter(match=m))
    matrix[(c1.champion.id, c2.champion.id)].append(c1.score)
    matrix[(c2.champion.id, c1.champion.id)].append(c2.score)

    match_ids[(c1.champion.id, c2.champion.id)].append(m.id)
    match_ids[(c2.champion.id, c1.champion.id)].append(m.id)

    # Indices
    indice[c1.champion.id] += c1.score
    indice[c2.champion.id] += c2.score

    # Victories
    if c1.score > c2.score:
        score[c1.champion.id] += 2
    elif c2.score > c1.score:
        score[c2.champion.id] += 2
    else:
        score[c1.champion.id] += 1
        score[c2.champion.id] += 1

print('<!DOCTYPE html><head><title>Prologin 2016 : {}</title></head><body>'.format(tournoi.name))

print('''<style>
table, th, td {
    border: 1px solid black;
    border-collapse: collapse
}

th, td {
    padding-left: 10px;
    padding-right: 10px;
}
</style>''')

print('<h1>{}</h1>'.format(tournoi.name))
print('<h2>Classement</h2>')

print('<table>')
print('<thead><td>#</td><td>Victoires</td><td>Nom</td><td>Login</td><td>Nom</td></thead>')
l = chs[:]
l.sort(key=lambda x: -score.get(x.pk, 0))
for i, c in enumerate(l, 1):
    print('<tr>')
    print('<td>{}</td>'.format(i))
    print('<td>{}</td>'.format(score.get(c.pk, 0) // 2))
    print('<td><strong>{}</strong></td>'.format(c.name))
    print('<td>{}</td><td>{}</td>'.format(c.author.username, c.author.get_full_name()))
    print('</tr>')
print('</table>')

#print('<h1>Les scores</h1>')
#
#print('<table>')
#print('<thead>')
#print('<td>VS</td>')
#for i, c in enumerate(l, 1):
#    print('<td>{}</td>'.format(i))
#print('<td>Total</td>')
#print('</thead>')
#
#for i, c1 in enumerate(l, 1):
#    print('<tr><td><strong>{}</strong>'.format(i))
#    for j, c2 in enumerate(l, 1):
#        if c1.pk == c2.pk:
#            print('<td>X</td>')
#        else:
#            color = 'red' if sum(matrix[c1.pk, c2.pk]) < sum(matrix[c2.pk, c1.pk]) else 'green'
#            print('<td style="color: {}">{}</td>'.format(color, sum(matrix[c1.pk, c2.pk])))
#    print('<td><strong>{}</strong></td>'.format(indice.get(c1.pk, 0)))
#    print('</tr>')
#print('</table>')

#print('<h1>Les détails des {} matchs</h1>'.format(matches.count()))
#

for i, c1 in enumerate(l, 1):
    print('<h2>{}. {} (de {})</h2>'.format(i, c1.name, c1.author.username))

    print('<table>')
    print('<thead>')
    print('<td>VS</td>')
    for i in range(2):
        print('<td>Match {}</td>'.format(i + 1))
    print('<td>Total</td>')
    print('</thead>')

    for i, c2 in enumerate(l, 1):
        print('<tr><td><strong>{}</strong>'.format(i))

        for j in range(len(matrix[c1.pk, c2.pk])):
            if c1.pk == c2.pk:
                print('<td>X</td>')
            else:
                color = 'red' if matrix[c1.pk, c2.pk][j] < matrix[c2.pk, c1.pk][j] else 'green'
                print('<td><a style="color: {}" href="http://concours/matches/{}">{}</a></td>'.format(color, match_ids[c1.pk, c2.pk][j], matrix[c1.pk, c2.pk][j]))
        print('<td><strong>{}</strong></td>'.format(sum(matrix[c1.pk, c2.pk])))
        print('</tr>')
    print('</table>')

print('</body></html>')
