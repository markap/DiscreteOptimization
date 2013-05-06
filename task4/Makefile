#
# Makefile zum Uebersetzen von LEDA-Programmen
#
# z.B. zum Uebersetzen von dfs.cpp einfach "make dfs" eingeben!
#
# Hinweis: Um die kompilierten LEDA-Programme ausfuehren zu koennen,
# muessen die Environment-Variablen LEDAROOT und LD_LIBRARY_PATH
# entsprechend gesetzt sein. Bitte ggf. ändern.
#

GXXROOT  = /usr
# GXX      = $(GXXROOT)/bin/g++
GXX      = $(GXXROOT)/bin/g++-4.6
LEDAROOT = /usr/local/LEDA

.cpp:
	$(GXX) -Wall -m32 -o $* $*.cpp  -I$(LEDAROOT)/incl -I$(GXXROOT)/include -L$(GXXROOT)/lib -L/usr/X11R6/lib -L$(LEDAROOT) -lW -lP -lG -lL -lX11 -lm


