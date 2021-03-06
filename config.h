/* See LICENSE file for copyright and license details. */
#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Source Code Pro:size=10" };
static const char dmenufont[]       = "Source Code Pro:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char dark_grey[]       = "#3b4252";
static const char silver[]          = "#6d7a96";
static const char white[]           = "#d8dee9";
static const char light_blue[]      = "#81a1c1";
static const char dark_blue[]       = "#5e81ac";
static const char col_vdark[]       = "#292d36";
static const char col_dark[]        = "#3a404d";
static const char col_norm[]        = "#8790a3";
static const char col_full[]        = "#ffffff";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_norm,  col_vdark,  col_dark },
	[SchemeSel]  = { col_full,  col_vdark,  col_dark },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "urxvt", NULL };
static const char *lockcmd[]  = { "xscreensaver-command", "-lock", NULL };

static Key keys[] = {
	/* modifier                     key         function        argument */
	{ MODKEY,                       XK_Tab,     view,           {0} },
	{ MODKEY,                       XK_b,       togglebar,      {0} },
	{ MODKEY,                       XK_d,       incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_i,       incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_l,       spawn,          {.v = lockcmd } },
	{ MODKEY,                       XK_m,       setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_p,       spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_space,   setlayout,      {0} },
 	{ MODKEY,                       XK_j,       focusstack,     {.i = +1 } },
 	{ MODKEY,                       XK_k,       focusstack,     {.i = -1 } },    
    { MODKEY,					    XK_Left,    cycle,	        {.i = -1 } },    
    { MODKEY,					    XK_Right,   cycle,	        {.i = +1 } },

	{ MODKEY|ShiftMask,             XK_Return,  spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_c,       killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_j,       movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,       movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_q,       quit,           {0} },
	{ MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_Left,    tagcycle,       {.i = -1 } },   /* Move focused window to the Previous Tag */
    { MODKEY|ShiftMask,             XK_Right,   tagcycle,       {.i = +1 } },   /* Move focused window to the Next     Tag */

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
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

