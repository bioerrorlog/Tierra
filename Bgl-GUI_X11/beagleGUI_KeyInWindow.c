/* beagleGUI_KeyInWindow.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Oct  1 16:50:24 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 17:14:04 1998
 * Update Count    : 143
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleGUI_KeyInWindow.c	1.17	05/10/99 15:51:29";
#endif /* lint */

#include <stdio.h>

#include "clnt.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>

#include "beagleMessageMgr.h"
#include "beagleGUI.h"
#include "beagleGUI_Global.h"
#include "beagleGUI_InfoWindows.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 *- This window show only one variable for another file using the file
 *- 'beagleGUI_Global.h'. Its name and type are 'Widget strKeyInWindow
 *- and 'Widget secretStrKeyInWindow' .
 *- So you can open this window from every GUI file if need it.
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- These windows (strKeyInWindow and secretStrKeyInWindow) use
 *- here external function only.
 *- So this file need the file include here function fo linking.
 *- I tried to collect there functions in '../Bglclnt/beagleRoutines.c'.
 *- Another GUI functions are define in the 'beagleGUI_KeyInWindow.h'.
 */

/*** NONE ***/

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static Widget	toplevel;	/* This file need to keep toplevel widget */

#define MAX_TEXT 10
static Widget	strKeyInForm;
static Widget	strKeyInLabel[MAX_TEXT];
static Widget	strKeyInText[MAX_TEXT];
static Widget	strKeyInOKButton;
static Widget	strKeyInCancelButton;
static int	strKeyInWindowState = 0;
static int	strKeyInTextNumber = 0;
static int	strKeyInTextPointer;
static Widget	strKeyInToggle[MAX_TEXT];
static int	strKeyInToggleNumber = 0;

static Widget	secretStrKeyInLabel;
static Widget	secretStrKeyInText;
static int	secretStrKeyInWindowState = 0;

static XtAccelerators	at_accOK, at_accCancel, at_accText;
static XtTranslations	tt_transText, tt_accOK;


/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/*- Action definition.*/
static void	StrKeyInButtonPress	P_((Widget, int));
static void	SecretStrKeyInButtonPress	P_((Widget, int));
/*- This windows process routines -*/
static void	ReMakeStrKeyInDialog	P_((Widget, int, String *, String *));


/*
 *- HERE ARE THIS WINDOW's SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleGUI_KeyInWindow.h'.
 */
void	CreateStrKeyInDialog(newtoplevel)
     Widget newtoplevel;
{
  toplevel = newtoplevel;
  at_accOK =
    XtParseAcceleratorTable("#override \n\
	<Key>Return:	set() notify() unset()");
  at_accCancel =
    XtParseAcceleratorTable("#override\n\
	<Key>Escape:	set() notify() unset()");
  at_accText =
    XtParseAcceleratorTable("#override\n\
	<Key>Right:	forward-character() \n\
	<Key>Left:	backward-character() \n\
	<Key>Delete:	delete-previous-character() \n\
	<Key>BackSpace:	delete-previous-character() \n\
	<Key>Tab:	display-caret(off) swapActiveText() \n\
	<Key>:		insert-char()");
  tt_accOK =
    XtParseTranslationTable("#override \n\
	<Key>Return:	set() notify() unset()");
  tt_transText =
    XtParseTranslationTable("#override\n\
	<Btn1Down>:	activateText() display-caret(on) select-start()");
}

void StrKeyIn(out, ntexts, strs)
     String* out;
     int ntexts;
     String* strs;
{
  Arg args[1];
  String str;
  static String empty = "";
  static String s_yes = "yes";
  static String s_no  = "no";
  int i;
  Boolean state;
    
  if (strKeyInTextNumber){
    XtSetArg(args[0], XtNlabel, &str);
    XtGetValues(strKeyInLabel[0], args, 1);

    if (strcmp(strs[0], str) || out[0]){
      ReMakeStrKeyInDialog(toplevel, ntexts, out, strs);
    }
  }
  else{
    ReMakeStrKeyInDialog(toplevel, ntexts, out, strs);
  }

  SelectWaitLoop(strKeyInWindow, &strKeyInWindowState);

  if ( strKeyInWindowState == 1 ){ /*  OK */
    for (i = 0 ; i < strKeyInTextNumber ; i ++){
      XtSetArg(args[0], XtNstring, out + i);
      XtGetValues(strKeyInText[i], args, 1);
    }
    for (i = 0 ; i < strKeyInToggleNumber ; i++){
      XtSetArg(args[0], XtNstate, &state);
      XtGetValues(strKeyInToggle[i], args, 1);
      out[strKeyInTextNumber + i] = state ? s_yes : s_no;
    }
    return;
  }
  else{   /*   Cancel  */
    *out = empty;
    return;
  }
}

void	CreateSecretStrKeyInDialog(topWidget)
     Widget topWidget;
{
  Widget button, form;
    
  secretStrKeyInWindow
    = XtVaCreatePopupShell("secretStringKeyInputDialog",
			   topLevelShellWidgetClass,	topWidget,
			   NULL);
  form
    = XtVaCreateManagedWidget("secretStrKeyInDialog",
			      formWidgetClass, secretStrKeyInWindow,
			      NULL);

  secretStrKeyInLabel
    = XtVaCreateManagedWidget("secretStrKeyInLabel",
			      labelWidgetClass, form,
			      NULL);
  secretStrKeyInText
    = XtVaCreateManagedWidget("secretStrKeyInText",
			      asciiTextWidgetClass, form,
			      XtNfromVert,	secretStrKeyInLabel,
			      XtNecho,		False,
			      XtNeditType,	XawtextEdit,
			      NULL);

  button
    = XtVaCreateManagedWidget("OKButton",
			      commandWidgetClass, form,
			      XtNfromVert,	secretStrKeyInText,
			      XtNaccelerators,	at_accOK,
			      XtNborderWidth,	2,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)SecretStrKeyInButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(button, tt_accOK);
  XtInstallAccelerators(form, button);
  XtInstallAccelerators(secretStrKeyInText, button);

  button
    = XtVaCreateManagedWidget("CancelButton",
			      commandWidgetClass, form,
			      XtNfromVert,	secretStrKeyInText,
			      XtNfromHoriz,	button,
			      XtNaccelerators,	at_accCancel,
			      NULL);
  XtAddCallback(button,
		XtNcallback, (XtCallbackProc)SecretStrKeyInButtonPress,
		(XtPointer)2);
  XtAugmentTranslations(button, tt_accOK);
  XtInstallAccelerators(form, button);
  XtInstallAccelerators(secretStrKeyInText, button);
}

char*	SecretStrKeyIn(msg)
     char* msg;
{
  Arg	args[1];
  String str;
  static String empty = "";

  XtSetArg(args[0], XtNlabel, &str);
  XtGetValues(secretStrKeyInLabel, args, 1);

  if (strcmp(msg, str)){
    XtDestroyWidget(secretStrKeyInWindow);
    CreateSecretStrKeyInDialog(toplevel);
    XtSetArg(args[0], XtNlabel, msg);
    XtSetValues(secretStrKeyInLabel, args, 1);
  }

  SelectWaitLoop(secretStrKeyInWindow, &secretStrKeyInWindowState);

  if ( secretStrKeyInWindowState == 1 ){ /*  OK */   
    XtSetArg(args[0], XtNstring, &str);
    XtGetValues(secretStrKeyInText, args, 1);
    return str;
  }
  else{   /*   Cancel  */
    str = empty;
    return str;
  }
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
void	ActivateText(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  int i, j;
  Arg args[1];

  XtSetArg(args[0], XtNdisplayCaret, False);
  XtSetValues(strKeyInText[strKeyInTextPointer], args, 1);
  
  for (i = 0 ; (i < strKeyInTextNumber) && (strKeyInText[i] != w) ; i++);
  strKeyInTextPointer = i;
  
  for (j = 0 ; (j < strKeyInTextNumber) ; j++){
    XtInstallAccelerators(strKeyInText[j], strKeyInText[strKeyInTextPointer]);
    XtInstallAccelerators(strKeyInText[j], strKeyInOKButton);
    XtInstallAccelerators(strKeyInText[j], strKeyInCancelButton);
  }
  XtInstallAccelerators(strKeyInForm, strKeyInText[i]);
  XtInstallAccelerators(strKeyInForm, strKeyInOKButton);
  XtInstallAccelerators(strKeyInForm, strKeyInCancelButton);
}

void	SwapActiveText(w, bevent, parms, num_parms)
     Widget w;
     XEvent* bevent;
     String* parms;
     Cardinal* num_parms;
{
  int j;
  Arg args[1];
  
  strKeyInTextPointer = (++strKeyInTextPointer < strKeyInTextNumber) ?
    strKeyInTextPointer : 0;
  
  for (j = 0 ; (j < strKeyInTextNumber) ; j++){
    XtInstallAccelerators(strKeyInText[j], strKeyInText[strKeyInTextPointer]);
    XtInstallAccelerators(strKeyInText[j], strKeyInOKButton);
    XtInstallAccelerators(strKeyInText[j], strKeyInCancelButton);
  }
  XtInstallAccelerators(strKeyInForm, strKeyInText[strKeyInTextPointer]);
  XtInstallAccelerators(strKeyInForm, strKeyInOKButton);
  XtInstallAccelerators(strKeyInForm, strKeyInCancelButton);

  XtSetArg(args[0], XtNdisplayCaret, True);
  XtSetValues(strKeyInText[strKeyInTextPointer], args, 1);
}
  
static void	StrKeyInButtonPress(w, n)
     Widget w;
     int n;
{
  strKeyInWindowState = n;
  XtPopdown(strKeyInWindow);
}


static void	SecretStrKeyInButtonPress(w, n)
     Widget w;
     int n;
{
  secretStrKeyInWindowState = n;
  XtPopdown(secretStrKeyInWindow);
}

/*
 *- This windows process routines.
 */
static void	ReMakeStrKeyInDialog(topWidget, ninput, output, input)
     Widget topWidget;
     int ninput;
     String* output;
     String* input;
{
  int	i, l;
  char	str[21], *str1;
  Arg	args[1];
  Boolean st;

  strKeyInTextNumber	= ninput;
  strKeyInToggleNumber	= 0;

  if ( strKeyInWindow ) XtDestroyWidget(strKeyInWindow);

  strKeyInWindow
    = XtVaCreatePopupShell("stringKeyInputDialog",
			   topLevelShellWidgetClass,	topWidget,
			   NULL);
  strKeyInForm
    = XtVaCreateManagedWidget("strKeyInDialog",
			      formWidgetClass, strKeyInWindow,
			      NULL);

  /* look for toggle buttons */
  for (i = 0 ; i < ninput ; i++){
    if (output[i] && (!strcmp(output[i], "yes") || !strcmp(output[i], "no"))){
      strKeyInToggleNumber ++;
    }
  }
  strKeyInTextNumber -= strKeyInToggleNumber;

  /* create texts */
  for (i = 0 ; i < strKeyInTextNumber ; i++){
    strcpy(str, "strKeyInLabel");
    str[l = strlen(str)] =	i + '0';
    str[l + 1] =		'\0';
    if (i){
      strKeyInLabel[i]
	= XtVaCreateManagedWidget(str,
				  labelWidgetClass,	strKeyInForm,
				  XtNfromVert,	strKeyInText[i - 1],
				  XtNlabel,	input[i],
				  NULL);
    }
    else{
      strKeyInLabel[i]
	= XtVaCreateManagedWidget(str,
				  labelWidgetClass,	strKeyInForm,
				  XtNlabel,	input[i],
				  NULL);
    }

    strcpy(str, "strKeyInText");
    str[l = strlen(str)] =	i + '0';
    str[l + 1] =		'\0';
    str1 =			output[i] ? output[i] : NULL;
    strKeyInText[i]
      = XtVaCreateManagedWidget(str,
				asciiTextWidgetClass, strKeyInForm,
				XtNfromVert,		strKeyInLabel[i],
				XtNeditType,		XawtextEdit,
				XtNwidth,		300,
				XtNaccelerators,	at_accText,
				XtNdisplayCaret,	False,
				XtNstring,		str1,
				NULL);
    XtOverrideTranslations(strKeyInText[i], tt_transText);
  }

  /* Create Toggle */
  for (i = 0 ; i < strKeyInToggleNumber ; i++){
    strcpy(str, "strKeyInToggle");
    str[l = strlen(str)] =	i + '0';
    str[l + 1] =		'\0';
    st =			strcmp(output[strKeyInTextNumber + i], "no");

    if ((strKeyInTextNumber == 0) && (i == 0)){
      strKeyInToggle[i] =
	XtVaCreateManagedWidget(str,
				toggleWidgetClass, strKeyInForm,
				XtNlabel,	input[strKeyInTextNumber + i],
				XtNstate,	st,
				NULL);
    }
    else if (strKeyInTextNumber == 0){
      strKeyInToggle[i] =
	XtVaCreateManagedWidget(str,
				toggleWidgetClass, strKeyInForm,
				XtNfromHoriz,	strKeyInToggle[i - 1],
				XtNlabel,	input[strKeyInTextNumber + i],
				XtNstate,	st,
				NULL);
    }
    else if (strKeyInTextNumber && (i == 0)){
      strKeyInToggle[i] =
	XtVaCreateManagedWidget(str,
				toggleWidgetClass, strKeyInForm,
				XtNfromVert, strKeyInText[strKeyInTextNumber - 1],
				XtNlabel,    input[strKeyInTextNumber + i],
				XtNstate,	st,
				NULL);
    }
    else{
      strKeyInToggle[i] =
	XtVaCreateManagedWidget(str,
				toggleWidgetClass, strKeyInForm,
				XtNfromVert, strKeyInText[strKeyInTextNumber - 1],
				XtNfromHoriz,	strKeyInToggle[i - 1],
				XtNlabel,	input[strKeyInTextNumber + i],
				XtNstate,	st,
				NULL);
    }
  }

  /*create buttons */
  strKeyInOKButton
    = XtVaCreateManagedWidget("OKButton",
			      commandWidgetClass,	strKeyInForm,
			      XtNaccelerators,		at_accOK,
			      XtNborderWidth,		2,
			      ninput ? XtNfromVert : NULL,
			      strKeyInToggleNumber ?
			      strKeyInToggle[strKeyInToggleNumber - 1] :
			      strKeyInText[strKeyInTextNumber - 1],
			      NULL);
  XtAddCallback(strKeyInOKButton,
		XtNcallback, (XtCallbackProc)StrKeyInButtonPress,
		(XtPointer)1);
  XtAugmentTranslations(strKeyInOKButton, tt_accOK);
  for (i = 0 ; i < strKeyInTextNumber ; i++){
    XtInstallAccelerators(strKeyInText[i], strKeyInOKButton);
  }

  strKeyInCancelButton
    = XtVaCreateManagedWidget("CancelButton",
			      commandWidgetClass,	strKeyInForm,
			      XtNfromHoriz,		strKeyInOKButton,
			      XtNaccelerators,		at_accCancel,
			      ninput ? XtNfromVert : NULL,
			      strKeyInToggleNumber ?
			      strKeyInToggle[strKeyInToggleNumber - 1] :
			      strKeyInText[strKeyInTextNumber - 1],
			      NULL);
  XtAddCallback(strKeyInCancelButton,
		XtNcallback, (XtCallbackProc)StrKeyInButtonPress,
		(XtPointer)2);
  XtAugmentTranslations(strKeyInCancelButton, tt_accOK);

  if (strKeyInTextNumber){
    strKeyInTextPointer = 0;
    for (i = 0 ; i < strKeyInTextNumber ; i++){
      XtInstallAccelerators(strKeyInText[i], strKeyInText[strKeyInTextPointer]);
      XtInstallAccelerators(strKeyInText[i], strKeyInOKButton);
      XtInstallAccelerators(strKeyInText[i], strKeyInCancelButton);
    }
    XtInstallAccelerators(strKeyInForm, strKeyInText[strKeyInTextPointer]);
    XtInstallAccelerators(strKeyInForm, strKeyInOKButton);
    XtInstallAccelerators(strKeyInForm, strKeyInCancelButton);

    XtSetArg(args[0], XtNdisplayCaret, True);
    XtSetValues(strKeyInText[strKeyInTextPointer], args, 1);
  }
}
