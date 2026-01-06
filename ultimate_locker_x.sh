#!/bin/bash
# On compile
# gcc -O3 ultimate_locker_x.c -o ultimate_locker_x -lX11

# On vérifie xdpyinfo
WIDTH=$(xdpyinfo | grep dimensions | awk '{print $2}' | cut -d 'x' -f1)
HEIGHT=$(xdpyinfo | grep dimensions | awk '{print $2}' | cut -d 'x' -f2)

# Lancement XTERM
# J'ai enlevé l'OverrideRedirect pour le test. Remets-le seulement si MP fonctionne.
xterm -fullscreen \
      -b 0 \
      -geometry ${WIDTH}x${HEIGHT}+0+0 \
      -e ./ultimate_locker_x
