/* beagleGUI_KeyInWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_KeyInWindow.h	1.7	05/22/98 18:13:46
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Oct  1 16:49:12 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:13:32 1998
 * Update Count    : 27
 * Status          : 
 */

#ifndef beagleGUI_KeyIn_h
#define beagleGUI_KeyIn_h

#include "clnt.h"

/*
 * StrKeyIn(out, n, label)
 *	This function creates a n questions dialog box. label must be
 * an array of n strings which contain the text of questions. After the
 * user clicked on OK button, this function fill in the array of n texts
 * out with his answers.
 *	If out[i] have value, this value will be set as default. If
 * this value is "yes" or "no", its widget is a toggle box and 
 * corresponding answer is "yes" or "no" in out[i]. Those toggle must
 * be at the end of the list.
 */
extern void StrKeyIn	P_((String *, int, String *));

/*
 * SecretStrKeyIn(label)
 *
 *	This function creates a dlog where the
 * user can intput text without echo.  It returns the string typed by
 * him.
 */
extern char* SecretStrKeyIn	P_((char*));


/************************************************************************/
extern void	CreateStrKeyInDialog	P_((Widget));
extern void	CreateSecretStrKeyInDialog	P_((Widget));
extern void	ActivateText	P_((Widget, XEvent*, String*, Cardinal*));
extern void	SwapActiveText	P_((Widget, XEvent*, String*, Cardinal*));
/******************************DON'T USE*********************************/
#endif /* beagleGUI_KeyIn_h */



