/* See LICENSE file for copyright and license details. */

#include "exitdwm.c"
#include "movestack.c"

/* appearance */
static const unsigned int attachbelow = 1;
static const unsigned int gappx = 12;
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
static const int topbar             = 1;     /* 0 means bottom bar */
static const char *fonts[] = { "FiraCode Nerd Font Mono-Bold:size=12" };
static const char dmenufont[]       = "FiraCode Nerd Font Mono-Bold:size=12";

/* Index of preferred layout], if LENGTH(lpm)<#monitors -> default layout */
static const int lpm[] = {0, 1};

/* Gruvbox Dark — canonical palette */
/* Backgrounds / neutrals */
static const char bg0[]   = "#282828"; /* normal background */
static const char bg1[]   = "#1d2021"; /* darker (bar / contrast) */
static const char bg2[]   = "#3c3836"; /* subtle separation */

static const char fg0[]   = "#ebdbb2"; /* main foreground */
static const char fg1[]   = "#cab699"; /* muted foreground */
static const char gray[] = "#928374";  /* bright black / comments */

/* Normal colors */
static const char red[]     = "#cc241d";
static const char green[]   = "#98971a";
static const char yellow[]  = "#d79921";
static const char blue[]    = "#458588";
static const char magenta[] = "#b16286";
static const char cyan[]    = "#689d6a";

/* Bright colors */
static const char bred[]    = "#fb4934";
static const char bgreen[]  = "#b8bb26";
static const char byellow[] = "#fabd2f";
static const char bblue[]   = "#83a598";
static const char bmagenta[]= "#d3869b";
static const char bcyan[]   = "#8ec07c";

static const char *colors[][3] = {
    /*               fg      bg      border */
    /* Normal (unfocused) windows */
    [SchemeNorm]  = { fg0,    bg1,    bg1     },
    /* Selected window */
    [SchemeSel]   = { bg1,    fg0,   byellow },
    /* Bar title (middle) */
    [SchemeTitle] = { fg0,    bg1,    bg1     },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const int taglayouts[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ ":T:",     tile },    /* first entry is default */
	{ ":F:",      NULL },    /* no layout function means floating behavior */
	{ ":M:",      monocle },
    { ":G:",      grid},
    { ":C:",      centeredmaster },
	{ ":c:",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* Gruvbox Dark dmenu */
static const char *dmenucmd[] = {
    "dmenu_run",
    "-fn", dmenufont,
    "-nb", bg1,     /* normal background */
    "-nf", fg0,     /* normal text */
    "-sb", byellow,    /* selected background */
    "-sf", bg2,     /* selected text */
    NULL
};

static const char *thunarcmd[]  = {"thunar", NULL };
static const char *librewolfcmd[] = {"librewolf", NULL};
static const char *termcmd[]  = { "alacritty", NULL };

static const Key keys[] = {
    /* launchers */
    { MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
    { MODKEY|ShiftMask,		    XK_d,      spawn,          {.v = thunarcmd } },
    { MODKEY|ShiftMask,		    XK_f,      spawn,	       {.v = librewolfcmd} },
    /* layouts */
    { MODKEY,                       XK_F1,      setlayout,      {.v = &layouts[0]} },
    { MODKEY,                       XK_F2,      setlayout,      {.v = &layouts[1]} },
    { MODKEY,                       XK_F3,      setlayout,      {.v = &layouts[2]} },
    { MODKEY,                       XK_F4,      setlayout,      {.v = &layouts[3]} },
    { MODKEY,                       XK_F5,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_F6,      setlayout,      {.v = &layouts[5]} },
    { MODKEY,                       XK_space,  setlayout,      {0} },

    /* floating toggle */
    { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },

    /* window focus */
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },

    /* move stack */
    { MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
    { MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },

    /* master size */
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },

    /* toogle bar */
    { MODKEY,                       XK_b,      togglebar,      {0} },

    /* zoom (swap master) */
    { MODKEY|ShiftMask,             XK_Return, zoom,           {0} },

    /* kill window */
    { MODKEY|ShiftMask,             XK_q,      killclient,     {0} },

    /* monitor focus */
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },

    /* move windows to monitor */
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

    /* quit dwm */
    { MODKEY|ShiftMask,             XK_e,      exitdwm,           {0} },

    /* workspace (tag) keys */
    TAGKEYS( XK_1, 0 )
    TAGKEYS( XK_2, 1 )
    TAGKEYS( XK_3, 2 )
    TAGKEYS( XK_4, 3 )
    TAGKEYS( XK_5, 4 )
    TAGKEYS( XK_6, 5 )
    TAGKEYS( XK_7, 6 )
    TAGKEYS( XK_8, 7 )
    TAGKEYS( XK_9, 8 )
    { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
    { MODKEY,                       XK_p,      incnmaster,     {.i = -1 } },

    /* gaps */
    { MODKEY|ShiftMask,                       XK_F11,  setgaps,       {.i = +1 } }, // increase
    { MODKEY|ShiftMask,                       XK_F12,  setgaps,       {.i = -1 } }, // decrease
};

//static const Key keys[] = {
//	/* modifier                     key        function        argument */
//	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
//	{ MODKEY,	                XK_Return, spawn,          {.v = termcmd } },
//	{ MODKEY,                       XK_b,      togglebar,      {0} },
//	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
//	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
//	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
//	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
//	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
//	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
//	{ MODKEY|ShiftMask,		XK_Return, zoom,           {0} },
//	{ MODKEY,                       XK_Tab,    view,           {0} },
//	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
//	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
//	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
//	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
//	{ MODKEY,                       XK_space,  setlayout,      {0} },
//	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
//	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
//	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
//	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
//	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
//	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
//	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
//	TAGKEYS(                        XK_1,                      0)
//	TAGKEYS(                        XK_2,                      1)
//	TAGKEYS(                        XK_3,                      2)
//	TAGKEYS(                        XK_4,                      3)
//	TAGKEYS(                        XK_5,                      4)
//	TAGKEYS(                        XK_6,                      5)
//	TAGKEYS(                        XK_7,                      6)
//	TAGKEYS(                        XK_8,                      7)
//	TAGKEYS(                        XK_9,                      8)
//	{ MODKEY|ShiftMask,             XK_p,      quit,           {0} },
//};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

