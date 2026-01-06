#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/keysym.h> // Pour reconnaître les touches

#define NUM_STARS 200
const char* levels[] = {"█", "▓", "▒", "░", "·"};
typedef struct { float x, y, z; int last_px, last_py; } Star;

void init_star(Star *s, int w, int h) {
    s->x = (float)(rand() % (w * 8) - (w * 4));
    s->y = (float)(rand() % (h * 8) - (h * 4));
    s->z = (float)(rand() % 100 + 1);
    s->last_px = -1; s->last_py = -1;
}

int main() {
    struct winsize w;
    srand(time(NULL));

    // 1. INITIALISATION X11
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) return 1;
    Window root = DefaultRootWindow(dpy);
    XGrabKeyboard(dpy, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);

    // 2. CONFIG TERMINAL (Uniquement pour le visuel)
    printf("\033[?1049h\033[?25l\033[2J\033[H");
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    Star stars[NUM_STARS];
    for (int i = 0; i < NUM_STARS; i++) init_star(&stars[i], w.ws_col, w.ws_row);

    int locked = 1;
    char last_key = 0;

    while (locked) {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        // Animation (Code inchangé)
        for (int i = 0; i < NUM_STARS; i++) {
            if (stars[i].last_px >= 0) printf("\033[%d;%dH ", stars[i].last_py + 1, stars[i].last_px + 1);
            stars[i].z -= 2.5f;
            if (stars[i].z <= 0) init_star(&stars[i], w.ws_col, w.ws_row);
            int sx = (int)((stars[i].x / stars[i].z) * 50.0f + (w.ws_col / 2));
            int sy = (int)((stars[i].y / stars[i].z) * 25.0f + (w.ws_row / 2));
            if (sx >= 0 && sx < w.ws_col && sy >= 0 && sy < w.ws_row) {
                printf("\033[%d;%dH", sy + 1, sx + 1);
                if (stars[i].z < 25) printf("\033[1;37m%s", levels[0]);
                else if (stars[i].z < 50) printf("\033[0;37m%s", levels[2]);
                else printf("\033[1;30m%s", levels[4]);
                stars[i].last_px = sx; stars[i].last_py = sy;
            } else stars[i].last_px = -1;
        }

        // 3. LECTURE CLAVIER VIA X11 (La correction est ici)
        XEvent ev;
        while(XPending(dpy)) {
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym keysym = XLookupKeysym(&ev.xkey, 0);
                char c = (char)keysym; // On convertit la touche en char
                
                if (c == 'm') last_key = 'm';
                else if (last_key == 'm' && c == 'p') locked = 0;
                else last_key = 0;
            }
        }

        fflush(stdout);
        usleep(35000);
    }

    // 4. LIBÉRATION
    XUngrabKeyboard(dpy, CurrentTime);
    XCloseDisplay(dpy);
    printf("\033[?1049l\033[?25h\033[0m\nDéverrouillé.\n");
    return 0;
}
