/* @(#)frontend.c    1.2 11/25/94 */
/* frontend.c 30-3-98 Tierra Simulator frontend routines */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #05XX, in use: none */
/* FEError labels: #05XX, in use: 0500-0505 */

/* frontend rationale:  to have a common set of functions for passing info
   from Tierra to the front end.  Any particular front end will likely
   use only a subset of all frontend functions.  Those functions that a
   particular front end does not use, will either have to be implemented
   here as dummy functions, or they will have to be ifdef'ed where they
   are called from Tierra */

#include "license.h"
#include "tierra.h"

#ifdef __GUI__
#define WEXTERN_H
#endif /* __GUI__ */

#include "globals.h"

#if INTERFACE == BASICD
#include <gppconio.h>
#endif /* INTERFACE == BASICD */

#ifdef __GUI__
#include "wntierra.h"
static char ENDBLANK[80];
#endif /* __GUI__ */

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

typedef struct
{   I32s x;                     /* x position (column) */
    I32s y;                     /* y position (row) */
}
Spos;                           /* last x,y position of cursor on screen */

Spos spos;

static void CalcEntropy P_((double *gtypentrp, double *szclsentrp));

/*
 * FEClear - clear display
 *
 */
void FEClear()
{
#if FRONTEND == STDIO
    tsprintf((char *)(&(Fbuf[0])), "\n");
    FEPrintf(0, 0, 0, 0, (char *)(&(Fbuf[0])));
    spos.x = spos.y = 0;
#endif /* FRONTEND == STDIO */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    clrscr();
#else /* __GUI__ */
    int i;
    for (i = 0; i < 25; i++)
    {   tsprintf((char *)(&(Fbuf[0]))," ");
        FEPrintf(0, i, 0, 0, &(Fbuf[0]));
    }
    EndPaint(hwnd, &ps);
    InvalidateRect(hwnd, NULL, TRUE);
    hdcnew = BeginPaint(hwnd, &ps);
    SelectObject(hdcnew, GetStockObject(SYSTEM_FIXED_FONT));
#endif /* __GUI__ */

#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
    tclear();
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICM
    GrafPtr oldPort;
    Rect destRect;

    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    destRect = wp->port.portRect;
    destRect.left = displayOffset + 1;
    EraseRect(&destRect);
    SetPort(oldPort);
#endif /* INTERFACE == BASICM */

}

/*
 * FEExit - close files and exit Tierra
 * ENO - exit code
 *
 */
void FEExit(ENO)
    I32s ENO;
{
#if FRONTEND == BASIC
#if INTERFACE == BASICW
    Sleep(1000);
#else /* INTERFACE == BASICW */
    sleep(1);
#endif /* INTERFACE == BASICW */
#endif /* FRONTEND == BASIC */

#ifndef CLSTRSRVR
    fclose(sgfile);
#endif /* CLSTRSRVR */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    textmode(C80);
    textbackground(0);
    textcolor(15);
    clrscr();
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    techo();
    tnl();

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)
    tnocbreak();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA) */

    tclear();
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */

    tendwin();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#ifndef __GUI__
    TierraExit(ENO);
#else /* __GUI__ */
    MessageBox(hwnd, "Tierra quits, and Windows too!", "Tierra Info", MB_OK);
    ExitWindows(0, 0);  /* This was the only way to exit cleanly... */
#endif  /* __GUI__ */ /* with non-preemptive multitasking...  */
}

/*
 * FEGetch - position cursor and get character from keyboard
 *    x,y - cursor position
 *
 * returns: character as I32s integer
 */
I32s FEGetch(x, y)
I32s x,y;
{
#if INTERFACE == BASICM
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
#endif /* INTERFACE == BASICM */

char getchrtn;

#if FRONTEND == BASIC

#if INTERFACE == BASICM

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    EventRecord event;
    short character = 0;
    GrafPtr oldPort;
    short x_pixel, y_pixel;
    Rect destRect;

    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    x_pixel = x * ((windowWidth - displayOffset + 5) / COLS) +
        displayOffset + 5;
    y_pixel = (y + 1) * rowHeight + fontInfo.ascent;
    destRect.left = x_pixel;
    destRect.right = destRect.left + CharWidth(character);
    destRect.top = y_pixel - fontInfo.ascent;
    destRect.bottom = y_pixel + fontInfo.descent + fontInfo.leading;

    while (!character)
    {   if (GetNextEvent(everyEvent, &event))
        {   if (event.what == keyDown)
            {   character = event.message & charCodeMask;
                EraseRect(&destRect);
                MoveTo(x_pixel, y_pixel);
                DrawChar(character);
            }
        }
        else if (event.what == nullEvent)
            character = 0;
    }
    SetPort(oldPort);

#ifdef SIGBLOCK
    (void) TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

      getchrtn=character;

#endif /* INTERFACE == BASICM */

#if (INTERFACE == BASICA)||(INTERFACE == BASICC)||\
    (INTERFACE == BASICV)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICD)||(INTERFACE == BASICB)
#if INTERFACE == BASICA
    getchrtn=tmvgetch(y, x);
#endif /* INTERFACE == BASICA */
#if (INTERFACE == BASICD)||(INTERFACE == BASICB)
    gotoxy(x+1, y+1);
#else /* (INTERFACE == BASICD)||(INTERFACE == BASICB) */
    tmove(y, x);
#endif /* (INTERFACE == BASICD)||(INTERFACE == BASICB) */
#if INTERFACE == BASICV
    tcrmode();
#endif /* INTERFACE == BASICV */
#if (INTERFACE == BASICW)||(INTERFACE == BASICD)||\
    (INTERFACE == BASICB)
    getchrtn=getche();
#else /* (INTERFACE == BASICW)||(INTERFACE == BASICD)||
         (INTERFACE == BASICB) */
    getchrtn=tigetch();
#endif /* (INTERFACE == BASICW)||(INTERFACE == BASICD)||
          (INTERFACE == BASICB) */
#if INTERFACE == BASICV
    tnocrmode();
#endif /* INTERFACE == BASICV */
#endif /* (INTERFACE == BASICA)||(INTERFACE == BASICC)||
          (INTERFACE == BASICV)||(INTERFACE == BASICW)||
          (INTERFACE == BASICD)||(INTERFACE == BASICB) */

#ifdef __GUI__
    int i = ' '; /*initialize to 'space bar' -the return value if no key
                pressed GUI FEGetch is used only by Micro_Spy() in ttools.c */
    MSG msg; /* to clear the Windows Message Queue */
    PeekMessage(&msg, (HWND) NULL, 0, 0, PM_REMOVE);
    if (GetAsyncKeyState(VK_LBUTTON) < 0)
        getchrtn='z';
    else if (GetAsyncKeyState(0x1b) < 0)
        getchrtn=0x1b;
    else if (GetAsyncKeyState(84) < 0)
        getchrtn = 't';
    else if (GetAsyncKeyState(85) < 0)
        getchrtn = 'u';
    else if (GetAsyncKeyState(78) < 0)
        getchrtn = 'n';
    else if (GetAsyncKeyState(VK_SHIFT) < 0)
        getchrtn = toupper(i);

#endif /* __GUI__ */
#else /* FRONTEND == BASIC */
    tfgets(&(Fbuf[0]), 84, stdin);
    getchrtn=Fbuf[0];
#endif /* FRONTEND == BASIC */

    tiestrtme=tietime(NULL);
    return (I32s)getchrtn;
}

/*
 * FEBanner - blank and highlight lines 0 and 1 (top two display lines)
 *            and bottom line
 */
void FEBanner()
{
#if (INTERFACE == BASICA)||(INTERFACE == BASICV)||\
    (INTERFACE == BASICC)||(INTERFACE == BASICW)
    I8s tt, dummy;

    tstandout();
    for (tt = 0; tt < (COLS - 1); tt++)
    {   dummy = tt;
#ifndef __linux__
        tmvaddch(0, dummy, ' ');
        tmvaddch(1, dummy, ' ');
        tmvaddch(LINES-1, dummy, ' ');
#else /* __linux__ */
        tmvprintw(0, dummy, " ");
        tmvprintw(1, dummy, " ");
        tmvprintw(LINES-1, dummy, " ");
#endif /* __linux__ */
    }
    tstandend();
#endif /* (INTERFACE == BASICA)||(INTERFACE == BASICV)||
          (INTERFACE == BASICC)||(INTERFACE == BASICW) */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    textbackground(InfoBack);
    textcolor(InfoText);
    gotoxy(1, 1);
    clreol();
    gotoxy(1, 2);
    clreol();
    gotoxy(1, HLP_Y + 1);
    clreol();
    textbackground(DispBack);
    textcolor(DispText);
#else /* __GUI__ */
    FEPrintf( 0, 0, 1, 0, ENDBLANK);
    FEPrintf( 0, 1, 1, 0, ENDBLANK);
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */
}

/*
 * FEStartup - initialize display parameters, clear screen,
 *             display startup screen and credits
 */
/* called immediately after the soup_in file is read */
/* an opportunity to interactively set soup_in vars */
void FEStartup()
{
#ifdef SIGBLOCK
#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    sigset_t SigSaveSet;
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */
#endif /* SIGBLOCK */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    FILE *inf;
    I8s first[12], second[12];
    I32s gmode, color;
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

    SetDispVars();
    if ((LINES < 20) || (COLS < 80))
    {   tfprintf(stderr, "\nTIERRA: not enough space on screen ... \n");
        FEExit(-666);
    }

#if FRONTEND == STDIO
#ifdef __TURBOC__
#ifndef __GUI__
    clrscr();
#else /* __GUI__ */
    FEClear();
#endif /* __GUI__ */
#else /* __TURBOC__ */
#ifndef _WIN32
    system("clear");
#endif /* _WIN32 */
#endif /* __TURBOC__ */
#endif /* FRONTEND == STDIO */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#if INTERFACE == BASICD
    gppconio_init();
#endif /* INTERFACE == BASICD */

#ifndef __GUI__

#if (INTERFACE == BASICB) && (__TURBOC__ >= 0x200) && (__TURBOC__ < 0x400)
    /* deal with old and (stupid) new compilers */
    FE_DV_Mode = DETECT;
    detectgraph(&FE_DV_Mode, &gmode);
#endif /* (INTERFACE == BASICB) && (__TURBOC__ >= 0x200) &&
          (__TURBOC__ < 0x400) */

    if (inf = tfopen("tcolors.cfg", "r"))
    {   tfgets(Buff, 84, inf);
        do
        {   if (*Buff != '#' && strlen(Buff) > 3)
            {
#ifdef SIGBLOCK
                SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
                sscanf(Buff, "%11s%11s%d", first, second, &color);
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

                if (!strcmp(first, "information"))
                {   if (!strcmp(second, "text:"))
                        InfoText = color;
                    else if (!strcmp(second, "background:"))
                        InfoBack = color;
                }
                else if (!strcmp(first, "display"))
                {   if (!strcmp(second, "text:"))
                        DispText = color;
                    else if (!strcmp(second, "background:"))
                        DispBack = color;
                }
            }
        }
        while (tfgets(Buff, 84, inf) != NULL);
        tfclose(inf);
    }
    textmode(C80);
    textbackground(DispBack);
    textcolor(DispText);
    clrscr();

#else /* __GUI__ */
    strcpy(ENDBLANK,
        "                                                                ");
    FEClear();
#endif /* __GUI__ */

#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICM)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    FEClear();
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    tnonl();
    tcbreak();
    tnoecho();   /* set up keyboard */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICM)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */

#ifndef CLSTRSRVR
IMode = PLN_STATS;
#endif /* CLSTRSRVR */

#ifndef __GUI__ /* Windows title screen in wntierra.c */

#ifndef CLSTRSRVR
#if INTERFACE != BASICM
    tsprintf((char *)(&(Fbuf[0])), "\n\n");  /* Title screen */
    FEPrintf(0, 3, 0, 0, (char *)(&(Fbuf[0])));
#endif /* INTERFACE != BASICM */
    tsprintf((char *)(&(Fbuf[0])), "Tierra\n\n");
    FEPrintf(37, 5, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "April 2003  VER %5.2f\n\n", VER);
    FEPrintf(32, 7, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "Information Wants To Be Alive!\n\n");
    FEPrintf(25, 9, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "By\n\n");
    FEPrintf(39, 11, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "Thomas S. Ray\n\n");
    FEPrintf(31, 13, 0, 0, (char *)(&(Fbuf[0])));

    tsprintf((char *)(&(Fbuf[0])),
        "Matt Jones, Walter Tackett, Tooru Yoshikawa, Joseph Hart\n\n");
    FEPrintf(14, 15, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Kurt Thearling, Tsukasa Kimezawa, Agnes Charrel\n\n");
    FEPrintf(16, 17, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Daniel Pirone, Tom Uffner, Marc Cygnus\n\n");
    FEPrintf(20, 19, 0, 0, (char *)(&(Fbuf[0])));
#endif /* CLSTRSRVR */

    FEClear();

#endif /* __GUI__ */

    FEBanner();

#if FRONTEND != STDIO
    CtrBufStr(&(Fbuf[0]),COLS,
#ifdef CLSTRSRVR
        "cluster server initializing, please wait ..."
#else /* CLSTRSRVR */
        "Tierra initializing, please wait ..."
#endif /* CLSTRSRVR */
    );
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
#endif /* FRONTEND != STDIO */
}

void SetDispVars()
{
#if FRONTEND == BASIC

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
    tinitscr();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICB
#if (__TURBOC__ >= 0x200) && (__TURBOC__ < 0x400)
    if (mode == EGA)
        LINES = 43;
    else if (mode == VGA)
        LINES = 50;
    else
#endif /* (__TURBOC__ >= 0x200) && (__TURBOC__ < 0x400) */
        LINES = 25;
    COLS = 80;
#endif /* INTERFACE == BASICB */

#if (INTERFACE == BASICD)||(INTERFACE == BASICM)||\
    (INTERFACE == BASICW)
    LINES = 25;
    COLS = 80;
#endif /* (INTERFACE == BASICD)||(INTERFACE == BASICM)||
          (INTERFACE == BASICW) */

    ERR_X=0;
    ERR_Y=LINES-2;;
    PLN_X=0;
    PLN_Y=2;
    HLP_X=0;
    HLP_Y=LINES-1;;
    MSG_X=0;
    SetPlanMSG_Y();
#else /* FRONTEND == BASIC */
    LINES=24;
    COLS=80;
#endif /* FRONTEND == BASIC */
    fe_lines=LINES-1;
    fe_width=COLS-23;
}

/*
 * FEPrintf - Tierra frontend string print function
 *
 * scr_x, scr_y - cursor position
 * scr_a - 0   - no highlight
 *       - <>0 - highlighted
 * sleep - < 0 - pause time in seconds after printing
 *         <>0 - pause time in seconds after printing
 *               if histogram is in effect
 * str   - string to display
 */
void FEPrintf(scr_x, scr_y, scr_a, slee, str)
    I32s scr_x, scr_y, scr_a, slee;
    char *str;
{
#if FRONTEND == BASIC
    I8s CF = 0;
#endif /* FRONTEND == BASIC */
    I32s len = strlen((const char *)str);

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
I32s i;
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICM
    char temp_str[500], meas_str[500], *token;
    char *cr = "\n";
    GrafPtr oldPort;
    short x_pixel, y_pixel;
    Rect destRect;
#endif /* INTERFACE == BASICM */
#ifdef SIGBLOCK
#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
    sigset_t SigSaveSet;
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */
#endif /* SIGBLOCK */

    if (str[0] == 0)
        return;

#if FRONTEND == STDIO

    if (scr_x < spos.x)
    {   tfprintf(stdout,"\n");
        spos.x = 0;
    }
    if (scr_x > spos.x)
    {   while (spos.x < scr_x)
        {   tfprintf(stdout," ");
            spos.x++;
        }
    }
    tfprintf(stdout,str);
#ifndef DECVAX
    tfflush(stdout);
#endif /* DECVAX */
    if (str[len - 1] == '\n')
    {   spos.x = 0;
    }
    else
        spos.x += len;

#endif /* FRONTEND == STDIO */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
/* the upper left character is 1,1; the lower right is 80,25 */

    scr_x++;
    scr_y++;
    if ((scr_x < 1) || (scr_x + len > COLS) || (scr_y < 1) ||
        (scr_y > LINES))
    {   sound(500);
        delay(500);
        nosound();
        return;
    }
#endif /* __GUI__ */
    if (str[len - 1] == '\n')
    {   str[len - 1] = '\0';
        CF = 1;
    }
#ifndef __GUI__
    if (scr_a)
    {   textbackground(InfoBack);
        textcolor(InfoText);
    }
    gotoxy(scr_x, scr_y);
    cprintf(str);
    if (CF)
        clreol();
    if (scr_a)
    {   textbackground(DispBack);
        textcolor(DispText);
    }
#else /* __GUI__ */
    if (scr_a)
        SetBkColor(hdcnew, RGB(255, 255, 0));  /* yellow */
    else
        SetBkColor(hdcnew, RGB(255, 255, 255));  /* white  */

    if (CF)
    {   (FEw + scr_y)->szMarg = 0L;
        strcpy((FEw + scr_y)->szDesc, ENDBLANK);
        FEwTextOut(FEw, scr_y);
        (FEw + scr_y)->szMarg = 20L;
        FEwTextOut(FEw, scr_y);
    }
    (FEw + scr_y)->szMarg = scr_x;
    strcpy((FEw + scr_y)->szDesc, str);
    FEwTextOut(FEw, scr_y);

#endif /* __GUI__ */

#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
    if (str[len - 1] == '\n')
    {   str[len - 1] = '\0';
        CF = 1;
    }
    if (scr_a)
        tstandout();

#if INTERFACE == BASICV
    tmove(scr_y, scr_x);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    tprintw(str);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
#else /* INTERFACE == BASICV */
    tmvprintw(scr_y, scr_x, str);
#endif /* INTERFACE == BASICV */

    if(CF)
    {   if(scr_a)
        {
#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            for(i=0;i<(COLS-len);i++)
                tprintw(" ");
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
        }
        else
            tclrtoeol();
    }
    if (scr_a)
        tstandend();

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */

#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICM
    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    /* strip out the carriage returns */
    if (str[len - 1] == '\n')
    {   spos.x = 0;
        CF = 1;
    }
    token = strtok(str, cr);
    strcpy(temp_str, token);
    while (token != NULL)
    {   token = strtok(NULL, cr);
        if (token != NULL)
            strcat(temp_str, token);
    }
    strcpy(meas_str, temp_str);
    /* Erase the current block of text */
    x_pixel = scr_x * ((windowWidth - displayOffset + 5) / COLS) +
        (displayOffset + 5);
    y_pixel = (scr_y) * rowHeight + fontInfo.ascent;
    destRect.left = x_pixel;
    destRect.right = destRect.left + StringWidth(CtoPstr(meas_str));
    destRect.top = y_pixel - fontInfo.ascent;
    destRect.bottom = y_pixel + fontInfo.descent + fontInfo.leading;
    EraseRect(&destRect);
    /* draw the string to screen */
    MoveTo(x_pixel, y_pixel);
    DrawString(CtoPstr(temp_str));
    if (CF)
    {   destRect.left = destRect.right;
        destRect.right = windowWidth;
        EraseRect(&destRect);
    }
    SetPort(oldPort);
#endif /* INTERFACE == BASICM */

#if FRONTEND == BASIC
    if (slee < 0)
#if INTERFACE == BASICW
        Sleep((-slee)*1000);
#else /* INTERFACE == BASICW */
        sleep(-slee);
#endif /* INTERFACE == BASICW */
#ifdef TIERRA
    else if(slee&&HistoInEffect())
    {
#if INTERFACE == BASICW
        Sleep(slee*1000);
#else /* INTERFACE == BASICW */
        sleep(slee);
#endif /* INTERFACE == BASICW */
        query_species(fe_lines);
    }
#endif /* TIERRA */
#endif /* FRONTEND == BASIC */
}

/*
 * CtrBufStr - center string in buffer
 *
 * buf - string buffer
 * buflen - string buffer length
 * bufstr   - string to be centered
 */
void CtrBufStr(buf,buflen,bufstr)
char *buf,*bufstr;
{   tsprintf((char *)buf,"%*s%s\n",
        (int)((buflen-strlen((const char *)
        bufstr))/2)," ",bufstr);
}

/*
 * FEGetStr - position cursor and get character from keyboard
 *    x,y - cursor position
 *
 * returns: string in global FGSbuf buffer
 */
void FEGetStr(x, y)
    I32s x, y;
{
#if INTERFACE == BASICM
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
#endif /* INTERFACE == BASICM */

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)||(INTERFACE == BASICW)
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    tnl();
    techo();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */
    tmove(y, x);
#if INTERFACE != BASICW
    trefresh();
#endif /* INTERFACE != BASICW */
#if INTERFACE == BASICW
    gets(FGSbuf);
#else /* INTERFACE == BASICW */
    tiegetstr(FGSbuf);
#endif /* INTERFACE == BASICW */
    tiestrtme=tietime(NULL);
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    tnoecho();
    tnonl();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV)||(INTERFACE == BASICW) */

#if INTERFACE == BASICM
    EventRecord event;
    char character[2];
    GrafPtr oldPort;
    short x_pixel, y_pixel;
    Rect destRect;

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    strcpy(FGSbuf, "");
    character[1] = '\0';
    FEClrmsg(HLP_Y - 3, "");
    x_pixel = HLP_X * ((windowWidth - displayOffset + 5) / COLS) +
        displayOffset + 5;
    y_pixel = (HLP_Y - 3) * rowHeight + fontInfo.ascent;
    destRect.top = y_pixel - fontInfo.ascent;
    destRect.bottom = y_pixel + fontInfo.descent + fontInfo.leading;

    while (character[0] != '\n' && character[0] != '\r')
    {   if (GetNextEvent(everyEvent, &event))
        {   if (event.what == keyDown)
            {   character[0] = event.message & charCodeMask;
                strcat(FGSbuf, (char *) character);
                destRect.left = x_pixel;
                destRect.right = destRect.left + CharWidth(
                    (short) character[0]);
                EraseRect(&destRect);
                MoveTo(x_pixel, y_pixel);
                DrawChar((short) character[0]);
                x_pixel = destRect.right;
            }
        }
    }
    FEClrmsg(HLP_Y - 3, "");

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return;
#endif /* INTERFACE == BASICM */

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
    gotoxy(x+1, y+1);
    tgets(FGSbuf,  sizeof(FGSbuf));
    tiestrtme=tietime(NULL);
#else /* __GUI__ */
    int return_code;
    return_code = DialogBox(hInst,
        MAKEINTRESOURCE(GETSTRBOX), hwnd, lpfnGetStrDlgProc);
    /* gets text into FGSbuf thru GetStrDlgProc */
    if (return_code == -1)
    {   MessageBox(hwnd, "Unable to display dialog", "System Error",
            MB_ICONHAND);
    }
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if FRONTEND == STDIO
    tfgets(FGSbuf, 84, stdin);
    tiestrtme=tietime(NULL);
    if (FGSbuf[strlen((const char *)(&(FGSbuf[0])))-1]=='\n')
        FGSbuf[strlen((const char *)(&(FGSbuf[0])))-1]='\0';
#endif /* FRONTEND == STDIO */
}

/*
 * FEStats - update top two lines of Tierra display
 */
void FEStats()
{   I32s bfpsn;

    bfpsn=0;
    if (InstExe.m)
        bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),
            "InstExec  = %d,%.6d",
            InstExe.m, InstExe.i);
    else
        bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),
            "InstExec  = %6d", InstExe.i);

    bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),
        "  Cells = %4d", NumCells);

#ifndef CLSTRSRVR
    if(GeneBnker)
        bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),
            "  Genotypes = %4d  Sizes =%4d",
            NumGenotypes, NumSizes);
#endif /* CLSTRSRVR */

    bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),"\n");
    FEPrintf(0, 0, 1, 0, (char *)(&(Fbuf[0])));

#ifndef CLSTRSRVR
    bfpsn=0;
    if (GeneBnker)
        bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])),
            "Extracted = %-20.20s  ", ExtrG);
    bfpsn+=tsprintf((char *)(&(Fbuf[bfpsn])), "%s\n",
        (MC_step == (-1)) ? "normal" :
        (MC_step == 1) ? "step" : "delay");
    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
#endif /* CLSTRSRVR */

#if FRONTEND == BASIC
#if (INTERFACE != BASICD) && (INTERFACE != BASICB)
    tmove(2, 0);
#if INTERFACE != BASICW
    trefresh();
#endif /* INTERFACE != BASICW */
#endif /* (INTERFACE != BASICD) && (INTERFACE != BASICB) */
#ifndef CLSTRSRVR
    InstExeUpdNow=InstExeUpdFreq;
#endif /* CLSTRSRVR */
#endif /* FRONTEND == BASIC */
}

/*
 * T_sig_int - SIGINT signal handler
 */
#ifdef SIGBLOCK

#ifdef __TURBOC__
int T_sig_int(void)
#endif /* __TURBOC__ */

#ifdef __MWERKS__
void T_sig_int(sig)
I32s sig;
#endif /* __MWERKS__ */

#ifdef unix
void T_sig_int(sig)
int sig;
#endif /* unix */

#ifdef DECVAX
void T_sig_int(int sig)
#endif /* DECVAX */

{   sigset_t SigSaveSet;

    if(tisatty(0))
    {   SigSaveSet=TSigBlock(SigSet);

#ifdef DECVAX
        TieSignal(SIGINT, SIG_DFL);
        TieSignal(SIGINT, T_sig_int);
#endif /* DECVAX */
        TC_Menu = 1;

#ifdef __linux__
        TieSignal(SIGINT, T_sig_int);
#endif /* __linux__ */

        TSigRelease(SigSaveSet);
    }
#ifdef __TURBOC__
    return TC_Menu;
#else
    return;
#endif /* __TURBOC__ */
}
#endif /* SIGBLOCK */

/*
 * FEPrVQ - print "<parameter name> = <parameter value>"
 *          on Tierra display
 *
 * data - parameter name
 *
 */
void FEPrVQ(data)
    I8s *data;
{   I8s found = 1;

    if (vqu.type == 's')
        tsprintf((char *)(&(Fbuf[0])), "%s = %s\n", data, vqu.s);
    else if (vqu.type == 'h')
        tsprintf((char *)(&(Fbuf[0])), "%s = %hu\n", data, vqu.h);
    else if (vqu.type == 'i')
        tsprintf((char *)(&(Fbuf[0])), "%s = %d\n", data, vqu.i);
    else if (vqu.type == 'f')
        tsprintf((char *)(&(Fbuf[0])), "%s = %f\n", data, vqu.f);
    else if (vqu.type == 'd')
        tsprintf((char *)(&(Fbuf[0])), "%s = %f\n", data, vqu.d);
    else
        found = 0;
    if (found)
        FEPrintf(MSG_X, MSG_Y + 5, 0, 0, (char *)(&(Fbuf[0])));
}

#ifndef __GUI__
/*
 * FEMenu - Tierra main menu
 */
void FEMenu()
{   I8s answer;

#ifdef BGL
    TBglOneCellContFlg = 1;
#endif /* BGL */

#if INTERFACE == BASICC
#ifdef SIGBLOCK
    TieSignal(SIGINT, T_sig_int);
#endif /* SIGBLOCK */
#endif /* INTERFACE == BASICC */

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
#if INTERFACE == BASICV
#ifdef SIGBLOCK
    TieSignal(SIGINT, T_sig_int);
#endif /* SIGBLOCK */
#endif /* INTERFACE == BASICV */

#ifndef __linux__
    ttouchwin();
#endif /* __linux__ */
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */

#ifndef CLSTRSRVR
#if FRONTEND == STDIO
    tsprintf((char *)(&(Fbuf[0])),
        "\n==========================================================\n");
    FEPrintf(0, 0, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),"TIERRA: Main Menu");
    FEPrintf(0, 0, 1, 0, &(Fbuf[0]));
    tsprintf((char *)(&(Fbuf[0])),
        "VER=%5.2f PLOIDY=%d  %s %s %s %s %s \n", VER, PLOIDY

#ifdef ERRORTIE
        ,"ERROR"
#else /* ERRORTIE */
        ,""
#endif /* ERRORTIE */

#ifdef MEM_PROF
        ,"MEM_PROF"
#else /* MEM_PROF */
        ,""
#endif /* MEM_PROF */

#ifdef MICRO
        ,"MICRO"
#else /* MICRO */
        ,""
#endif /* MICRO */

        ,""

#ifdef BGL
        ,"BGL"
#else /* BGL */
        ,""
#endif /* BGL */
        );
    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "---------------------------------------------------------\n");
    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "InstExe.m    = %6d  InstExec.i = %6d  NumCells = %4d ",
        InstExe.m, InstExe.i, NumCells);
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    if (GeneBnker)
    {   tsprintf((char *)(&(Fbuf[0])),
            "NumGenotypes =   %4d  NumSizes   =   %4d",
            NumGenotypes, NumSizes);
        FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    }
#ifdef __TURBOC__
    tsprintf((char *)(&(Fbuf[0])), "CoreLeft     =  %d", coreleft());
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
#endif /* __TURBOC__ */

    tsprintf((char *)(&(Fbuf[0])),
        "---------------------------------------------------------\n");
    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "        Key      Function\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "        i        Information on simulation\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "        v        Change or examine a soup_in variable\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "        m        Misc. commands\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "        S        Execute a system Shell\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "        s        Save the soup\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "        q        Save the soup & quit\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "        Q        Quit/Abort simulation\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "        c        To Continue simulation\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "---------------------------------------------------------\n");
    FEPrintf(1, 1, 1, 0, (char *)(&(Fbuf[0])));
#endif /* FRONTEND == STDIO */
#endif /* CLSTRSRVR */

    while (1)
    {   FEVidInit((I32s)0);
#ifndef CLSTRSRVR
        tsprintf((char *)(&(Fbuf[0])),"%s%s",
            "i-info  v-var  s-save  S-shell  q-save&quit",
            "  Q-quit  m-misc  c-continue |->");
#else /* CLSTRSRVR */
        tsprintf((char *)(&(Fbuf[0])), "v-var  Q-quit  c-continue |->");
#endif /* CLSTRSRVR */

        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        answer=(I8s)FEGetch(HLP_X+strlen(
            (const char *)(&(Fbuf[0]))), HLP_Y);
        if (answer == 'c')
            break;
#ifndef CLSTRSRVR
        else if (answer == 's')
            GoSave();
        else if (answer == 'S')
            GoShell();
        else if (answer == 'q')
            GoSaveQuit();
#endif /* CLSTRSRVR */
        else if (answer == 'Q')
            GoQuit();
        else if (answer == 'v')
            GoVariable();
#ifndef CLSTRSRVR
        else if (answer == 'i')
            GoInfo();
        else if (answer == 'm')
            GoMisc();
#endif /* CLSTRSRVR */
        answer = ' ';
    }           /* end while loop */
#ifdef TIERRA
    if ((FRONTEND == STDIO)||(!HistoInEffect()))
#endif /* TIERRA */
        FEClrmsg(MSG_Y,
            ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                           "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
    TC_Menu = 0;
}   /* end of FEMenu */
#endif /* __GUI__ */

#ifndef CLSTRSRVR
/*
 * GoSave - confirm soup save selection and save soup
 */
void GoSave()
{   I8s answer;

    FEVidInit((I32s)0);
    tsprintf((char *)(&(Fbuf[0])), "Are you sure ? (y/n)->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    if ((answer == 'y') || (answer == 'Y'))
    {   tsprintf((char *)(&(Fbuf[0])),"Writing soup ...\n");
        FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
        WriteSoup(0);
    }
}
#ifndef __GUI__

/*
 * GoShell - invoke system shell
 */
void GoShell()
{   tsprintf((char *)(&(Fbuf[0])),
        "TIERRA: shell (%s), type exit to return ... \n", TIESHELL);
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    techo();
    tnl();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    tnocbreak();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    tclear();
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
         (INTERFACE == BASICV) */

    tendwin();

#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

    system(TIESHELL);
}
#endif /* __GUI__ */

/*
 * GoSaveQuit - save soup and quit on confirmation
 */
void GoSaveQuit()
{   I8s answer;

    FEVidInit((I32s)0);
    tsprintf((char *)(&(Fbuf[0])), "Are you sure ? (y/n)->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    if ((answer == 'y') || (answer == 'Y'))
    {   tsprintf((char *)(&(Fbuf[0])), "Writing soup ...\n");
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
        WriteSoup(1);
        tsprintf((char *)(&(Fbuf[0])), "Tierra going down ...\n");
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));

#ifdef NET
        if (ping_fp)
        {   tfclose(ping_fp);
            ping_fp=NULL;
        }
#endif /* NET */

        FEExit(0);
    }
}
#endif /* CLSTRSRVR */

#ifndef __GUI__

/*
 * GoQuit - quit without soup save on confirmation
 */
void GoQuit()
{   I8s answer;

    FEVidInit((I32s)0);
    tsprintf((char *)(&(Fbuf[0])), "Are you sure ? (y/n)->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer = (I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    if ((answer == 'y') || (answer == 'Y'))
        FEExit(0);
}

/*
 * GoVariable - Tierra parameter inquiry and alteration menu
 */
void GoVariable()
{   I8s answer;

    FEVidInit((I32s)0);
    tsprintf((char *)(&(Fbuf[0])),
        "VAR |  a-alter variable  e-examine variable->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    if (answer == 'a')
        GoAlter();
    else if (answer == 'e')
        GoExamine();
    answer = ' ';
}
#endif /* __GUI__ */

/*
 * GoAlter - Tierra parameter alteration
 */
void GoAlter()
{   I8s answer;
#ifdef NET
    I32s TBasSubNetChgFrq, TSubNetCnt, TTieSubNet;
    double TSubNetChgFrqRanFrac, TSubNetChgFrqFixFrac;
#endif /* NET */
#ifndef CLSTRSRVR
    I32s TThreadAnalysis;
#endif /* CLSTRSRVR */
    int getavarrtn;

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

#ifdef NET
    TTieSubNet=TieSubNet;
    TSubNetCnt=SubNetCnt;
    TBasSubNetChgFrq=BasSubNetChgFrq;
    TSubNetChgFrqRanFrac=SubNetChgFrqRanFrac;
    TSubNetChgFrqFixFrac=SubNetChgFrqFixFrac;
#endif /* NET */
#ifndef CLSTRSRVR
    TGeneBnker = GeneBnker;
    TThreadAnalysis=ThreadAnalysis;
#endif /* CLSTRSRVR */

    tsprintf((char *)(&(Fbuf[0])),
        "To alter any global variable from soup_in, type\n");
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "the variable name (using proper case), a space,\n");
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "an equal sign, a space, and the new value.\n");
    FEPrintf(PLN_X, PLN_Y + 2, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Use no space at start of line.  Some examples:\n");
    FEPrintf(PLN_X, PLN_Y + 3, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "alive = 0\n");
    FEPrintf(PLN_X, PLN_Y + 4, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "DistProp = .6\n");
    FEPrintf(PLN_X, PLN_Y + 5, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "GenebankPath = newpath/\n");
    FEPrintf(PLN_X, PLN_Y + 6, 0, 0, (char *)(&(Fbuf[0])));
    FEGetStr(HLP_X, HLP_Y - 3);
    if (!(getavarrtn=GetAVar(FGSbuf, 1, 0, sizeof(FGSbuf))))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra T_sig_int() Not a valid soup_in variable: %s\n",
            FGSbuf);
        FEError(-500, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    else
    {   ToggleLog(TierraLog, &tfp_log,
            (I8s *)TIERRALOGNAME, &tielogpos);
#ifdef NET
        ToggleLog(PingDatLog, &ping_fp,
            (I8s *)TPINGLOGNAME, &tpinglogpos);
#endif /* NET */
        tsprintf((char *)(&(Fbuf[0])),
            "FE: Var change @ %d,%.5d - %.30s\n",
            InstExe.m, InstExe.i, FGSbuf);
        FEPrintf(PLN_X, PLN_Y + 8, 0, 0, (char *)(&(Fbuf[0])));
        FEBanner();
        FEStats();
        CtrBufStr(&(Fbuf[0]),COLS, "hit any key to continue");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        answer=(I8s)FEGetch(HLP_X+strlen(
            (const char *)(&(Fbuf[0]))), HLP_Y);
    }

#ifndef CLSTRSRVR
    if(TGeneBnker!=GeneBnker)
    {   if((TGeneBnker)&&(!GeneBnker))
        {   GeneBnkerOvrd=0;
            CloseGeneBanker();
        }
        else
            if((!TGeneBnker)&&(GeneBnker))
            {   GeneBnkerOvrd=1;
                OpenGeneBanker();
            }
    }
    if(TThreadAnalysis!=ThreadAnalysis)
    {   if((TThreadAnalysis)&&(!ThreadAnalysis))
        {   CloseGeneBanker();
            ThrdAnaDatDump();
        }
        else
        {   if((!TThreadAnalysis)&&(ThreadAnalysis))
            {   OpenGeneBanker();
                ThrdAnaTimeLastOn=(I32s)tietime(NULL);
            }
        }
    }
#endif /* CLSTRSRVR */

#ifdef NET
    if((TSubNetCnt!=SubNetCnt)||
        (TBasSubNetChgFrq!=BasSubNetChgFrq)||
        (TSubNetChgFrqRanFrac!=SubNetChgFrqRanFrac)||
        (TSubNetChgFrqFixFrac!=SubNetChgFrqFixFrac)||
        (TTieSubNet!=TieSubNet))
        TieCommChgSubNet(TTieSubNet,0,0,1);
#endif /* NET */
#ifdef TIERRA
    if(TierraSleep)
    {   CpuLoadLimitRunTime=(I32s)(CpuLoadLimitProp*
            ((double)CpuLoadLimitPeriod));
        CpuLoadLimitSleepTime=(I32s)((1.0-CpuLoadLimitProp)*
            ((double)CpuLoadLimitPeriod));
    }
#endif /* TIERRA */
}

/*
 * GoExamine - Tierra parameter inquiry
 */
void GoExamine()
{   I8s answer;

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    tsprintf((char *)(&(Fbuf[0])),
        "To examine any global variable from soup_in,\n");
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "the name of the variable:\n");
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    FEGetStr(HLP_X, HLP_Y - 3);
    if (!GetAVar(FGSbuf, 0, 0, sizeof(FGSbuf)))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra T_sig_int() Not a valid soup_in variable: %s\n",
            FGSbuf);
        FEError(-501, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
        FEBanner();
        FEStats();
    }
    else
    {   FEPrVQ(FGSbuf);
        FEBanner();
        FEStats();
        CtrBufStr(&(Fbuf[0]),COLS, "hit any key to continue");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        answer=(I8s)FEGetch(HLP_X+strlen(
            (const char *)(&(Fbuf[0]))), HLP_Y);
    }
}

#ifndef CLSTRSRVR
/*
 * GoInfo - Tierra histogram mode menu
 */
void GoInfo()
{   I8s answer;

    FEVidInit((I32s)0);
#ifdef MEM_PROF
    tsprintf((char *)(&(Buf85[0])),
        "TotNumAloc: %8d DynMemUse: %8d seed: %d",
        MemStat.TotNumAloc, MemStat.DynMemUse, seed);
#else /* MEM_PROF */
    tsprintf((char *)(&(Buf85[0])), "seed: %d", seed);
#endif /* MEM_PROF */

    tsprintf((char *)(&(Fbuf[0])), "%s\n", (char *)(&(Buf85[0])));
#ifndef __GUI__
    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "INFO | p-plan s-size g-gen m-mem z-size_query e-reprod_eff >");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    switch (answer)
    {   case 'p':
            histostrtidx = 0;
            GoPlan();
            break;
        case 's':
            histostrtidx = 0;
            GoSizeHist();
            break;
        case 'm':
            histostrtidx = 0;
            GoMem();
            break;
        case 'g':
            histostrtidx = 0;
            GoGene();
            break;
        case 'z':
            histostrtidx = 0;
            GoSizeQuery();
            break;
        case 'e':
            GoReprodEff();
            break;
        default:
            break;
    }
    answer = ' ';
#else /* __GUI__ */
    MessageBox(hwnd, (char *)(&(Fbuf[0])), "Tierra Info", MB_OK);
#endif /* __GUI__ */
}

/*
 * GoPlan - turn off histogram display (default)
 */
void GoPlan()
{   if((IMode!=PLN_STATS)&&(IMode!=SCRN_DEBUG))
        CloseGeneBanker();
    if (Hist)
    {   thfree((void *) Hist, HistAlocSiz, 2101);
        Hist = (HistType *) NULL;
        HistAlocSiz = 0;
    }
    IMode = PLN_STATS;
}

/*
 * GoSizeQuery - size class inquiry menu
 */

void GoSizeQuery()
{   I32s abortflag, num_spec;
    I32s size_class;
    size_buf *buf;

    OpenGeneBanker();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    /* enter size class */
    size_class = EnterLongVal(PLN_Y+1,
        "Enter a size class ( eg: 80 ) to examine -> ",
        &abortflag);
    if (!abortflag)
    {   if ((size_class > 0) && (size_class < siz_sl))
        {   if (sl[size_class])
            {   num_spec = sl[size_class]->num_g;
                /* initialize size query buffer */
                if ( (buf = InitSizeQuery(size_class, num_spec)) != 0 )
                {   /* display size query buffer */
                    DispSizeQuery(buf, num_spec, size_class);
                    thfree((void *) buf, num_spec * sizeof(size_buf), 2100);
                }
            }
        }
    }
    /* query termination screen handling */
    CloseGeneBanker();
}

/*
 * GoSizeHist - set size histogram display mode
 */
void GoSizeHist()
{   int abortflag;
    SizeSortOrder(&abortflag, SIZ_HIST);
    if (!abortflag)
    {   if(!HistoInEffect())
            OpenGeneBanker();
        IMode = SIZ_HIST;
    }
}

/*
 * GoMem - set memory histogram display mode
 */
void GoMem()
{   int abortflag;
    SizeSortOrder(&abortflag, SIZM_HIST);
    if (!abortflag)
    {   if(!HistoInEffect())
            OpenGeneBanker();
        IMode = SIZM_HIST;
    }
}

/*
 * GoGene - set gene histogram display mode
 */
void GoGene()
{   int abortflag;
    GenSortOrder(&abortflag, GEN_HIST);
    if (!abortflag)
    {   if(!HistoInEffect())
            OpenGeneBanker();
        IMode = GEN_HIST;
    }
}

/*
 * GoMisc - misc menu
 *          histogram logging control
 *          genome injection
 *          normal/delay/step run mode
 *          break trap menu
 *          histogram paging/scrolling controls
 *          on/off line control
 *    
 */
void GoMisc()
{   I8s  answer;

    FEVidInit((I32s)0);
    tsprintf((char *)(&(Fbuf[0])),
        "VER=%5.2f PLOIDY=%d  %s %s %s %s %s\n", VER,PLOIDY,
#ifdef ERRORTIE
        "ERROR",
#else /* ERRORTIE */
        "",
#endif /* ERRORTIE */
#ifdef MEM_PROF
        "MEM_PROF",
#else /* MEM_PROF */
        "",
#endif /* MEM_PROF */
#ifdef MICRO
        "MICRO",
#else /* MICRO */
        "",
#endif /* MICRO */
        "",
#ifdef BGL
        "BGL"
#else /* BGL */
        ""
#endif /* BGL */
        );

#ifdef __GUI__

    MessageBox(hwnd, (char *)(&(Fbuf[0])),
        "Tierra Simulation Status", MB_OK);

#else /*__GUI__ */

    FEPrintf(0, 1, 1, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "H-Histo Log  I-Inj Gene  %s  %s  %s  %s  %s->\n",

#ifdef MICRO
        "M-Micro Tog",
#else /* MICRO */
        "",
#endif /* MICRO */

        "",

#ifdef BGL
        "P-Bgl Pause",
#else /* BGL */
        "",
#endif /* BGL */


#ifdef MICRO
        "B-Brk Menu",
#else /* MICRO */
        "",
#endif /* MICRO */
        "m-more"
        );
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))),HLP_Y);
    switch(answer)
    {   case 'H':                  /* Histo Logging */
           GoHistoLog();
           break;
       case 'I':                  /* Inject Gene */
           GoInject();
           break;
       case 'M':                  /* Micro Toggle */
           GoMicroToggle();
           break;
#ifdef BGL
       case 'P':                  /* Beagle Pause */
           GoBglComm();
           break;
#endif /* BGL */
#ifdef MICRO
       case 'B':                  /* Break Menu */
           GoBrkMenu();
           break;
#endif /* MICRO */
       case 'm':                  /* more */
           GoMiscMore();
           break;
       default:
           break;
    }
    answer = ' ';
#endif /*__GUI__ */
}

/*
 * GoHistoLog - histogram logging toggle - uses tierra.log
 */
void GoHistoLog()
{   HistPrint = (TierraLog && !HistPrint) ? 1 : 0;
    tsprintf((char *)(&(Fbuf[0])),
        "%s \n", (TierraLog) ? (HistPrint) ? "Logging Histograms \n" :
        "NOT Logging Histograms \n" : "Log NOT on ! \n");
#ifdef __GUI__
    MessageBox(hwnd, (char *)(&(Fbuf[0])),
        "Tierra Simulation Status", MB_OK);
#else /* __GUI */
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /*__GUI__ */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
#ifdef BGL
void GoBglComm()
{   TBglOneCellContFlg = (TBglOneCellContFlg) ? 0 : 1;
    tsprintf((char *)(&(Fbuf[0])), " Beagle Pause = %s\n",
        (TBglOneCellContFlg) ? "off" : "on");
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
}
#endif /* BGL */

/*
 * GoInject - manual genome injection function
 */
void GoInject()
{   I32s injectnum, prefinjaddr, actnuminject;
    I32s abortflag;
    GlIndex injgen;
    char injgennam[8];
    GList *g;

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    abortflag = 0;

    /* enter genome to be injected */
    for(;;)
    {   injgen = EnterGenotype(PLN_Y,
            "Enter genome to be injected (eg 0080aaa) (enter to abort)->",
            &abortflag, 0);
        if (!abortflag)
        {   tsprintf(&(injgennam[0]), "%.4d%-3s",
                injgen.si, Int2Lbl(injgen.gi));
            if((g=GetAGen((I8s *)(&(injgennam[0])))) != 0 )
            {   FreeGen(g);
                break;
            }
        }
        else
            break;
    }
    if (!abortflag)
    {   for(;;)
        {   /* enter number of creatures to be injected */
            injectnum = EnterLongVal(PLN_Y + 1,
                "Enter number of creatures to inject (enter to abort)->",
                &abortflag);
            if (!abortflag)
            {   if (injectnum < 1)
                {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-502, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
                else
                    break;
            }
            else
                break;
        }
        if (!abortflag)
        {   /* display soup limits */
            tsprintf((char *)(&(Fbuf[0])),
                "soup address limits = 0 - %d\n", SoupSize-1);
            FEPrintf(PLN_X, PLN_Y + 2, 0, 0, (char *)(&(Fbuf[0])));

            for(;;)
            {   /* enter preferred injection address */
                prefinjaddr = EnterLongVal(PLN_Y + 3,
       "Enter preferred address, (Enter to abort, or -1 for no preference)->",
                    &abortflag);
                if (!abortflag)
                {   if ((prefinjaddr >= 0) && (prefinjaddr < SoupSize))
                        break;
                    else
                    {   if (prefinjaddr == (-1))
                            break;
                        else
                        {   tsprintf((char *)(&(Fbuf[0])),
                                "invalid soup address");
                            FEError(-503, NOEXIT, NOWRITE,
                                (char *)(&(Fbuf[0])));
                        }
                    }
                }
                else
                    break;
            }
            if (!abortflag)
            {   actnuminject=InjectFromBank(
                    (I8s *)(&(injgennam[0])),
                    prefinjaddr,0,injectnum);
                if (actnuminject != injectnum)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "only %d actually injected",actnuminject);
                    FEError(-505, NOEXIT, NOWRITE,
                        (char *)(&(Fbuf[0])));
                }
            }
        }
    }
}
#endif /* CLSTRSRVR */

/*
 * FEClrmsg - clear Tierra display message area
 */
void FEClrmsg(n, buf)
    I32s n;
    I8s *buf;
{
#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
    I8s t;
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if INTERFACE == BASICM
    GrafPtr oldPort;
    Rect destRect;
#endif /* INTERFACE == BASICM */

    if (n < 0)
        return;

#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
    for (t = n + 1; t <= fe_lines; t++)
    {
#ifndef __GUI__
        gotoxy(1, t);
        clreol();
#else /* _GUI__ */
        FEPrintf(20, t - 1, 0, 0, ENDBLANK);
        FEPrintf(0, t - 1, 0, 0, ENDBLANK);
#endif /* __GUI__ */
    }
    FEPrintf(HLP_X, HLP_Y, 1, 0, buf);
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE==BASICC)||(INTERFACE==BASICW)||\
    (INTERFACE==BASICA)||(INTERFACE==BASICV)
    tmove(n, 0);
    tclrtobot();
    FEPrintf(HLP_X, HLP_Y, 1, 0, buf);
#if (INTERFACE==BASICC)||(INTERFACE==BASICA)||\
    (INTERFACE==BASICV)
    trefresh();
#endif /* (INTERFACE==BASICC)||(INTERFACE==BASICA)||
          (INTERFACE==BASICV) */
#endif /* (INTERFACE==BASICC)||(INTERFACE==BASICW)||
          (INTERFACE==BASICA)||(INTERFACE==BASICV) */

#if INTERFACE == BASICM
    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    destRect.left = displayOffset + 1;
    destRect.right = windowWidth;
    destRect.top = (n) * rowHeight;
    destRect.bottom = (n + 1) * rowHeight;
    EraseRect(&destRect);
    SetPort(oldPort);
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)buf);
#endif /* INTERFACE == BASICM */

#if FRONTEND == STDIO
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)buf);
#endif /* FRONTEND == STDIO */
}

/*
 * FEVidInit - initialize display for selected display/histogram mode
 * mode - display mode
 */
#if (OPSYS == DECVMS) || (OPSYS == MACTIE)
void FEVidInit(I32s mode)
#else /* (OPSYS == DECVMS) || (OPSYS == MACTIE) */
void FEVidInit(mode)
    I32s mode;
#endif /* (OPSYS == DECVMS) || (OPSYS == MACTIE) */
{
#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)

#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
    tcbreak();
    tnonl();
    tnoecho();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */

    FEClear();

#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICB
#if (__TURBOC__ >= 0x200) && (__TURBOC__ < 0x400)
    /* deal with old and (stupid) new compilers */
    if (mode == EGA)
        textmode(C4350);  /* use C4350 for ega/vga */
    else if (mode == VGA)
        textmode(C4350);
    else
#endif /* (__TURBOC__ >= 0x200) && (__TURBOC__ < 0x400) */
    {
#ifndef __GUI__
        textmode(C80);
#endif /* __GUI__ */
    }
#endif /* INTERFACE == BASICB */

#if INTERFACE == BASICD
    textmode(C80);
#endif /* INTERFACE == BASICD */

#if FRONTEND != STDIO
    SetPlanMSG_Y();
    FEBanner();
    FEStats();
#endif /* FRONTEND != STDIO */

#ifdef TIERRA
    if(HistoInEffect())
        query_species(fe_lines);
#if FRONTEND == BASIC
    else if(IMode==SCRN_DEBUG)
        DispDebuggerScrn(ce);
    else if (IMode == PLN_STATS)
        FEPlan(0);
#endif /* FRONTEND == BASIC */
#else /* TIERRA */
    srvrFEPlan(0);
#endif /* TIERRA */
}

/*
 * KeyHit - returns 1 if key is hit
 */
#if (OPSYS == BCDOS) || (OPSYS == DJGPPDOS) || (OPSYS == WIN32TIE)
I8s KeyHit()
{   if(!TC_Menu)
    {
#ifndef __GUI__
        if(kbhit())
        {   getch();
            TC_Menu=1;
        }
#else /* __GUI__ */
        if(GetKeyState(VK_SPACE)<0)
            TC_Menu=1;
#endif /* __GUI__ */
    }
    return (I8s)TC_Menu;
}
#endif /* (OPSYS == BCDOS) || (OPSYS == DJGPPDOS) || (OPSYS == WIN32TIE) */

/*
 * FEError - display error code and message
 * errno1  - error code to be displayed
 * exit    - 0    - do not exit Tierra 
 *         - <> 0 - exit Tierra
 * write   - 0    - do not save soup on error
 *         - 1    - save soup on error
 * buff    - message to display
 *
 */
void FEError(errno1, exit, write, buff)
    I32s errno1, exit, write;
    char *buff;
{   I8s ch;

    if (buff[0] != 0)
    {   FEPrintf(ERR_X, ERR_Y, 1, 0, buff);

#if FRONTEND == STDIO
        FEPrintf(ERR_X, ERR_Y, 0, 0, "\n");
#endif /* FRONTEND == STDIO */

#ifdef BGL
        TBgl_make_femessages((I8s *)buff, strlen((const char *)buff));
#endif /* BGL */

        if (TierraLog && tfp_log)
            tfprintf(tfp_log, "%s\n", buff);
    }

#ifndef BGL
#ifndef __GUI__
    if (strlen(buff) > 1)
    {   tsprintf((char *)(&(Fbuf[0])),
"                     Press any key to continue               ");
        FEPrintf(HLP_X, HLP_Y, 1, 0,&(Fbuf[0]));

#ifdef DECVAX
        ch = FEGetch(HLP_X + 74, HLP_Y);
#else /* DECVAX */
        ch = (I8s)FEGetch(-1, -1);
#endif /* DECVAX */

#if FRONTEND == BASIC
        FEClrmsg(ERR_Y, "");
        FEBanner();
        FEStats();
#endif /* FRONTEND == BASIC */
    }
#endif /* __GUI__ */
#endif /* BGL */

#ifndef CLSTRSRVR
    if (write == WRITE)
    {   tsprintf((char *)(&(Fbuf[0])), "Writing soup ...");
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
        WriteSoup(exit);
    }
#endif /* CLSTRSRVR */

    if (exit == EXIT)
    {   tsprintf((char *)(&(Fbuf[0])), "going down ... ");
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));

        tsprintf((char *)(&(Fbuf[0])),
#ifdef __GUI__
            "Tierra exiting");
        MessageBox(hwnd, (char *)(&(Fbuf[0])),
            "Tierra Error", MB_ICONEXCLAMATION | MB_OK);
#else /* __GUI__ */
"                      Tierra exiting  (PRESS ANY KEY)                   \n");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));

#ifndef CLSTRSRVR
        if (hangup)
            FEMenu();
#endif /* CLSTRSRVR */

#ifdef DECVAX
        ch = FEGetch(HLP_X + 72, HLP_Y);
#else /* DECVAX */
        ch = (I8s)FEGetch(-1, -1);
#endif /* DECVAX */

#endif /* __GUI__ */
        FEExit(errno1);
    }
    if (TierraLog && tfp_log)
    {
#ifndef AMIGA
#ifndef DECVAX
        tfflush(tfp_log);
#endif /* DECVAX */
#endif /* AMIGA */
    }
}

#ifndef CLSTRSRVR
/*
 * FEPlan - display Plan mode display screen
 *          tielog - 0 - do not add entries to tierra.log
 *                   1 - add entries to tierra.log
 */
void FEPlan(tielog)
    I32s tielog;
{   time_t tp;
    double gtypentrp, szclsentrp;

    tp = tietime(NULL);
    tsprintf((char *)(&(Fbuf[0])),
        "InstExeC  %8d  Generation%8.0f  %11d   %s",
        InstExe.m, Generations, tp, ctime(&tp));

    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    if (GeneBnker)
    {   tsprintf((char *)(&(Fbuf[0])),
        "NumCells  %8d  NumGen    %8d  NumSizes  %8d  Speed     %8d\n",
            NumCells, NumGenotypes,
            NumSizes, Speed);
        FEPrintf(PLN_X,PLN_Y+1,0,0,(char *)(&(Fbuf[0])));
    }
    else /* (GeneBnker) */
    {   tsprintf((char *)(&(Fbuf[0])),
            "NumCells  %8d  Speed     %8d\n",
            NumCells, Speed);
        FEPrintf(PLN_X,PLN_Y+1,0,0,(char *)(&(Fbuf[0])));
    }
    tsprintf((char *)(&(Fbuf[0])),
        "RateMut   %8d  RatMovMut %8d  RateFlaw  %8d  AgeAvg    %8d\n",
        RateMut, RateMovMut, RateFlaw, AgeAvg);
    FEPrintf(PLN_X, PLN_Y + 2, 0, 0, (char *)(&(Fbuf[0])));

    tsprintf((char *)(&(Fbuf[0])),
"AvgSize   %8d  AvgPop    %8.0f  FecundAvg %8.1f  RepInstEf %8.1f\n",
        AverageSize, AvgPop, FecundityAvg, RepInstEff);
    FEPrintf(PLN_X, PLN_Y + 3, 0, 0, (char *)(&(Fbuf[0])));


    tsprintf((char *)(&(Fbuf[0])),
#ifdef NET
        "Births    %8d  BirthLoca %8d  BirthInje %8d  BirthEjec %8d\n"
#else /* NET */
        "Births    %8d\n"
#endif /* NET */

        , FEBirthNum
#ifdef NET
        , FEBirthLocal, FEBirthInject, FEBirthEject
#endif /* NET */
    );
    FEPrintf(PLN_X, PLN_Y + 4, 0, 0, (char *)(&(Fbuf[0])));

    tsprintf((char *)(&(Fbuf[0])),
#ifdef NET
        "Deaths    %8d  DeathFecY %8d  DeathFecZ %8d  DeathEjec %8d\n"
#else /* NET */
        "Deaths    %8d\n"
#endif /* NET */
        , FEDeathNum
#ifdef NET
        , FEFecundYesSum, FEFecunZeroSum, FEFecunEjecSum
#endif /* NET */
    );
    FEPrintf(PLN_X, PLN_Y + 5, 0, 0, (char *)(&(Fbuf[0])));

    if (GeneBnker)
    {   tsprintf((char *)(&(Fbuf[0])),
"MaxGenPop %8d  (%.4d%3.3s) MaxGenMem %8d (%.4d%3.3s)  NumGenDG  %8d\n",
            MaxPop, MaxGenPop.size, MaxGenPop.label,
            MaxMemry, MaxGenMem.size, MaxGenMem.label,
            NumGenDG);
        FEPrintf(PLN_X, PLN_Y + 6, 0, 0, (char *)(&(Fbuf[0])));
    }

#ifdef NET
    tsprintf((char *)(&(Fbuf[0])),
"SImmigMs  %8d  SImmigPk  %8d  SImmigBt  %8d  NumNodes  %8hu\n",
        FEMonCom[EMIGRATETAG].SNumMess,
        FEMonCom[EMIGRATETAG].SNumPack,
        FEMonCom[EMIGRATETAG].SNumByte,
        NumNodesC);
    FEPrintf(PLN_X,PLN_Y+6+((GeneBnker)?1:0),
        0,0,(char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"SPingQMs  %8d  SPingQPk  %8d  SPingQBt  %8d  EjToSelf  %8d\n",
        FEMonCom[TPINGREQTAG].SNumMess,
        FEMonCom[TPINGREQTAG].SNumPack,
        FEMonCom[TPINGREQTAG].SNumByte,
        FEEjectToSelf);
    FEPrintf(PLN_X,PLN_Y+7+((GeneBnker)?1:0),
        0,0,(char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"SPingPMs  %8d  SPingPPk  %8d  SPingPBt  %8d  RImmigMs  %8d\n",
        FEMonCom[TPINGREPTAG].SNumMess,
        FEMonCom[TPINGREPTAG].SNumPack,
        FEMonCom[TPINGREPTAG].SNumByte,
        FEMonCom[EMIGRATETAG].RNumMess);
    FEPrintf(PLN_X, PLN_Y+8+((GeneBnker)?1:0),
        0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"SAllMesg  %8d  SAllPack  %8d  SAllByte  %8d  RPingQMs  %8d\n",
        FEMonCom[0].SNumMess,
        FEMonCom[0].SNumPack,
        FEMonCom[0].SNumByte,
        FEMonCom[TPINGREQTAG].RNumMess);
    FEPrintf(PLN_X, PLN_Y+9+((GeneBnker)?1:0),
        0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"RAllMesg  %8d  RAllPack  %8d  RAllByte  %8d  RPingPMs  %8d\n",
        FEMonCom[0].RNumMess,
        FEMonCom[0].RNumPack,
        FEMonCom[0].RNumByte,
        FEMonCom[TPINGREPTAG].RNumMess);
    FEPrintf(PLN_X, PLN_Y+10+((GeneBnker)?1:0),
        0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"SAllMs/S  %8d  SAllPk/S  %8d  SAllBt/S  %8d\n",
        FEMonCom[0].SNumMess / FESpeed,
        FEMonCom[0].SNumPack / FESpeed,
        FEMonCom[0].SNumByte / FESpeed);
    FEPrintf(PLN_X, PLN_Y+11+((GeneBnker)?1:0),
        0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"RAllMs/S  %8d  RAllPk/S  %8d  RAllBt/S  %8d\n",
        FEMonCom[0].RNumMess / FESpeed,
        FEMonCom[0].RNumPack / FESpeed,
        FEMonCom[0].RNumByte / FESpeed);
    FEPrintf(PLN_X, PLN_Y+12+((GeneBnker)?1:0),
        0, 0, (char *)(&(Fbuf[0])));

#endif /* NET */

#ifdef MEM_PROF
#if (defined __TURBOC__) && (!defined __GUI__)
    tsprintf((char *)(&(Fbuf[0])),
        "DynMemUse %8d  TotNumAlo %8d  Coreleft  %8u\n",
        MemStat.DynMemUse, MemStat.TotNumAloc, (I32u) coreleft());
#else /* (defined __TURBOC__) && (!defined __GUI__) */
    tsprintf((char *)(&(Fbuf[0])),
        "DynMemUse %8d  TotNumAlo %8d\n",
        MemStat.DynMemUse, MemStat.TotNumAloc);
#endif /* (defined __TURBOC__) && (!defined __GUI__) */
    FEPrintf(PLN_X, PLN_Y+6+
#ifdef NET
        7
#endif /* NET */
        +((GeneBnker)?1:0), 0, 0, (char *)(&(Fbuf[0])));
#endif /* MEM_PROF */

    if (TierraLog && tielog)
    {   tfprintf(tfp_log, "\nie%d gn%.0f ti%d %s",
            InstExe.m, Generations, tp, ctime(&tp));
        if (GeneBnker)
            tfprintf(tfp_log, "nc%d ng%d ns%d sp%d\n",
                NumCells, NumGenotypes,
                NumSizes, Speed);
        else
            tfprintf(tfp_log, "nc%d ns%d sp%d\n",
                NumCells, NumSizes, Speed);
        tfprintf(tfp_log, "rm%d mm%d rf%d aa%d\n",
            RateMut, RateMovMut, RateFlaw, AgeAvg);
        tfprintf(tfp_log, "as%d ap%.0f fa%.1f re%.1f\n",
            AverageSize, AvgPop, FecundityAvg, RepInstEff);

        tfprintf(tfp_log, "bn%d", FEBirthNum);
#ifdef NET
        tfprintf(tfp_log, " bl%d bi%d be%d",
            FEBirthLocal, FEBirthInject, FEBirthEject);
#endif /* NET */
        tfprintf(tfp_log, "\n");

        tfprintf(tfp_log, "dn%d", FEDeathNum);
#ifdef NET
        tfprintf(tfp_log, " dy%d dz%d de%d",
            FEFecundYesSum, FEFecunZeroSum, FEFecunEjecSum);
#endif /* NET */
        tfprintf(tfp_log, "\n");

        if (GeneBnker)
        {   CalcEntropy(&gtypentrp, &szclsentrp);
            tfprintf(tfp_log,
            "mp%d @ %.4d%3.3s mg%d @ %.4d%3s dg%d gten %f szen %f\n",
                MaxPop, MaxGenPop.size, MaxGenPop.label,
                MaxMemry, MaxGenMem.size, MaxGenMem.label,
                NumGenDG, gtypentrp, szclsentrp);
        }

#ifdef NET
        tfprintf(tfp_log, "sim%d sip%d sib%d nn%hu\n",
            FEMonCom[EMIGRATETAG].SNumMess,
            FEMonCom[EMIGRATETAG].SNumPack,
            FEMonCom[EMIGRATETAG].SNumByte,
            NumNodesC);
        tfprintf(tfp_log, "spqm%d spqp%d spqb%d es%d\n",
            FEMonCom[TPINGREQTAG].SNumMess,
            FEMonCom[TPINGREQTAG].SNumPack,
            FEMonCom[TPINGREQTAG].SNumByte,
            FEEjectToSelf);
        tfprintf(tfp_log, "sppm%d sppp%d sppb%d rim%d\n",
            FEMonCom[TPINGREPTAG].SNumMess,
            FEMonCom[TPINGREPTAG].SNumPack,
            FEMonCom[TPINGREPTAG].SNumByte,
            FEMonCom[EMIGRATETAG].RNumMess);
        tfprintf(tfp_log, "sam%d sap%d sab%d rpqm%d\n",
            FEMonCom[0].SNumMess,
            FEMonCom[0].SNumPack,
            FEMonCom[0].SNumByte,
            FEMonCom[TPINGREQTAG].RNumMess);
        tfprintf(tfp_log, "ram%d rap%d rab%d rppm%d\n",
            FEMonCom[0].RNumMess,
            FEMonCom[0].RNumPack,
            FEMonCom[0].RNumByte,
            FEMonCom[TPINGREPTAG].RNumMess);
        tfprintf(tfp_log, "sams%d saps%d sabs%d\n",
            FEMonCom[0].SNumMess / FESpeed,
            FEMonCom[0].SNumPack / FESpeed,
            FEMonCom[0].SNumByte / FESpeed);
        tfprintf(tfp_log, "rams%d raps%d rabs%d\n",
            FEMonCom[0].RNumMess / FESpeed,
            FEMonCom[0].RNumPack / FESpeed,
            FEMonCom[0].RNumByte / FESpeed);
#endif /* NET */

#ifdef MEM_PROF
        if (TierraLog && InstExe.m)
        {   tfprintf(tfp_log, "du%d ta%d",
                MemStat.DynMemUse, MemStat.TotNumAloc);
#if (defined __TURBOC__) && (!defined __GUI__)
            tfprintf(tfp_log, " co%u", coreleft());
#endif /* (defined __TURBOC__) && (!defined __GUI__) */
            tfprintf(tfp_log, "\n");
            tfprintf(tfp_log, "\n");
            MemStatReport();
        }
#endif /* MEM_PROF */

#ifndef AMIGA
#ifndef DECVAX
#ifndef _WIN32
        tfflush(tfp_log);
#endif /* _WIN32 */
#endif /* DECVAX */
#endif /* AMIGA */
#ifdef _WIN32
        ToggleLog(0, &tfp_log, TIERRALOGNAME, &tielogpos);
        ToggleLog(1, &tfp_log, TIERRALOGNAME, &tielogpos);
#endif /* _WIN32 */

    }
#ifdef BGL
    TBglPlan(tp);
#endif /* BGL */
}
#endif /* CLSTRSRVR */

/*
 * Log code:

 * ie = InstExeC
 * gn = Generation
 * ti = time

 * nc = NumCells
 * ng = NumGen
 * ns = NumSizes
 * sp = Speed

 * rm = RateMut
 * mm = RateMovMut
 * rf = RateFlaw
 * aa = AgeAvg

 * as = AvgSize
 * ap = AvgPop
 * fa = FecundAvg
 * re = RepInstEf

 * bn = BirthNum
 * bl = BirthLoca
 * bi = BirthInje
 * be = BirthEjec

 * dn = DeathNum
 * dy = DeathFecY
 * dz = DeathFecZ
 * de = DeathEjec

 * mp = MaxGenPop
 * mg = MaxGenMem 
 * dg = NumGenDG
 * gten = genotype entropy
 * szen = size class entropy

 * sim = SImmigMs = FEMonCom[EMIGRATETAG].SNumMess
 * sip = SImmigPk = FEMonCom[EMIGRATETAG].SNumPack
 * sib = SImmigBt = FEMonCom[EMIGRATETAG].SNumByte
 * nn  = NumNodes

 * spqm = SPingQMs = FEMonCom[TPINGREQTAG].SNumMess
 * spqp = SPingQPk = FEMonCom[TPINGREQTAG].SNumPack
 * spqb = SPingQBt = FEMonCom[TPINGREQTAG].SNumByte
 * es   = EjeToSelf

 * sppm = SPingPMs = FEMonCom[TPINGREPTAG].SNumMess
 * sppp = SPingPPk = FEMonCom[TPINGREPTAG].SNumPack
 * sppb = SPingPBt = FEMonCom[TPINGREPTAG].SNumByte
 * sams = SAllMs/S = FEMonCom[0].SNumMess / FESpeed

 * rim  = RImmigMs = FEMonCom[EMIGRATETAG].RNumMess
 * rpqm = RPingQMs = FEMonCom[TPINGREQTAG].RNumMess
 * rppm = RPingPMs = FEMonCom[TPINGREPTAG].RNumMess
 * saps = SAllPk/S = FEMonCom[0].SNumPack / FESpeed

 * sam  = SAllMesg = FEMonCom[0].SNumMess
 * sap  = SAllPack = FEMonCom[0].SNumPack
 * sab  = SAllByte = FEMonCom[0].SNumByte
 * sabs = SAllBt/S = FEMonCom[0].SNumByte / FESpeed

 * ram  = RAllMesg = FEMonCom[0].RNumMess
 * rap  = RAllPack = FEMonCom[0].RNumPack
 * rab  = RAllByte = FEMonCom[0].RNumByte
 * rabs = RAllBt/S = FEMonCom[0].RNumByte / FESpeed

 * du = DynMemUse
 * ta = TotNumAloc
 * co = coreleft
 */

#ifndef CLSTRSRVR
/*
 * ChangeGeneBnker - turn gene banker on or off
 *    gb - 0 - turn gene banker off and free gene bank
 *         1 - turn gene banker on and build gene bank
 *
 */
void ChangeGeneBnker(gb)
    I32s gb;
{   I32s ar, ci, flag;
    Pcells cp;

    if (gb)
    {   flag=(DiskBank)?25:0;
            flag = 0;
        tsprintf((char *)(&(Fbuf[0])),
            "Please wait while I create a Genebank ...\n");
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
        sl = MkGBFromSoup(&siz_sl, &sl, flag);
#ifdef ERRORTIE
        VerifyGB(1,1);
#endif /* ERRORTIE */
    }
    else
    {   FreeGB(&sl, &siz_sl);
        for (ar = 0; ar < NumCelAr; ar++)
            for (ci = 0; ci < CelArSiz; ci++)
            {   if (ar == 0 && ci < 2)
                    continue;
                cp = &cells[ar][ci];
                if (cp->ld)
                {   FreeThrdAnaDatCell(cp);
                    tsprintf((char *)(&((cp->d.
                        gen.label)[0])), "---");
                    tsprintf((char *)(&((cp->d.
                        parent.label)[0])), "---");
                    cp->d.gi = -1;
                    cp->d.origgi=-1;
                }
            }
    }
}

/**********************************************************************
 * DisplayMenu - display menus for curses or stdio frontends
 * input
 *    ncols - number of columns in menu
 *    menutitles - menu titles
 *    displaywidth - display width
 *    intrvwidth - column interval width
 *    optnowidth - option number width
 *    rmargine - right margin width
 *    promptofst - offset to prompt line
 **********************************************************************/
void DisplayMenu(ncols, menutitles, displaywidth,
    intrvwidth, optnowidth, rmargin, promptofst)

    int ncols, displaywidth, intrvwidth, optnowidth, rmargin;
    int *promptofst;
    char *menutitles[];

{   int titleidx, labelwidth, optno, titlecnt, colidx;
    int nrows, colwidth, linebufsub, rowidx;
    char *linebuf;


#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    linebuf=(char *)tcalloc(displaywidth + 1, sizeof(char), 2190);
    colwidth=((displaywidth - rmargin) - ((ncols - 1) *
            intrvwidth)) / ncols;
    labelwidth = colwidth - (optnowidth + 3);
    for (titlecnt = 0; menutitles[titlecnt]; titlecnt++);
    nrows = (titlecnt) ? ((titlecnt - 1) / ncols) + 1 : 0;
    for (rowidx = 0; rowidx < nrows; rowidx++)
    {   titleidx = rowidx;
        linebufsub = 0;
        for (colidx = 0; colidx < ncols; colidx++)
        {   if (titleidx < titlecnt)
            {   optno = titleidx + 1;
                tsprintf(&(linebuf[linebufsub]),
                    "%*d - %-*s", optnowidth, optno,
                    labelwidth, menutitles[titleidx]);
                titleidx += nrows;
                linebufsub += colwidth;
            }
        }
        FEPrintf(PLN_X, PLN_Y + rowidx, 0, 0, linebuf);
    }
    tsprintf((char *)(&(Fbuf[0])),"q - Quit");
    FEPrintf(PLN_X, PLN_Y + (rowidx++), 0, 0, &(Fbuf[0]));
    tsprintf((char *)(&(Fbuf[0]))," ");
    FEPrintf(PLN_X, PLN_Y + (rowidx++), 0, 0, &(Fbuf[0]));
    tsprintf((char *)(&(Fbuf[0])), "Menu Option->");
    FEPrintf(PLN_X, PLN_Y + rowidx, 0, 0, (char *)(&(Fbuf[0])));
    (*promptofst) = rowidx;
    tfree(linebuf, (displaywidth+1)*sizeof(char), 2190);
}

/*
 * GoHistoCtrl - call histogram paging/scrolling menu if histogram
 *               is in effect, otherwise print error message
 */
void GoHistoCtrl()
{   if(HistoInEffect())
        PageHist();
    else
    {   tsprintf((char *)(&(Fbuf[0])), "no histogram active");
        FEError(-504, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
}

/*
 * PageHist - histogram paging/scrolling menu
 *            scroll up/down one line
 *            move up/down one page
 */
void PageHist()
{   I8s answer;
    I32s maxhistostrt, loopflag;

    loopflag=1;
    while(loopflag)
    {   FEVidInit((I32s)0);
        tsprintf((char *)(&(Fbuf[0])),
            "U-pg up  D-pg dwn  u-scroll up  d-scroll dwn  q-quit->");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        answer=(I8s)FEGetch(HLP_X+strlen(
            (const char *)(&(Fbuf[0]))), HLP_Y);
        switch (answer)
        {   case 'U':
            histostrtidx += histodsplnct;
            break;
        case 'D':
            histostrtidx -= histodsplnct;
            break;
        case 'u':
            histostrtidx++;
            break;
        case 'd':
            histostrtidx--;
            break;
        case 'q':
            loopflag=0;
            break;
        default:
            break;
        }
        maxhistostrt = histoentcnt - histodsplnct;
        histostrtidx = (histostrtidx < 0) ?
            0 : ((histostrtidx > maxhistostrt) ?
            maxhistostrt : histostrtidx);
    }
}

/*
 * GoReprodEff - menu - select reproductive efficiency histograms for
 *                      genotype or size class
 */
void GoReprodEff()
{   I8s answer;

#if FRONTEND == BASIC
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    tsprintf((char *)(&(Fbuf[0])),
        "Reprod Effic | s-size_avg  g-genome->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    switch (answer)
    {   case 's':  /* size class avg reprod eff */
        histostrtidx = 0;
        GoSizRprdEff();
        break;
    case 'g':  /* genotype reprod eff */
        histostrtidx = 0;
        GoGenRprdEff();
        break;
    default:
        break;
    }
}

/*
 * GoSizRprdEff - for size class reproductive efficiency histogram,
 *                select :
 *                   1st or 2nd daughter
 *                   sort order - freq/size or size/freq
 *                open gene banker
 *                set size class reproductive efficiency histogram 
 *                   display mode
 * return : IMode = SIZ_EFF
 *          EffDaught - selected daughter
 *          HistSortOrder - selected sort order
 *             FREQ_SIZE
 *             SIZE_FREQ
 */
void GoSizRprdEff()
{   int abortflag;

    abortflag = 0;
    GoRprdEffDaught(&abortflag);
    if (!abortflag)
    {   SizeSortOrder(&abortflag, SIZ_EFF);
        if (!abortflag)
        {   if(!HistoInEffect())
               OpenGeneBanker();
            IMode = SIZ_EFF;
        }
    }
}

/*
 * GoRprdEffDaught - select 1st/2nd daughter for reproductive 
 *                   efficiency histogram
 * return : EffDaught - selected daughter
 */
void GoRprdEffDaught(abortflag)
    int *abortflag;
{   I8s answer;

    (*abortflag) = 0;
#if FRONTEND == BASIC
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    tsprintf((char *)(&(Fbuf[0])),
        "Reprod Effic | 1-1st_daught  2-2nd_daught->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))),HLP_Y);
    switch(answer)
    {   case '1':
        EffDaught = 0;
        break;
    case '2':
        EffDaught = 1;
        break;
    default:
        (*abortflag) = 1;
        break;
    }
}

/*
 * GoGenRprdEff - for genotype reproductive efficiency histogram,
 *                select :
 *                   1st or 2nd daughter
 *                   sort order - freq/size or size/freq
 *                open gene banker
 *                set genotype reproductive efficiency histogram display mode
 * return : IMode = SIZ_EFF
 *          EffDaught - selected daughter
 *          HistSortOrder - selected sort order
 *             FREQ_SIZE
 *             SIZE_FREQ
 */
void GoGenRprdEff()
{   int abortflag;

    abortflag = 0;
    GoRprdEffDaught(&abortflag);
    if (!abortflag)
    {   GenSortOrder(&abortflag, GEN_EFF);
        if (!abortflag)
        {   if(!HistoInEffect())
                OpenGeneBanker();
            IMode = GEN_EFF;
        }
    }
}

/*
 * SizeSortOrder - specify size type histogram sorting order
 * 
 * return : HistSortOrder - selected sort order
 *             FREQ_SIZE
 *             SIZE_FREQ
 */
void SizeSortOrder(abortflag, mode)
    int *abortflag, mode;
{   I8s answer;

    (*abortflag) = 0;
#if FRONTEND == BASIC
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    if (mode == SIZ_EFF)
        tsprintf((char *)(&(Fbuf[0])),
            "sort order | f-effic/size  s-size/effic ->");
    else
        tsprintf((char *)(&(Fbuf[0])),
            "sort order | f-freq/size  s-size/freq ->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    switch(answer)
    {   case 'f':  /* frequency/size */
        HistSortOrder = FREQ_SIZE;
        break;
    case 's':  /* size/frequency */
        HistSortOrder = SIZE_FREQ;
        break;
    default:
        (*abortflag) = 1;
        break;
    }
}

/*
 * GenSortOrder - specify genotype type histogram sorting order
 *
 * return : HistSortOrder - selected sort order
 *             FREQ_SIZE
 *             SIZE_FREQ
 */

/**********************************************************************
 * GenSortOrder - specify genome type histogram sorting order
 **********************************************************************/
void GenSortOrder(abortflag, mode)
    int *abortflag, mode;
{   I8s answer;

    (*abortflag) = 0;
#if FRONTEND == BASIC
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    if (mode == GEN_EFF)
        tsprintf((char *)(&(Fbuf[0])),
            "sort order | f-effic/gen  g-gen/effic ->");
    else
        tsprintf((char *)(&(Fbuf[0])),
            "sort order | f-freq/gen  g-gen/freq ->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    switch (answer)
    {   case 'f':  /* frequency/genotype */
        HistSortOrder = FREQ_GEN;
        break;
    case 'g':  /* genotype/frequency */
        HistSortOrder = GEN_FREQ;
        break;
    default:
        (*abortflag) = 1;
        break;
    }
}

/*
 * GoMiscMore - misc menu
 *              access to 
 *                 histogram paging/scrolling menu
 *                 Tierra node on/off line
 */
void GoMiscMore()
{   I8s  answer;

#if FRONTEND == BASIC
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    tsprintf((char *)(&(Fbuf[0])), "O-Histo Ctrl %s %s %s->",
#ifdef NET
   "o-online", "f-offline"
#else /* NET */
   "", ""
#endif /* NET */
#ifdef MEM_PROF
    ,"m-mem prof"
#else /* MEM_PROF */
    ,""
#endif /* MEM_PROF */
    );
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer=(I8s)FEGetch(HLP_X+strlen(
        (const char *)(&(Fbuf[0]))), HLP_Y);
    switch(answer)
    {   case 'O':                  /* Display Control */
            GoHistoCtrl();
            break;
#ifdef NET
        case 'o':
            GoOnLine();
            break;
        case 'f':
            GoOffLine();
            break;
#endif /* NET */
#ifdef MEM_PROF
        case 'm':
            if (tfp_log)
                MemStatReport();
            break;
#endif /* MEM_PROF */
        default:
            break;
    }
    answer = ' ';
}

#ifdef NET
/*
 * GoOnLine - place Tierra node online if not already online
 */
void GoOnLine()
{   if(!OnLineStat)
        TieCommOnOffLine(1, 0);
}

/*
 * GoOnLine - take Tierra node offline if not already offline
 */
void GoOffLine()
{   if(OnLineStat)
        TieCommOnOffLine(0, 0);
}

#endif /* NET */

/*
 * HistoInEffect - check for histogram in effect
 * return : 0 - histogram not in effect
 *          1 - histogram is in effect
 */
I32s HistoInEffect()
{   I32s histoflag;

    switch(IMode){
        case SIZ_HIST:
        case SIZM_HIST:
        case GEN_HIST:
        case SIZ_EFF:
        case GEN_EFF:
            histoflag=1;
            break;
        default:
            histoflag=0;
            break;
    }
    return histoflag;
}
#endif /* CLSTRSRVR */

/*
 * kbhit - return TRUE if key is hit, otherwise return FALSE
 *
 * description
 *
 * Returns:
 */
#ifdef AMIGA
#if FRONTEND == BASIC
int kbhit()
{   return ((getch() == ERR) ? FALSE : TRUE);
}
#endif /* FRONTEND == BASIC */
#endif /* AMIGA */

/*
 * SetPlanMSG_Y - set message area starting line
 *
 * return : MSG_Y - message area starting line
 */
void SetPlanMSG_Y()
{
    MSG_Y=PLN_Y
#ifdef TIERRA
        +((IMode!=PLN_STATS)?0:(6+((GeneBnker)?1:0)
#ifdef NET
        +7
#endif /* NET */
        ))
#endif /* TIERRA */
#ifdef CLSTRSRVR
        +5
#endif /* CLSTRSRVR */
#ifdef MEM_PROF
        +1
#endif /* MEM_PROF */
        ;
}

/*
 * FECeol - Tierra frontend interface - clear to end of given line
 *
 * n - number of line to clear
 */
void FECeol(n)
I32s n;
{
#if INTERFACE == BASICM
    GrafPtr oldPort;
    Rect destRect;
#endif /* INTERFACE == BASICM */
#if (INTERFACE == BASICB) || (INTERFACE == BASICD)
#ifndef __GUI__
        gotoxy(1, n+1);
        clreol();
#else /* _GUI__ */
        FEPrintf(20, n - 1, 0, 0, ENDBLANK);
        FEPrintf(0, n - 1, 0, 0, ENDBLANK);
#endif /* __GUI__ */
#endif /* (INTERFACE == BASICB) || (INTERFACE == BASICD) */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
    tmove(n, 0);
    tclrtoeol();
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)||\
    (INTERFACE == BASICV)
    trefresh();
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA)||
          (INTERFACE == BASICV) */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#if INTERFACE == BASICM
    GetPort(&oldPort);
    SetPort((GrafPtr) wp);
    destRect.left = displayOffset + 1;
    destRect.right = windowWidth;
    destRect.top = (n) * rowHeight;
    destRect.bottom = (n + 1) * rowHeight;
    EraseRect(&destRect);
    SetPort(oldPort);
#endif /* INTERFACE == BASICM */
}

#ifndef CLSTRSRVR
static void CalcEntropy(gtypentrp, szclsentrp)
double *gtypentrp;
double *szclsentrp;
{   I32s sizclsidx, szclspop, szclstot=0;
    double szclsfrq;

    I32s gnotypidx, gtyppop, gtyptot=0;
    double gtypfrq;

    double log2=log(2.0);
    SList *slist;
    GList *g;

    (*gtypentrp)=0.0;
    (*szclsentrp)=0.0;

    for(sizclsidx=0;sizclsidx<siz_sl;sizclsidx++)
        if((slist=sl[sizclsidx]))
            if((szclspop=sl[sizclsidx]->num_c))
            {   szclstot+=szclspop;
                for(gnotypidx=0;gnotypidx<slist->a_num;gnotypidx++)
                    if(!TNULL(g=slist->g[gnotypidx]))
                        if((gtyppop=g->pop))
                            gtyptot+=gtyppop;
            }
    if(szclstot)
        for(sizclsidx=0;sizclsidx<siz_sl;sizclsidx++)
            if((slist=sl[sizclsidx]))
                if((szclspop=sl[sizclsidx]->num_c))
                {   szclsfrq=((double)szclspop)/((double)szclstot);
                    (*szclsentrp)+=(szclsfrq*(log(szclsfrq)/log2));
                    for(gnotypidx=0;gnotypidx<slist->a_num;gnotypidx++)
                        if(!TNULL(g=slist->g[gnotypidx]))
                            if((gtyppop=g->pop))
                            {   gtypfrq=((double)gtyppop)/((double)gtyptot);
                                (*gtypentrp)+=gtypfrq*(log(gtypfrq)/log2); 
                            }
                }
    (*gtypentrp)*=(-1.0);
    (*szclsentrp)*=(-1.0);
    return;
}
#endif /* CLSTRSRVR */
