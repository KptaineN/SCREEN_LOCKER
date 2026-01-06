#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define NUM_STARS 200

static const char *levels[] = {"█", "▓", "▒", "░", "·"};

typedef struct s_star {
    float x, y, z;
    int   last_px, last_py;
} Star;

typedef struct s_xctx {
    Display *dpy;
    Window  root;
} XCtx;

/* =========================
   STARFIELD: INIT / UPDATE
   ========================= */

static void init_star(Star *s, int w, int h)
{
    s->x = (float)(rand() % (w * 8) - (w * 4));
    s->y = (float)(rand() % (h * 8) - (h * 4));
    s->z = (float)(rand() % 100 + 1);
    s->last_px = -1;
    s->last_py = -1;
}

static void init_stars(Star stars[], int n, int w, int h)
{
    int i = 0;
    while (i < n)
    {
        init_star(&stars[i], w, h);
        i++;
    }
}

/* =========================
   TERMINAL HELPERS (VISUEL)
   ========================= */

static void get_term_size(struct winsize *w)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, w);
}

static void term_enter_alt_screen(void)
{
    /* alt-screen + hide cursor + clear + home */
    printf("\033[?1049h\033[?25l\033[2J\033[H");
}

static void term_leave_alt_screen(void)
{
    /* restore + show cursor + reset attrs */
    printf("\033[?1049l\033[?25h\033[0m");
}

/* =========================
   DRAW / ANIMATE ONE FRAME
   ========================= */

static void erase_prev_star(const Star *s)
{
    if (s->last_px >= 0)
        printf("\033[%d;%dH ", s->last_py + 1, s->last_px + 1);
}

static void draw_star(Star *s, int sx, int sy)
{
    printf("\033[%d;%dH", sy + 1, sx + 1);
    if (s->z < 25)
        printf("\033[1;37m%s", levels[0]);
    else if (s->z < 50)
        printf("\033[0;37m%s", levels[2]);
    else
        printf("\033[1;30m%s", levels[4]);
    s->last_px = sx;
    s->last_py = sy;
}

static void update_and_draw_star(Star *s, int cols, int rows)
{
    int sx, sy;

    erase_prev_star(s);

    s->z -= 2.5f;
    if (s->z <= 0)
    {
        init_star(s, cols, rows);
        return;
    }

    sx = (int)((s->x / s->z) * 50.0f + (cols / 2));
    sy = (int)((s->y / s->z) * 25.0f + (rows / 2));

    if (sx >= 0 && sx < cols && sy >= 0 && sy < rows)
        draw_star(s, sx, sy);
    else
        s->last_px = -1;
}

static void render_frame(Star stars[], int n, int cols, int rows)
{
    int i = 0;
    while (i < n)
    {
        update_and_draw_star(&stars[i], cols, rows);
        i++;
    }
}

/* =========================
   X11: INIT + EVENTS
   ========================= */

static int x11_init_and_grab(XCtx *x)
{
    x->dpy = XOpenDisplay(NULL);
    if (!x->dpy)
        return 0;
    x->root = DefaultRootWindow(x->dpy);

    /* On ne check pas le status pour garder ton comportement actuel */
    XGrabKeyboard(x->dpy, x->root, False,
                 GrabModeAsync, GrabModeAsync, CurrentTime);
    return 1;
}

static void x11_cleanup(XCtx *x)
{
    if (!x || !x->dpy)
        return;
    XUngrabKeyboard(x->dpy, CurrentTime);
    XCloseDisplay(x->dpy);
    x->dpy = NULL;
}

static void handle_key_sequence(char c, char *last_key, int *locked)
{
    if (c == 'm')
        *last_key = 'm';
    else if (*last_key == 'm' && c == 'p')
        *locked = 0;
    else
        *last_key = 0;
}

static void process_x11_events(Display *dpy, char *last_key, int *locked)
{
    XEvent ev;

    while (XPending(dpy))
    {
        XNextEvent(dpy, &ev);
        if (ev.type == KeyPress)
        {
            KeySym keysym = XLookupKeysym(&ev.xkey, 0);
            char c = (char)keysym; /* identique à ton code */
            handle_key_sequence(c, last_key, locked);
        }
    }
}

/* =========================
   MAIN LOOP
   ========================= */

static void main_loop(XCtx *x, Star stars[])
{
    struct winsize w;
    int locked = 1;
    char last_key = 0;

    while (locked)
    {
        get_term_size(&w);
        render_frame(stars, NUM_STARS, w.ws_col, w.ws_row);
        process_x11_events(x->dpy, &last_key, &locked);

        fflush(stdout);
        usleep(35000);
    }
}

/* =========================
   MAIN
   ========================= */

int main(void)
{
    XCtx x;
    struct winsize w;
    Star stars[NUM_STARS];

    srand(time(NULL));

    if (!x11_init_and_grab(&x))
        return 1;

    term_enter_alt_screen();
    get_term_size(&w);
    init_stars(stars, NUM_STARS, w.ws_col, w.ws_row);

    main_loop(&x, stars);

    x11_cleanup(&x);
    term_leave_alt_screen();
    printf("\nDéverrouillé.\n");
    return 0;
}

