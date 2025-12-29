🌌 THE BLACK HOLE / ULTIMATE STARFIELD LOCKER
Linux • X11 • Terminal • Low-Level Input Control
📖 Introduction

The Black Hole Locker (alias Ultimate Starfield Locker) est un projet expérimental écrit en C, né d’un défi précis :

Transformer une simple animation Starfield en un écran de verrouillage quasi infranchissable,
capable de neutraliser les vecteurs d’évasion classiques
(Ctrl+C, Alt+F4, Alt+Tab, focus loss)
sans droits administrateur (no sudo).

Le programme combine :

un rendu visuel immersif en terminal (ANSI / Unicode),

une capture clavier bas niveau via X11,

et une logique de déverrouillage par séquence secrète.

Ce projet n’est pas un vrai lockscreen sécurisé système,
mais un proof-of-concept avancé montrant jusqu’où on peut aller sans privilèges root, uniquement avec X11 et le terminal.

🛠️ L’Évolution — Problèmes rencontrés & ruses employées
1️⃣ Le faux plein écran : l’illusion contrôlée

Problème
En C standard, il est impossible de forcer une application terminal en plein écran natif.

Solution

Activation du buffer alternatif ANSI :

\033[?1049h


Masquage du curseur et nettoyage visuel.

Lancement dans un xterm brut, sans décorations.

Ruse

Utilisation de xterm avec :

-xrm 'xterm.overrideRedirect: true'


→ le Window Manager n’ajoute ni bordure, ni barre de titre, ni gestion de focus.

➡️ Résultat : une interface stealth, au-dessus de tout.

2️⃣ Le combat contre le Window Manager (Alt+F4 / Alt+Tab)

Problème
GNOME / KDE interceptent les raccourcis système avant qu’ils n’atteignent ton programme.

Tentative échouée

Ignorer les signaux POSIX (SIGINT, SIGTERM) n’est pas suffisant :

le WM tue la fenêtre, pas juste le process.

Solution ultime : X11 Keyboard Grab

XGrabKeyboard(dpy, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);


➡️ Le serveur X11 redirige tout le flux clavier matériel vers ton programme.
➡️ Le Window Manager devient sourd.

3️⃣ Le bug du “Clavier Muet”

Problème
Une fois le clavier capturé par X11 :

read(), stdin, termios → inutilisables

la séquence secrète (m puis p) ne fonctionne plus

Ruse
Passage complet à la boucle d’événements X11 :

while (XPending(dpy)) {
    XNextEvent(dpy, &ev);
    if (ev.type == KeyPress) {
        KeySym keysym = XLookupKeysym(&ev.xkey, 0);
    }
}


➡️ On lit directement les signaux matériels du clavier,
sans passer par le terminal.

🧬 Anatomie du Code — Les 4 Piliers
A. 🛡️ La couche X11 — La Sécurité
Display *dpy = XOpenDisplay(NULL);
XGrabKeyboard(dpy, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);


Capture exclusive du clavier

Neutralisation des touches système (Alt, Super, F4…)

Lecture directe des événements KeyPress

B. 🌠 Le moteur physique — L’Animation

Le Starfield repose sur une projection 3D → 2D réelle :

x_screen = (x_3D / z_3D) * focal + center_x
y_screen = (y_3D / z_3D) * focal + center_y


z diminue → l’étoile “s’approche”

perspective naturelle par division

illusion de vitesse et de profondeur

Ruse visuelle

Caractères Unicode :

█ ▓ ▒ ░ ·


Couleurs ANSI :

étoiles lointaines → sombres

étoiles proches → brillantes

C. 🔐 La boucle d’écoute — Le Secret
if (c == 'm') last_key = 'm';
else if (last_key == 'm' && c == 'p') locked = 0;
else last_key = 0;


Détection séquentielle, pas instantanée

Toute erreur reset la progression

Le programme reste bloqué tant que la combinaison exacte n’est pas entrée

D. 🧹 Le nettoyage — La Restauration
XUngrabKeyboard(dpy, CurrentTime);
printf("\033[?1049l\033[?25h");


⚠️ Partie critique

Rend le clavier au système

Restaure le terminal normal

Évite de “casser” la session utilisateur

🚀 Commandes essentielles
Action	Commande
Installation	sudo apt install libx11-dev xterm x11-utils
Compilation	gcc -O3 ultimate_locker.c -o locker -lX11
Lancement	xterm -xrm 'xterm.overrideRedirect: true' -fullscreen -e ./locker

⚠️ Important :
-lX11 doit être à la fin (ordre du linker).

⚠️ Leçons apprises

🔑 Le terminal n’est qu’une surcouche
→ pour bloquer vraiment, il faut parler au serveur graphique

🧠 X11 > stdin pour la capture clavier

🧨 Toujours prévoir une porte de secours

Ctrl + Alt + F3 (TTY)

pkill locker

🧭 Limitations & Disclaimer

❌ Pas un lockscreen système sécurisé

❌ Dépend fortement de X11 (Wayland = comportement variable)

✔️ Excellent exercice bas niveau, architecture, événements, input handling

🧪 Pistes d’amélioration

Séquence configurable (argv)

Hash + mot de passe

Timeout / idle detection

Gestion propre de tous les signaux

Mode “panic exit” caché

Motion blur / traînées d’étoiles

🏁 Conclusion

Ce projet démontre qu’avec :

du C pur

un terminal

et une compréhension fine de X11

on peut détourner des mécanismes bas niveau pour créer une illusion de contrôle total, sans privilèges élevés.

Le système pense toujours être maître.
Mais parfois… on peut lui faire croire le contraire.
