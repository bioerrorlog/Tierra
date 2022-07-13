/* beagleGUI_InfoWindows.c -- -*- C -*-
 * 
 *-  Information popup windows define.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Sep  2 11:10:51 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 17:11:27 1998
 * Update Count    : 141
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_InfoWindows.c	1.11	05/10/99 15:51:06";
#endif /* lint */

#include <stdio.h>

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Command.h>

#include "beagleGUI.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_InfoWindows.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only five variable for another file using the file
 *- 'beagleGUI_Global.h'. Its name and type a 'Widget confWindow, infoWindow,
 *- warnWindow, errWindow, msgWindow'.
 *- So you can open this window from every GUI file if need it.
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- These windows (confWindow, infoWindow, warnWindow, errWindow,
 *- and msgWindow) use here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_InfoWindow.h'.
 */

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	toplevel;	/* This file need to keep toplevel Widget */
static Widget	confDialog;	/* keep confirmation dialog widget */
static Widget	infoDialog;	/* keep information dialog widget */
static Widget	warnDialog;	/* keep warning dialog widget */
static Widget	msgDialog;	/* keep message dialog widget */

static int	confWindowState = 0;
static int	infoWindowState = 0;
static int	warnWindowState = 0;
static int	msgWindowState  = 0;
static XtTranslations	tt_OK;
static XtAccelerators	at_OK, at_Cancel;


/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Action definition.*/
static void	ConfButtonPress	P_((Widget, int));
static void	InfoButtonPress	P_((Widget, int));
static void	WarnButtonPress	P_((Widget, int));
static void	MsgButtonPress	P_((Widget, int));
/*- This windows process routines -*/


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_InfoWindows.h'.
 */
void	InfoWindowPutTop(newtoplevel)
     Widget newtoplevel;
{
  toplevel = newtoplevel;
  at_OK =
    XtParseAcceleratorTable("<Key>Return:	set() notify() unset()");
  at_Cancel =
    XtParseAcceleratorTable("<Key>Escape:	set() notify() unset()");
  tt_OK =
    XtParseTranslationTable("<Key>Return:	set() notify() unset()");
}

void	CreateConfirmationDialog(topWidget)
     Widget topWidget;
{
  Widget button;
  
  confWindow
    = XtVaCreatePopupShell("confirmationDialog",
			   topLevelShellWidgetClass,	topWidget,
			   NULL);
  confDialog
    = XtVaCreateManagedWidget("confDialog",
			      dialogWidgetClass, confWindow,
			      NULL);

  button
    = XtVaCreateManagedWidget("YesButton",
			      commandWidgetClass, confDialog,
			      XtNaccelerators,	at_OK,
			      XtNborderWidth,	2,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ConfButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(confDialog, button);

  button
    = XtVaCreateManagedWidget("NoButton",
			      commandWidgetClass, confDialog,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)ConfButtonPress,
		(XtPointer)2);
  XtAugmentTranslations(button, tt_OK);
}

void	CreateInformationDialog(topWidget)
     Widget topWidget;
{
  Widget button;

  infoWindow
      = XtVaCreatePopupShell("informationDialog",
			   topLevelShellWidgetClass,	topWidget,
			   NULL);
  infoDialog
    = XtVaCreateManagedWidget("infoDialog",
			      dialogWidgetClass, infoWindow,
			      NULL);

  button
    = XtVaCreateManagedWidget("YesButton",
			      commandWidgetClass, infoDialog,
			      XtNaccelerators,	at_OK,
			      XtNborderWidth,	2,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)InfoButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(infoDialog, button);

  button
    = XtVaCreateManagedWidget("NoButton",
			      commandWidgetClass, infoDialog,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)InfoButtonPress,
		(XtPointer)2);
  XtAugmentTranslations(button, tt_OK);

  button
    = XtVaCreateManagedWidget("CancelButton",
			      commandWidgetClass, infoDialog,
			      XtNaccelerators, at_Cancel,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)InfoButtonPress,
		(XtPointer)3);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(infoDialog, button);
}

void	CreateWarningDialog(topWidget)
     Widget topWidget;
{
  Widget button;

  warnWindow
      = XtVaCreatePopupShell("warningDialog",
			   topLevelShellWidgetClass,	topWidget,
			   NULL);
  warnDialog
    = XtVaCreateManagedWidget("warnDialog",
			      dialogWidgetClass, warnWindow,
			      NULL);

  button
    = XtVaCreateManagedWidget("YesButton",
			      commandWidgetClass, warnDialog,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)WarnButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(button, tt_OK);

  button
    = XtVaCreateManagedWidget("NoButton",
			      commandWidgetClass, warnDialog,
			      XtNaccelerators,	at_OK,
			      XtNborderWidth,	2,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)WarnButtonPress,
		(XtPointer)2);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(warnDialog, button);

  button
    = XtVaCreateManagedWidget("CancelButton",
			      commandWidgetClass, warnDialog,
			      XtNaccelerators, at_Cancel,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)WarnButtonPress,
		(XtPointer)3);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(warnDialog, button);
}


void	CreateMessageDialog(topWidget)
     Widget topWidget;
{
  Widget button;
  
  msgWindow
      = XtVaCreatePopupShell("messageDialog",
			     topLevelShellWidgetClass,	topWidget,
			     XtNallowShellResize,	True,
			     NULL);
  msgDialog
    = XtVaCreateManagedWidget("msgDialog",
			      dialogWidgetClass, msgWindow,
			      NULL);

  button
    = XtVaCreateManagedWidget("OKButton",
			      commandWidgetClass, msgDialog,
			      XtNaccelerators,	at_OK,
			      XtNborderWidth,	2,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)MsgButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(button, tt_OK);
  XtInstallAccelerators(msgDialog, button);
}


int	ConfirmationMessage(msg)
     char* msg;
{
  Arg	args[1];
  char	chaine[81];
  
  XtSetArg(args[0], XtNlabel, chaine);
  XtGetValues(confDialog, args, 1);

  if (strcmp(msg, chaine))
    {
      XtDestroyWidget(confWindow);
      CreateConfirmationDialog(toplevel);
      XtSetArg(args[0], XtNlabel, msg);
      XtSetValues(confDialog, args, 1);
    }
    
  return SelectWaitLoop(confWindow, &confWindowState);
}


int	InformationMessage(msg)
     char* msg;
{
  Arg	args[1];
  char	chaine[81];
  
  XtSetArg(args[0], XtNlabel, chaine);
  XtGetValues(infoDialog, args, 1);

  if (strcmp(msg, chaine)){
    XtDestroyWidget(infoWindow);
    CreateInformationDialog(toplevel);
    XtSetArg(args[0], XtNlabel, msg);
    XtSetValues(infoDialog, args, 1);
  }
  return SelectWaitLoop(infoWindow, &infoWindowState);
}

int	WarningMessage(msg)
     char* msg;
{
  Arg	args[1];
  char	chaine[81];
  
  XtSetArg(args[0], XtNlabel, chaine);
  XtGetValues(warnDialog, args, 1);

  if (strcmp(msg, chaine)){
    XtDestroyWidget(warnWindow);
    CreateWarningDialog(toplevel);
    XtSetArg(args[0], XtNlabel, msg);
    XtSetValues(warnDialog, args, 1);
  }
  return SelectWaitLoop(warnWindow, &warnWindowState);
}

int	MessageMessage(msg)
     char* msg;
{
  Arg	args[1];
  char	chaine[81];
  
  XtSetArg(args[0], XtNlabel, chaine);
  XtGetValues(msgDialog, args, 1);

  if (strcmp(msg, chaine)){
    XtDestroyWidget(msgWindow);
    CreateMessageDialog(toplevel);
    XtSetArg(args[0], XtNlabel, msg);
    XtSetValues(msgDialog, args, 1);
  }
  return SelectWaitLoop(msgWindow, &msgWindowState);
}

/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 *- -- First part are creating window style.
 *- -- Second part are window actions mechanism.
 *- -- Third part are this windows process routines.
 */

/*
 *- Creating window style.
 */
/* NONE */

/*
 *- Action definition.
 */
static void	ConfButtonPress(w, n)
     Widget w;
     int n;
{
  confWindowState = n;
  XtPopdown(confWindow);
}

static void	InfoButtonPress(w, n)
     Widget w;
     int n;
{
  infoWindowState = n;
  XtPopdown(infoWindow);
}


static void	WarnButtonPress(w, n)
     Widget w;
     int n;
{
  warnWindowState = n;
  XtPopdown(warnWindow);
}


static void	MsgButtonPress(w, n)
     Widget w;
     int n;
{
  msgWindowState = n;
  XtPopdown(msgWindow);
}

/*
 *- This windows process routines.
 */
/* NONE */
