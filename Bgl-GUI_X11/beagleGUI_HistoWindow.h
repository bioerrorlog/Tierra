/* beagleGUI_HistoWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_HistoWindow.h	1.5	05/22/98 18:04:11
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:27:27 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:03:56 1998
 * Update Count    : 22
 * Status          : 
 */

#ifndef beagleGUI_HistoWindow_h
#define beagleGUI_HistoWindow_h

#include "clnt.h"

extern void	CreateHistoWindow	P_((Widget));
extern void	RedrawHistoInfoArea	P_((Widget, XExposeEvent*,
					    String*, Cardinal*));
extern void	HistoAreaClean		P_((void));
extern void	UpdateSizeHistData	P_((void));
extern void	UpdateGeneHistData	P_((void));
extern void	UpdateMemHistData	P_((void));
extern void	UpdateESHistData	P_((void));
extern void	UpdateEGHistData	P_((void));

#endif /* beagleGUI_HistoWindow_h */

