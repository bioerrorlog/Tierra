/* beagleGUIGlobal.h -- -*-C -*-
 *
 * @(#)beagleGUI_Global.h	1.14	06/17/99 15:23:32
 * 
 *-  BEAGLE GLOVAL VARIABL (for GUI ROUTINES onry) DEFINE .
 *-  You can define own gloval valiable by writting same style these.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 19 17:45:26 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jun 17 15:23:14 1999
 * Update Count    : 110
 * Status          : 
 */

#ifndef	beagleGUIGlobal_h
#define	beagleGUIGlobal_h

#include "clnt.h"
#include <X11/Intrinsic.h>

#define	NUM_OF_COLOR_VARIATION	10

#ifdef	MAIN_DEF
# define	UI_GLOBAL
# define UI_GLOBAL_VAL(v)  = (v)
#else
# define	UI_GLOBAL extern
# define UI_GLOBAL_VAL(v)  /* */
#endif	/* MAIN */

UI_GLOBAL Display*	display;		/* beagleGUI.c */
UI_GLOBAL Screen*	screen;			/* beagleGUI.c */
UI_GLOBAL GC		default_gc;		/* beagleGUI.c */
UI_GLOBAL GC		text_gc;		/* beagleGUI.c */
UI_GLOBAL GC		white_gc;		/* beagleGUI.c */
UI_GLOBAL GC		black_gc;		/* beagleGUI.c */
UI_GLOBAL GC		gray_gc;		/* beagleGUI.c */
UI_GLOBAL GC		colorGC[NUM_OF_COLOR_VARIATION];/* beagleGUI.c */
UI_GLOBAL Widget	topWindow;		/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	statsWindow;		/* beagleGUI_StatsWindow.c */
UI_GLOBAL Widget	statsOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	planWindow;		/* beagleGUI_PlanWindow.c */
UI_GLOBAL Widget	planOpenButton;		/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	messageWindow;		/* beagleGUI_MessageWindow.c */
UI_GLOBAL Widget	messageOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	ovInstWindow;		/* beagleGUI_OvInstWindow.c */
UI_GLOBAL Widget	ovInstOpenButton;      /* beagleGUI_OverviewWindow.c */
UI_GLOBAL Widget	ovInfoWindow;		/* beagleGUI_OvInfoWindow.c */
UI_GLOBAL Widget	ovInfoOpenButton;      /* beagleGUI_OverviewWindow.c */
UI_GLOBAL Widget	ovGeneWindow;		/* beagleGUI_OvGeneWindow.c */
UI_GLOBAL Widget	overviewWindow;	       /* beagleGUI_OverviewWindow.c */
UI_GLOBAL Widget	overviewOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	queryWindow;		/* beagleGUI_QueryWindow.c */
UI_GLOBAL Widget	queryOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	histoWindow;		/* beagleGUI_HistoWindow.c */
UI_GLOBAL Widget	histoOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	tPingSWindow;		/* beagleGUI_TPingSWindow.c */
UI_GLOBAL Widget	tPingSOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	tPingCWindow;		/* beagleGUI_TPingCWindow.c */
UI_GLOBAL Widget	tPingCOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	varWindow;		/* beagleGUI_varWindows.c */
UI_GLOBAL Widget	varOpenButton;		/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	debugWindow;		/* beagleGUI_DebugWindow.c */
UI_GLOBAL Widget	debugOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	debugKeyWaitWindow;/* beagleGUI_DebugKeyWaitWindow.c */
UI_GLOBAL Widget	migrationWindow;     /* beagleGUI_migrationWindows.c */
UI_GLOBAL Widget	migrationOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	infoMessageWindow;	/* beagleGUI_topWindows.c */
UI_GLOBAL Widget	infoMessageOpenButton;	/* beagleGUI_TopWindow.c */
UI_GLOBAL Widget	confWindow;		/* beagleGUI_InfoWindows.c */
UI_GLOBAL Widget	infoWindow;		/* beagleGUI_InfoWindows.c */
UI_GLOBAL Widget	warnWindow;		/* beagleGUI_InfoWindows.c */
UI_GLOBAL Widget	errWindow;		/* beagleGUI_InfoWindows.c */
UI_GLOBAL Widget	msgWindow;		/* beagleGUI_InfoWindows.c */
UI_GLOBAL Widget	strKeyInWindow;		/* beagleGUI_KeyInWindows.c */
UI_GLOBAL Widget	secretStrKeyInWindow;	/* beagleGUI_KeyInWindows.c */

UI_GLOBAL char		selectedHostName[100];

#undef	UI_GLOBAL
#undef	UI_GLOBAL_VAL
#endif	/* beagleGUIGlobal_h */
