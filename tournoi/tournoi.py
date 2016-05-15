#!/usr/bin/python

import django
import os
import sys
import time

os.environ['DJANGO_SETTINGS_MODULE'] = 'prologin.concours.settings'
django.setup()

from django.contrib.auth.models import User
from prologin.concours.stechec.models import Tournament, Match, MatchPlayer, Champion, TournamentPlayer, Map

from tournoi_common import get_champions

maps_ids = (1,19,20,22)
maps = [Map.objects.get(pk=i) for i in maps_ids]

prologin = User.objects.get(username="seirl")
tournoi = Tournament.objects.create()

print('Launching tournament {}'.format(tournoi.id))

def lancer_match(c1, c2, map):
    m = Match(author=prologin,
              tournament=tournoi)
    m.map = map.path
    m.status = 'new'
    m.save()
    MatchPlayer(champion=c1, match=m).save()
    MatchPlayer(champion=c2, match=m).save()
    return m.id

chs = get_champions()

print()
print('Champions are :')
for ch in chs:
    print('-', ch)

print()
print('Maps are :')
for m in maps:
    print('-', m)

input()
print()
print('Launching matches...')
for c1 in chs:
    for c2 in chs:
        for map in maps:
           if c1.id == c2.id:
               continue
           print(c1.id, '-', c2.id)
           lancer_match(c1, c2, map)
