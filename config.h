/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>


/* Appearance */
static const unsigned int borderpx = 2;     /* border pixel of windows */
static const unsigned int snap = 32;        /* snap pixel */

static const int showbar = 1;               /* 0 means no bar */
static const int topbar = 1;                /* 0 means bottom bar */

static const char *fonts[] = {
    "DejaVuSansM Nerd Font Mono:size=14"
};

static const char dmenufont[] = 
    "DejaVuSansM Nerd Font Mono:size=18";


/* Systray */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */

static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray = 1;               /* 0 means no systray */


/* Window Management */
static const unsigned int attachbelow = 1;
static const unsigned int gappx = 8;


/* Multimonitor */
// Index of preferred layout, if LENGTH(lpm)<#monitors -> default layout
static const int lpm[] = {0, 1};


/* Theme (XRDB Fallback) */
// DWM
static char dwm_normbgcolor[]        = "#222222";
static char dwm_normbordercolor[]    = "#444444";
static char dwm_normfgcolor[]        = "#bbbbbb";

static char dwm_selbgcolor[]         = "#005577";
static char dwm_selbordercolor[]     = "#005577";
static char dwm_selfgcolor[]         = "#eeeeee";

static char dwm_titlebgcolor[]       = "#222222";
static char dwm_titlebordercolor[]   = "#222222";
static char dwm_titlefgcolor[]       = "#bbbbbb";

// DMENU
static char dmenu_normbgcolor[]      = "#222222";
static char dmenu_selbgcolor[]       = "#eeeeee";
static char dmenu_normfontcolor[]    = "#eeeeee";
static char dmenu_selfontcolor[]     = "#005577";


/* Color Schemes */
static char *colors[][3] = {
/*                    Foreground        Background        Border                */
    [SchemeNorm]  = { dwm_normfgcolor,  dwm_normbgcolor,  dwm_normbordercolor   },
    [SchemeSel]   = { dwm_selfgcolor,   dwm_selbgcolor,   dwm_selbordercolor    },
    [SchemeTitle] = { dwm_titlefgcolor, dwm_titlebgcolor, dwm_titlebordercolor  },
};


/* Tags */
static const char *tags[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

static const int taglayouts[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0
};


/* Rules */
static const Rule rules[] = {
	// xprop(1):
	//  *	WM_CLASS(STRING) = instance, class
	//  *	WM_NAME(STRING) = title
	// class, instance, title, tags mask, isfloating, monitor
	{ "Gimp", NULL, NULL, 0, 1, -1 },
	{ "Firefox", NULL, NULL, 1 << 8, 0, -1 },
};


/* Layout Configuration */
static const float mfact = 0.55;        /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;           /* number of clients in master area */
static const int resizehints = 0;       /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1;    /* 1 will force focus on the fullscreen window */


/* Layout Definitions */
static const Layout layouts[] = {
	/* symbol, arrange function */
	{ ":T:", tile }, /* first entry is default */
	{ ":F:", NULL }, /* no layout function means floating behavior */
	{ ":M:", monocle },
    { ":G:", grid},
    { ":C:", centeredmaster },
	{ ":c:", centeredfloatingmaster },
};


/* Macros */
#define MODKEY Mod4Mask

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

// Helper for spawning shell commands in the pre dwm-5.0 fashion
#define SHCMD(cmd) \
    { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

// Notification Helpers
#define BRIGHTNESS_NOTIFICATION \
"notify-send -a 'Backlight' -h string:x-dunst-stack-tag:backlight -h int:value:$(brightnessctl i -m | cut -d, -f4 | tr -d '%') 'Brightness'"


/* Commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */

// Dmenu
static const char *dmenucmd[] = {
    "dmenu_run",
    "-p","Run: ",
    "-l","10",
    "-g","4",
    "-bw", "3",
    "-dw", "1400",
    "-c",
    "-fn", dmenufont,
    "-nb", dmenu_normbgcolor,   /* Normal Background */
    "-nf", dmenu_normfontcolor, /* Normal Text */  
    "-sb", dmenu_selbgcolor,    /* Selected Background */
    "-sf", dmenu_selfontcolor,  /* Selected Text */
    NULL
};

// Applications
static const char *thunarcmd[]  = {"thunar", NULL };
static const char *librewolfcmd[] = {"librewolf", NULL};
static const char *termcmd[]  = { "alacritty", NULL };


/* Keybindings */
static const Key keys[] = {
    /* Launchers */
    { MODKEY, XK_Return, spawn, {.v = termcmd } },
    { MODKEY, XK_d, spawn, {.v = dmenucmd} },
    { MODKEY|ShiftMask, XK_d, spawn, {.v = thunarcmd } },
    { MODKEY|ShiftMask,	XK_f, spawn, {.v = librewolfcmd} },

    /* Layouts */
    { MODKEY, XK_F1, setlayout, {.v = &layouts[0]} },
    { MODKEY, XK_F2, setlayout, {.v = &layouts[1]} },
    { MODKEY, XK_F3, setlayout, {.v = &layouts[2]} },
    { MODKEY, XK_F4, setlayout, {.v = &layouts[3]} },
    { MODKEY, XK_F5, setlayout, {.v = &layouts[4]} },
	{ MODKEY, XK_F6, setlayout, {.v = &layouts[5]} },
    { MODKEY, XK_space, setlayout, {0} },

    /* Floating Toggle */
    { MODKEY|ShiftMask, XK_space, togglefloating, {0} },

    /* Window Focus */
    { MODKEY, XK_j, focusstack, {.i = +1 } },
    { MODKEY, XK_k, focusstack, {.i = -1 } },

    /* Move Stack */
    { MODKEY|ShiftMask, XK_j, movestack, {.i = +1 } },
    { MODKEY|ShiftMask, XK_k, movestack, {.i = -1 } },

    /* Master Size */
    { MODKEY, XK_h, setmfact, {.f = -0.05} },
    { MODKEY, XK_l, setmfact, {.f = +0.05} },

    /* Toggle Bar */
    { MODKEY, XK_b, togglebar, {0} },

    /* Zoom (swap master) */
    { MODKEY|ShiftMask, XK_Return, zoom, {0} },

    /* Kill Window */
    { MODKEY|ShiftMask, XK_q, killclient, {0} },

    /* Monitor Focus */
    { MODKEY, XK_comma, focusmon, {.i = -1 } },
    { MODKEY, XK_period, focusmon, {.i = +1 } },

    /* Move Windows to Monitor */
    { MODKEY|ShiftMask, XK_comma, tagmon, {.i = -1 } },
    { MODKEY|ShiftMask, XK_period, tagmon, {.i = +1 } },

    /* DWM Exit Menu */
    { MODKEY|ShiftMask, XK_e, exitdwm, {0} },

    /* Tags */
    TAGKEYS( XK_1, 0 )
    TAGKEYS( XK_2, 1 )
    TAGKEYS( XK_3, 2 )
    TAGKEYS( XK_4, 3 )
    TAGKEYS( XK_5, 4 )
    TAGKEYS( XK_6, 5 )
    TAGKEYS( XK_7, 6 )
    TAGKEYS( XK_8, 7 )
    TAGKEYS( XK_9, 8 )

    /* INCN Master - Number of Client Window */
    { MODKEY, XK_i, incnmaster, {.i = +1 } },
    { MODKEY, XK_p, incnmaster, {.i = -1 } },

    /* Gaps */
    { MODKEY|ShiftMask, XK_F11, setgaps, {.i = +1 } }, // increase
    { MODKEY|ShiftMask, XK_F12, setgaps, {.i = -1 } }, // decrease
    
    /* Screen Brightness */
    { 0, XF86XK_MonBrightnessUp, spawn,
        SHCMD("/usr/bin/brightnessctl set +5% && " BRIGHTNESS_NOTIFICATION) },

    { 0, XF86XK_MonBrightnessDown,spawn,
        SHCMD("/usr/bin/brightnessctl set 5%- && " BRIGHTNESS_NOTIFICATION) },
    
    /* Audio Volume */
    { 0, XF86XK_AudioRaiseVolume, spawn,
        SHCMD("~/.local/bin/volume-control/volume-up")},

    { 0, XF86XK_AudioLowerVolume, spawn,
        SHCMD("~/.local/bin/volume-control/volume-down" )},

    { 0, XF86XK_AudioMute, spawn,
        SHCMD("~/.local/bin/volume-control/volume-mute") },
    
    /* Theme Reload */
    { MODKEY, XK_F8, xrdb, {.v = NULL } },
};


/* Mouse Bindings */
// click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin
static Button buttons[] = {
	/* click, event mask, button, function, argument */
	{ ClkTagBar, MODKEY, Button1, tag, {0} },
	{ ClkTagBar, MODKEY, Button3, toggletag, {0} },
	{ ClkWinTitle, 0, Button2, zoom, {0} },
	{ ClkStatusText, 0, Button2, spawn, {.v = termcmd } },
	{ ClkClientWin, MODKEY, Button1, movemouse, {0} },
	{ ClkClientWin, MODKEY, Button2, togglefloating, {0} },
	{ ClkClientWin, MODKEY, Button3, resizemouse, {0} },
	{ ClkTagBar, 0, Button1, view, {0} },
	{ ClkTagBar, 0, Button3, toggleview, {0} },
	{ ClkTagBar, MODKEY, Button1, tag, {0} },
	{ ClkTagBar, MODKEY, Button3, toggletag, {0} },
};