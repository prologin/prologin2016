/// This file has been generated, if you wish to
/// modify it in a permanent way, please refer
/// to the script file : gen/generator_cxx.rb

#include "prologin.hh"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Printing
void printMap(bool color);
void printCase(int x, int y);

// Position helper
position getPosition(int x, int y);
int getSquareDist(position a, position b);

void addConnectedPipes(vector<position> &pipe, position current);
vector<position> getBorderCaseOf(vector<position> &pos, case_type type);
vector<position> getBorderCaseOf(position &pos, case_type type);

void partie_init() {
  cout << "Champion " << moi() << " started" << endl;
}

void jouer_tour() {
  // init
  vector<position> pipe = ma_base();
  for(int i = 0; i < LONGUEUR_BASE * 2; ++i)
    addConnectedPipes(pipe, pipe[i]);
  auto border = getBorderCaseOf(pipe, VIDE);

  for(int a = 0; a < 4; ++a) {
    auto pulsars = liste_pulsars();
    int min = TAILLE_TERRAIN * TAILLE_TERRAIN;
    position nearest;
    for(auto i = border.begin(); i != border.end(); ++i) {
      for(auto u = pulsars.begin(); u != pulsars.end(); ++u) {
        pulsar_info info = info_pulsar(*u);
        if(info.pulsations_restantes > 0
           && getBorderCaseOf(*u, TUYAU).size() == 0
           && getBorderCaseOf(*u, SUPER_TUYAU).size() == 0) {
          int dist = getSquareDist(*i, *u);
          if(dist < min) {
            min = dist;
            nearest = *i;
          }
        }
      } 
    }
    construire(nearest);
    pipe.push_back(nearest);
    border = getBorderCaseOf(pipe, VIDE);
  }
  if(moi() == 1) {
    printMap(false);
    cout << "Score 1 -> " << score(1) << " && 2 -> " << score(2) << endl;
  }
}

void partie_fin() {
}

void printMap(bool color) {
  for(int y = 0; y < TAILLE_TERRAIN; ++y) {
    for(int x = 0; x < TAILLE_TERRAIN; ++x) {
      if(color) {
        printCase(x, y);
      } else {
        switch(type_case(getPosition(x, y))) {
           case VIDE:
            cout << " ";
            break;
          case TUYAU:
            cout << "T";
            break;
          case SUPER_TUYAU:
            cout << "S";
            break;
          case DEBRIS:
            cout << "D";
            break;
          case PULSAR:
            cout << "P";
            break;
          case BASE_JOUEUR:
            cout << "B";
            break;
          case INTERDIT:
            cout << "X";
            break;
        }
      }
    }
    cout << endl;
  }
}

position getPosition(int x, int y) {
  position pos;
  pos.x = x;
  pos.y = y;
  return pos;
}

void printCase(int x, int y) {
  cout << "\033[";
  switch(type_case(getPosition(x, y))) {
    case VIDE:
      cout << "0m ";
      break;
    case TUYAU:
      cout << "32mT";
      break;
    case SUPER_TUYAU:
      cout << "33mS";
      break;
    case DEBRIS:
      cout << "47;37mD";
      break;
    case PULSAR:
      cout << "1;37mP";
      break;
    case BASE_JOUEUR:
      cout << "1;34mB";
      break;
    case INTERDIT:
      cout << "31mX";
    break;
  }
  cout << "\033[0m";
}

void addConnectedPipes(vector<position> &pipe, position current) {
  if(current.x > 0) {
    position p = getPosition(current.x - 1, current.y);
    case_type type = type_case(p);
    if(type == TUYAU || type == SUPER_TUYAU) {
      if(count(pipe.begin(), pipe.end(), p) == 0) {
        pipe.push_back(p);
        addConnectedPipes(pipe, p);
      }
    }
  }
  if(current.x < TAILLE_TERRAIN - 1) {
    position p = getPosition(current.x + 1, current.y);
    case_type type = type_case(p);
    if(type == TUYAU || type == SUPER_TUYAU) {
      if(count(pipe.begin(), pipe.end(), p) == 0) {
        pipe.push_back(p);
        addConnectedPipes(pipe, p);
      }
    }
  }
  if(current.y > 0) {
    position p = getPosition(current.x, current.y - 1);
    case_type type = type_case(p);
    if(type == TUYAU || type == SUPER_TUYAU) {
      if(count(pipe.begin(), pipe.end(), p) == 0) {
        pipe.push_back(p);
        addConnectedPipes(pipe, p);
      }
    }
  }
  if(current.y < TAILLE_TERRAIN - 1) {
    position p = getPosition(current.x, current.y + 1);
    case_type type = type_case(p);
    if(type == TUYAU || type == SUPER_TUYAU) {
      if(count(pipe.begin(), pipe.end(), p) == 0) {
        pipe.push_back(p);
        addConnectedPipes(pipe, p);
      }
    }
  }
}

int getSquareDist(position a, position b) {
  int x = abs(a.x - b.x);
  int y = abs(a.y - b.y);
  return (x * x) + (y * y);
}

vector<position> getBorderCaseOf(vector<position> &pos, case_type type) {
  vector<position> border;
  for(auto i = pos.begin(); i != pos.end(); ++i) {
    if(i->x > 0) {
      position p = getPosition(i->x - 1, i->y);
      if(type_case(p) == type && count(pos.begin(), pos.end(), p) == 0 &&
         count(border.begin(), border.end(), p) == 0) {
        border.push_back(p);
      }
    }
    if(i->x < TAILLE_TERRAIN - 1) {
      position p = getPosition(i->x + 1, i->y);
      if(type_case(p) == type && count(pos.begin(), pos.end(), p) == 0 &&
         count(border.begin(), border.end(), p) == 0) {
        border.push_back(p);
      }
    }
    if(i->y > 0) {
      position p = getPosition(i->x, i->y - 1);
      if(type_case(p) == type && count(pos.begin(), pos.end(), p) == 0 &&
         count(border.begin(), border.end(), p) == 0) {
        border.push_back(p);
      }
    }
    if(i->y < TAILLE_TERRAIN - 1) {
      position p = getPosition(i->x, i->y + 1);
      if(type_case(p) == type && count(pos.begin(), pos.end(), p) == 0 &&
         count(border.begin(), border.end(), p) == 0) {
        border.push_back(p);
      }
    }
  }
  return border;
}

vector<position> getBorderCaseOf(position &pos, case_type type) {
  vector<position> vec(1, pos);
  return getBorderCaseOf(vec, type);
}
