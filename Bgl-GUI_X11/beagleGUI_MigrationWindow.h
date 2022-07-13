/* beagleGUI_MigrationWindow.h -- -*- C -*-
 * 
 * @(#)beagleGUI_MigrationWindow.h	1.3	05/22/98 18:19:26
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Jun 10 09:57:58 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:19:14 1998
 * Update Count    : 4
 * Status          : 
 */

#ifndef beagleGUI_MigrationWindow_h
#define beagleGUI_MigrationWindow_h

#include "clnt.h"

extern void	CreateMigrationWindow	P_((Widget));
extern void	UpdateMigrationInfo	P_((void));
extern void     SetMigrationDisplayToOff	P_((void));
extern void     SetMigrationDisplayToOn		P_((void));

#endif /* beagleGUI_MigrationWindow_h */
