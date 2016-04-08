lib_TARGETS = gui

gui-dists = prologin.py

gui-dists += api.py interface.hh
gui-srcs = interface.cc

gui-cxxflags = -fPIC $(shell python3-config --includes)
gui-ldflags = -s $(shell python3-config --ldflags)

STECHEC_LANG=python
include ../includes/rules.mk
