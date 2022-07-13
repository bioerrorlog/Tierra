/* beagleGUI_OverviewWindow.h -- -*- C -*-
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:45:51 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu May 27 09:21:48 1999
 * Update Count    : 42
 * Status          : 
 */

#ifndef beagleGUI_OverviewWindow_h
#define beagleGUI_OverviewWindow_h

#include <X11/cursorfont.h>

#include "clnt.h"

/*
 *                             [data:  OvSizeData_t]
 *
 *                              +--------+
 *                              | next   | ----> point to nex data
 *                              +--------+
 *                              | length |
 *                              +--------+
 *                              | pop    |
 *                              +--------+
 *                              | area   |
 *                              +--------+
 *                              | color  |
 *                              +--------+
 */

struct OVSIZEDATA_T{
  struct OVSIZEDATA_T*	next;
  I32s	length;
  I32s	pop;
  I32s	area;
  I32s	color;
};
typedef struct OVSIZEDATA_T	OvSizeData_t;

extern Cursor	OvCurs;

extern void	CreateOverviewWindow	P_((Widget));
extern void	RedrawOverviewMemFrame	P_((Widget, XExposeEvent*,
					    String*,Cardinal*));
extern void	UpdateOvFrameData	P_((void));
extern void	UpdateOvData		P_((void));

extern OvSizeData_t*	Ov_GetSizeRoot	P_((void));


/*
 * functions for mouse management
 */
extern int	Ov_MouseMapRequest	P_((String str, void (*func)(I32u)));
/* ask request to X for getting a memory address from overview map */

/**************************************************************************/
extern void	Ov_QuitWindow	P_((Widget, XEvent*, String*, Cardinal*));
extern void	Ov_EnterWindow	P_((Widget, XEvent*, String*, Cardinal*));
extern void	Ov_StickylabelUpdate	P_((Widget, XEvent*,
					    String*, Cardinal*));
extern void	Ov_PutCursorValue	P_((Widget, XEvent*,
					    String* , Cardinal*));
extern void	Ov_ConfirmCursorValue	P_((Widget, XEvent*,
					    String* , Cardinal*));
extern void	Ov_MouseRedo	P_((Widget, XEvent*,String* , Cardinal*));
/*********************** DON'T USE THOSE FUNCTIONS*************************/

#endif /* beagleGUI_OverviewWindow_h */
