/* @(#)portable.c 1.13 1/13/95 */
/* portable.c 30-3-98 conditionally compiled functions for portability */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #12XX, in use: 1200 */
/* FEError labels: #12XX, in use: 1201-1215 */

#include "license.h"
#include "tierra.h"
#include "globals.h"
#ifdef TIEAUDIO
#include "tieaudsrv.h"
#endif /* TIEAUDIO */

#define MaxSizeT (((unsigned int) (~0)) - 1)
#ifdef SETENDIAN
static I32s SetEndianType P_((void));
#endif /* SETENDIAN */

#ifndef BGL_CLNT

#ifdef POSIX
#include <sys/utsname.h>
#endif /* POSIX */


#ifdef BGL
#ifdef TIERRA
#include "tbgl.h"
extern void BglError P_((I32s, I32s, I32s, I8s*));
#endif /* TIERRA */
#endif /* BGL */

#ifdef AMIGA
void Delay(long tmeout);
#endif /* AMIGA */

#ifdef NET
#ifdef _WIN32
static void revbufbytes P_((void *buf, void *revbuf, unsigned int buflen));
#endif /* _WIN32 */
#endif /* NET */

#if INTERFACE == BASICW

typedef struct
{   HANDLE hConsole;  /* console handle */
    DWORD fdwOldMode; /* console mode - for restore by endwin() */
} wndw_con;

struct _win_st {
    wndw_con incon;  /* input console  */
    wndw_con outcon; /* output console */
    /* console info - for restore by endwin() */
    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
};
#define WINDOW struct _win_st
WINDOW *stdscr;

#define ERR (0)
#define OK (1)

static HANDLE initscr_con(wndw_con *wndwcon, DWORD nStdHandle);
static void endwin_con(wndw_con *wndwcon);
static I32s clr_to_end(I32s clrlen);
static WINDOW *initscr(void);
static I32s clear(void);
static I32s clrtoeol(void);
static I32s clrtobot(void);
static I32s move(I32s y, I32s x);
static I32s standout(void);
static I32s standend(void);
static I32s mvaddch(I32s y, I32s x, I8s ch);
static I32s mvprintw(I32s y, I32s x, I8s *strbuf);
static I32s endwin(void);

#endif /* INTERFACE == BASICW */


#if defined(TIERRA)||defined(CLSTRSRVR)
#ifdef MEM_PROF

/*
 * MemStats - update memory profile statistics structure 
 *            due to allocate or free
 *
 * label - arbitrary label for this kind of allocation
 * af    - 1 = allocation, 0 = free
 * size  - size in bytes of this transaction (always positive)
 *
 * Returns:
 */
void MemStats(label, af, size)
    I32s label; /* arbitrary label for this kind of allocation */
    I32s af;    /* 1 = allocation, 0 = free */
    I32s size;  /* size in bytes of this transaction (always positive) */
{   I32s i;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    for (i = 0; i < MemStat.AlocSiz; i++)
        if (label == MemStat.mpa[i].label)
            break;
    if (i == MemStat.AlocSiz)
    {   if (!af)
        {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MemStats(): af zero for new label %d (free before alloc)",
                label);
            porterrmsg(1205,(char *)(&(Fbuf[0])),1);

        }
        MemStat.mpa = (MemProf *) realloc(MemStat.mpa,
            (MemStat.AlocSiz + 1) * sizeof(MemProf));

        MemStat.AlocSiz++;
        MemStat.TotNumAloc++;
        MemStat.DynMemUse += sizeof(MemProf);

        MemStat.mpa[0].num++;
        MemStat.mpa[0].siz += sizeof(MemProf);

        MemStat.mpa[i].label = label;
        MemStat.mpa[i].num = 1;
        MemStat.mpa[i].siz = size;
    }
    else
    {   MemStat.mpa[i].num += af ? 1 : -1;
        MemStat.mpa[i].siz += af ? size : -size;
    }
    MemStat.TotNumAloc += af ? 1 : -1;
    MemStat.DynMemUse += af ? size : -size;

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * InitMemProf - initialize memory profile statistics structure
 *
 * Returns:
 */
void InitMemProf()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    MemStat.mpa = (MemProf *) calloc(1, sizeof(MemProf));
    MemStat.mpa[0].label = 1200;
    MemStat.mpa[0].num = 1;
    MemStat.mpa[0].siz = MemStat.DynMemUse = sizeof(MemProf);
    MemStat.AlocSiz = MemStat.TotNumAloc = 1;

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * MemStatReport - list contents of memory profile statistics structure
 *                 to Tierra log file
 *
 * Returns:
 */
void MemStatReport()
{   I32s i, SumNum = 0, SumSiz = 0;

    tfprintf(tfp_log, "\n     label      number        size\n\n");
    for (i = 0; i < MemStat.AlocSiz; i++)
    {   tfprintf(tfp_log, "%10d  %10d  %10d\n",
            MemStat.mpa[i].label, MemStat.mpa[i].num, MemStat.mpa[i].siz);
        SumNum += MemStat.mpa[i].num;
        SumSiz += MemStat.mpa[i].siz;
    }
    tfprintf(tfp_log, "\n      Sums: %10d  %10d\n", SumNum, SumSiz);
    tfprintf(tfp_log, "TotNumAloc: %10d  %10d: DynMemUse\n\n",
        MemStat.TotNumAloc, MemStat.DynMemUse);
}

/*
 * FreeMemProf - free memory profile statistics structure
 *
 * Returns:
 */
void FreeMemProf()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    free(MemStat.mpa);
    memset(&MemStat, 0, sizeof(MemProfStat));

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

void MemStatCleanUp()
{   MemProf *mpa;
    I32s i;

#ifdef SIGBLOCK
     sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    mpa = (MemProf *) calloc(MemStat.AlocSiz, sizeof(MemProf));
    for (i = 0; i < MemStat.AlocSiz; i++)
        mpa[i] = MemStat.mpa[i];
    free(MemStat.mpa);
    MemStat.mpa = mpa;

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

#endif /* MEM_PROF */

#ifdef _WIN32
#if INTERFACE == BASICW
/*********************************/
/* Win32 curses static functions */
/*********************************/

/*
 * initscr_con - static function for implementation of initscr()
 *               curses function
 */
static HANDLE initscr_con(wndwcon, nStdHandle)
wndw_con *wndwcon;
DWORD  nStdHandle;
{   I32s errflag=0;

    if(((wndwcon->hConsole)=GetStdHandle(
        nStdHandle))!=INVALID_HANDLE_VALUE)
    {   if(GetConsoleMode(wndwcon->hConsole,
            &(wndwcon->fdwOldMode))!=TRUE)
            errflag=1;
        if(errflag)
            wndwcon->hConsole=INVALID_HANDLE_VALUE;
    }
    return wndwcon->hConsole;
}

/*********************************/
/* Win32 curses global functions */
/*********************************/

/* 
 * initscr - curses implementation for Win32 - see man ncurses(3X)
 *           called via tinitscr()
 */
WINDOW *initscr()
{   I32s errflag=0;

    if(stdscr=calloc(1, sizeof(WINDOW)))
    {   if(initscr_con(&(stdscr->incon),
            STD_INPUT_HANDLE)!=INVALID_HANDLE_VALUE)
            if(initscr_con(&(stdscr->outcon),
                STD_OUTPUT_HANDLE)!=INVALID_HANDLE_VALUE)
            {   if(GetConsoleScreenBufferInfo(
                    stdscr->outcon.hConsole,
                    &(stdscr->ConsoleScreenBufferInfo))==TRUE)
                {   if(clear()==OK)
                    {   LINES=(stdscr->ConsoleScreenBufferInfo.
                            srWindow.Bottom)-(stdscr->
                            ConsoleScreenBufferInfo.
                            srWindow.Top);
                        COLS=((stdscr->ConsoleScreenBufferInfo.
                            srWindow.Right)-(stdscr->
                            ConsoleScreenBufferInfo.
                            srWindow.Left))+1;
                    }
                    else
                        errflag=1;
                }
                else
                    errflag=1;
            }
            else
                errflag=1;
        else
            errflag=1;
        if(errflag)
        {   free(stdscr);
            stdscr=NULL;
        }
    }
    return stdscr;
}

/* 
 * clrtoeol - curses implementation for Win32 - see man ncurses(3X)
 *           called via tclrtoeol()
 */
I32s clrtoeol()
{   I32s rtncode=ERR;
    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;

    if(GetConsoleScreenBufferInfo(
        stdscr->outcon.hConsole,
        &ConsoleScreenBufferInfo)==TRUE)
        if(clr_to_end((COLS-
            (ConsoleScreenBufferInfo.dwCursorPosition.X))-
            (((ConsoleScreenBufferInfo.dwCursorPosition.Y)<
            (LINES-1))?0:1))==OK)
            if(move(ConsoleScreenBufferInfo.
                dwCursorPosition.Y, ConsoleScreenBufferInfo.
                dwCursorPosition.X)==OK)
                rtncode=OK;
    return rtncode;
}

/* 
 * clear - curses implementation for Win32 - see man ncurses(3X)
 *           called via tclear()
 */
I32s clear()
{   I32s rtncode=ERR;

    if(move(0,0)==OK)
        if(clrtobot()==OK)
            rtncode=OK;
    return rtncode;
}

/* 
 * move - curses implementation for Win32 - see man ncurses(3X)
 *        called via tmove()
 */
I32s move(y, x)
I32s y,x;
{   I32s rtncode=ERR;
    COORD dwCursorPosition;

    dwCursorPosition.X=(SHORT)x;
    dwCursorPosition.Y=(SHORT)y;
    if(SetConsoleCursorPosition(
        stdscr->outcon.hConsole,
        dwCursorPosition)==TRUE)
        rtncode=OK;
    return rtncode;
}

/* 
 * clrtobot - curses implementation for Win32 - see man ncurses(3X)
 *            called via tclrtobot()
 */
I32s clrtobot()
{   I32s rtncode=ERR;
    CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;

    if(GetConsoleScreenBufferInfo(
        stdscr->outcon.hConsole,
        &ConsoleScreenBufferInfo)==TRUE)
        if(clr_to_end(((COLS-
            (ConsoleScreenBufferInfo.dwCursorPosition.X))+
            ((LINES-((ConsoleScreenBufferInfo.
            dwCursorPosition.Y)+1))*COLS))-1)==OK)
            if(move(ConsoleScreenBufferInfo.dwCursorPosition.Y,
                ConsoleScreenBufferInfo.dwCursorPosition.X)==OK)
                rtncode=OK;
    return rtncode;
}

/* 
 * clr_to_end - curses implementation for Win32 - see man ncurses(3X)
 *              called via tclr_to_end()
 */
static I32s clr_to_end(clrlen)
I32s clrlen;
{   I8s *clrbuf;
    I32s rtncode=ERR;
    DWORD NumberOfCharsWritten;

    if(clrlen>0)
    {   if(clrbuf=malloc(clrlen))
        {   memset(clrbuf, ' ', clrlen);
            if(WriteConsole(stdscr->
                outcon.hConsole, clrbuf, clrlen,
                &NumberOfCharsWritten, NULL)==TRUE)
                rtncode=OK;
            free(clrbuf);
        }
    }
    else
        rtncode=OK;
    return rtncode;
}

/* 
 * standout - curses implementation for Win32 - see man ncurses(3X)
 *            called via tstandout()
 */
I32s standout()
{   I32s rtncode=ERR;

    if(SetConsoleTextAttribute(
        stdscr->outcon.hConsole,
        (WORD)(BACKGROUND_RED |
        BACKGROUND_GREEN | BACKGROUND_BLUE))==TRUE)
        rtncode=OK;
    return rtncode;
}

/* 
 * standend - curses implementation for Win32 - see man ncurses(3X)
 *            called via tstandend()
 */
I32s standend()
{   I32s rtncode=ERR;

    if(SetConsoleTextAttribute(
        stdscr->outcon.hConsole,
        (WORD)(FOREGROUND_RED |
        FOREGROUND_GREEN | FOREGROUND_BLUE))==TRUE)
        rtncode=OK;
    return rtncode;
}

/* 
 * mvaddch - curses implementation for Win32 - see man ncurses(3X)
 *           called via mvaddch()
 */
I32s mvaddch(y, x, ch)
I32s y,x;
I8s ch;
{   I32s rtncode=ERR;
    DWORD NumberOfCharsWritten;

    if(move(y,x)==OK)
        if(WriteConsole(stdscr->
            outcon.hConsole, &ch, sizeof(I8s),
            &NumberOfCharsWritten, NULL)==TRUE)
            rtncode=OK;
    return rtncode;
}

/* 
 * mvprintw - curses implementation for Win32 - see man ncurses(3X)
 *            called via mvprintw()
 */
I32s mvprintw(y,x, strbuf)
I32s y,x;
I8s *strbuf;
{   I32s rtncode=ERR;
    DWORD NumberOfCharsWritten;

    if(move(y,x)==OK)
        if(WriteConsole(stdscr->
            outcon.hConsole, strbuf, strlen(strbuf),
            &NumberOfCharsWritten, NULL)==TRUE)
            rtncode=OK;
    return rtncode;
}

/* 
 * printw - curses implementation for Win32 - see man ncurses(3X)
 *          called via printw()
 */
I32s printw(strbuf)
I8s *strbuf;
{   I32s rtncode=ERR;
    DWORD NumberOfCharsWritten;

    if(WriteConsole(stdscr->
        outcon.hConsole, strbuf, strlen(strbuf),
        &NumberOfCharsWritten, NULL)==TRUE)
        rtncode=OK;
    return rtncode;
}

/* 
 * endwin - curses implementation for Win32 - see man ncurses(3X)
 *          called via tendwin()
 */
I32s endwin()
{   if(stdscr)
    {
        /* Restore the original console mode and text colors. */
        SetConsoleMode(stdscr->incon.hConsole,
            stdscr->incon.fdwOldMode);
        SetConsoleMode(stdscr->outcon.hConsole,
            stdscr->outcon.fdwOldMode);
        SetConsoleTextAttribute(stdscr->outcon.hConsole,
            stdscr->ConsoleScreenBufferInfo.wAttributes);
        clear();
        free(stdscr);
        stdscr=NULL;
    }
    return OK;
}
#endif /* INTERFACE == BASICW */
#endif /* _WIN32 */

#if (INTERFACE == BASICC)||(INTERFACE == BASICW)||\
    (INTERFACE == BASICA)||(INTERFACE == BASICV)
/*****************************************************************************
 * tprintw - print text
 * strbuf - pointer to text
 * returns: from printw()
 *****************************************************************************/
I32s tprintw(strbuf)
I8s *strbuf;
{    I32s rtncode;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtncode=printw(strbuf);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtncode;
}

/*****************************************************************************
 * tmvprintw - position cursor and print text
 * y - line
 * x - column
 * strbuf - pointer to text
 * returns: from mvprintw()
 *****************************************************************************/
I32s tmvprintw(y,x, strbuf)
I32s y,x;
I8s *strbuf;
{    I32s rtncode;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtncode=mvprintw(y,x, strbuf);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtncode;
}

#if INTERFACE != BASICW

/*****************************************************************************
 * ttouchwin - signal protected interface to touchwin()
 *****************************************************************************/
void ttouchwin()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    touchwin(stdscr);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

#if ((INTERFACE == BASICC)&&(defined(TSysV)))||(INTERFACE == BASICA)
/*****************************************************************************
 * tnodelay - set for wait for 'enter/return' on keyboard input
 * bf - TRUE - wait, FALSE - no wait
 *****************************************************************************/
void tnodelay(I32s bf)
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    nodelay(stdscr,bf);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}
#endif /* ((INTERFACE == BASICC)&&(defined(TSysV)))||(INTERFACE == BASICA) */

/*****************************************************************************
 * tiegetstr - get string from curses window
 * str - pointer to buffer to hold string
 *****************************************************************************/
void tiegetstr(str)
I8s *str;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    getstr(str);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

#ifndef _WIN32
/*****************************************************************************
 * trefresh - signal protected interface to curses refresh()
 *****************************************************************************/
void trefresh()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    refresh();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}
#endif /* _WIN32 */

/*****************************************************************************
 * tmvgetch - position cursor and get character from curses window
 * y - line
 * x - column
 * returns: entered character
 *****************************************************************************/
I8s tmvgetch(y, x)
I32s y, x;
{   I8s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=mvgetch(y, x);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*****************************************************************************
 * tigetch - get character from curses window
 * returns: entered character
 *****************************************************************************/
I8s tigetch()
{   I8s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=getch();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}
#endif /* INTERFACE != BASICW */

/*****************************************************************************
 * tstandend - indicate display block highlighting end
 *****************************************************************************/
void tstandend()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    standend();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tclrtobot - clear screen from cursor position to bottom
 *****************************************************************************/
void tclrtobot()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    clrtobot();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tclrtoeol - clear display from cursor to end of line
 *****************************************************************************/
void tclrtoeol()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    clrtoeol();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tstandout - indicate display block highlighting start
 *****************************************************************************/
void tstandout()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    standout();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tmvaddch - position cursor and print character
 * y - display line
 * x - column
 * ch - character to be displayed
 *****************************************************************************/
void tmvaddch(y, x, ch)
int y, x;
int ch;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef _WIN32
    mvaddch(y, x, (I8s)ch);
#else /* _WIN32 */
    mvaddch(y, x, ch);
#endif /* _WIN32 */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tinitscr - signal protected curses interface to initscr()
 *****************************************************************************/
void tinitscr()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    initscr();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tendwin - signal protected interface to curses endwin()
 *****************************************************************************/
void tendwin()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    endwin();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tmove - position cursor on display
 * y - display line
 * x - column
 *****************************************************************************/
void tmove(y, x)
int y, x;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    move(y, x);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

/*****************************************************************************
 * tclear - clear entire curses display
 *****************************************************************************/
void tclear()
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    clear();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

}

#ifndef _WIN32
/*****************************************************************************
 * tcbreak - signal protected interface to curses cbreak()
 *****************************************************************************/
void tcbreak()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    cbreak();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tnocbreak - signal protected interface to curses nocbreak()
 *****************************************************************************/
void tnocbreak()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    nocbreak();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tcrmode - signal protected interface to curses crmode()
 *****************************************************************************/
void tcrmode()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    crmode();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tnocrmode - signal protected interface to nocrmode()
 *****************************************************************************/
void tnocrmode()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    nocrmode();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * techo - signal protected interface to curses echo()
 *****************************************************************************/
void techo()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    echo();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tnoecho - signal protected interface to curses noecho()
 *****************************************************************************/
void tnoecho()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    noecho();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tnl - signal protected interface to curses tnl()
 *****************************************************************************/
void tnl()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    nl();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*****************************************************************************
 * tnonl - signal protected interface to curses nonl()
 *****************************************************************************/
void tnonl()
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    nonl();

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}
#endif /* _WIN32 */

#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICW)||
          (INTERFACE == BASICA)||(INTERFACE == BASICV) */

#ifdef AMIGA
/*
 * sleep - sleep() function for Amiga
 *
 * delay - sleep time in seconds
 *
 * Returns:
 */
void sleep(unsigned int delay)
{   Delay((long) (delay * 50));
}

/*
 * _CXBRK - redefinition of ctrl-c handler for Amiga
 *
 * Returns:
 */
void __regargs _CXBRK(void)
{
#ifndef ARGTIE
    TC_Menu = 1;
#endif /* ARGTIE */
}

int __stat(file, statbuf)
const char *file;
struct stat *statbuf;
{   return stat(file, statbuf);
}
#endif /* AMIGA */

#ifdef __MWERKS__

#ifdef NET
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int getdomainname(char *name, int maxLength)
{   if (strlen(MacSite) > maxLength)
        return (-1);
    else
        strcpy(name, MacSite);
    return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
char *getenv(const char *variable)
{   char *envValue;

    if (!strcmp(variable, "HOST"))
    {   strcpy(envValue, MacHost);
        return envValue;
    }
    else
    {   return NULL;
    }
}

#endif /* NET */

#endif /* __MWERKS__ */

#ifdef POSIX
/* GDJ1: gethostname and getdomainname don't seem to be POSIX although
 * ISO/IEC 9445-1:1990 supposedly mentions them (according to the man page).
 * They are only given prototypes for the linux C library if 
 * __USE_BSD is defined
 * so here are POSIX compliant definitions
 */
int getdomainname(char *name, size_t len)
{   struct utsname buf;
    if (uname(&buf))
    {   perror("getdomainname");
        exit(-1);
    }
    strncpy(name, buf.domainname, len);
    return 0;
}
#endif /* POSIX */

#if defined(MSGCHKSUM)||(defined(PKTCHKSUM)&&(NETTYPE == UDPASM))

int getchksum(buf,buflen)
I8u *buf;
I32u buflen;
{   chksum sum,*trmptr;
    cksacc acc=0,bytrem;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    for(trmptr=(chksum *)buf,
        bytrem=buflen,sum=0,acc=0;
        bytrem>=sizeof(chksum);
        bytrem-=sizeof(chksum))
        acc+=ntohchksum(*(trmptr++));
    if(bytrem)
    {   memcpy(&sum,trmptr,bytrem);
        acc+=ntohchksum(sum);
    }
    acc=((~((~0)<<(sizeof(chksum)*BITSPERBYTE)))&acc)+
        (acc>>(sizeof(chksum)*BITSPERBYTE));
    acc+=(acc>>(sizeof(chksum)*BITSPERBYTE));
    sum=htonchksum((chksum)(~acc));

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return (schksum)sum;
}

void setchksum(buf,buflen)
I8u *buf;
I32u buflen;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    *((I32u *)buf)=htonl(0);
    *((chksum *)buf)=htonchksum(0);
    *((chksum *)buf)=getchksum(&(buf[
        sizeof(chksum)]),buflen-sizeof(chksum));

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}
#endif /* defined(MSGCHKSUM)||(defined(PKTCHKSUM)&&(NETTYPE == UDPASM)) */

void ToggleLog(logflag, loghandle, logname, logfilpsn)
I32s logflag, *logfilpsn;
FILE **loghandle;
I8s *logname;
{   I8s TierraLogFile[80];

    if(logflag)
    {   if(!(*loghandle))
        {   tsprintf((char *)(&(TierraLogFile[0])),
                "%s%s", GenebankPath, logname);
            if(new_soup)
            {   if(((*loghandle)=tfopen(&(
                    TierraLogFile[0]),(I8s *)"a")))
                    (*logfilpsn)=tftell(*loghandle);
            }
            else
            {   if(((*loghandle)=tfopen(
                    &(TierraLogFile[0]),(I8s *)"r+")))
                    tseek((*loghandle), (*logfilpsn), SEEK_SET);
                else
                {   (*loghandle)=tfopen(&(
                        TierraLogFile[0]),(I8s *)"w");
                    (*logfilpsn)=0;
                }
            }
            if(!(*loghandle))
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra ToggleLog() Problem opening - %s",
                    logname);
                porterrmsg(1206,(char *)(&(Fbuf[0])),1);
            }
        }
    }
    else
        if(*loghandle)
        {   (*logfilpsn)=tftell(*loghandle);
            tfclose(*loghandle);
            (*loghandle)=NULL;
        }
}

/*
 * TierraExit - clean up and exit Tierra
 *
 * status - Tierra exit status for exit() system call
 *
 */
void TierraExit(status)
int status;
{
#ifdef SIGBLOCK
    TieSignal(SIGIO, SIG_IGN);
#endif /* SIGBLOCK */
#ifdef NET
    TieCommOnOffLine(0, 0);
#ifndef CLSTRSRVR
#ifdef _WIN32
    HookUnInstall();
#endif /* _WIN32 */
#endif /* CLSTRSRVR */
#ifdef _WIN32
    WSACleanup();
#endif /* _WIN32 */
#endif /* NET */

#ifndef CLSTRSRVR
    if(GeneBnker)
        ChangeGeneBnker(0);
    FreeAMap();
    FreeDynMem();
#endif /* CLSTRSRVR */

#ifdef NET
    if(IPMapS)
        tfree(IPMapS, NumNodesS * sizeof(MapNode), 1000);
    IPMapS = NULL;
    if(IPMapC)
        tfree(IPMapC, NumNodesC * sizeof(MapNode), 1000);
    IPMapC = NULL;

    if (ping_fp)
    {   tfclose(ping_fp);
        ping_fp=NULL;
    }
#endif /* NET */

#ifdef BGL
    tfree(BglVarBuf,TBglVarNumMax*sizeof(BglVarSt),12310);
    tfree(BglMessageBuf,BGL_MAX_MES*sizeof(BglMessageSt),12311);
#endif /* BGL */

    if (tfp_log)
    {
#ifdef MEM_PROF
        MemStatReport();
        FreeMemProf();
#endif /* MEM_PROF */
        tfclose(tfp_log);
        tfp_log=NULL;
    }

#ifdef TIEAUDIO
#ifdef TIEAUDMP
    ClsTieAudDmp();
#endif /* TIEAUDMP */
    ClsTieAud();
#endif /* TIEAUDIO */

    exit(status);
}

/*
 * TieSleep - Tierra sleep function - allows interruption by keyboard,
 *            beagle, and Win32 activity monitor
 * starttime - sleep start time
 * sleepintrvl - length of sleep time in seconds
 */
void TieSleep(starttime, sleepintrvl)
#ifdef _WIN32
time_t (*starttime) P_((void));
#else /* _WIN32 */
#ifdef AMIGA
time_t (*starttime) P_((void));
#else /* AMIGA */
time_t (*starttime) P_((void));
#endif /* AMIGA */
#endif /* _WIN32 */
I32s sleepintrvl;
{   time_t strttme=0;
    I32s sleptflag;

#ifdef NET
#ifdef SIGBLOCK
    TieSignal(SIGIO, SIG_IGN);
#endif /* SIGBLOCK */
#endif /* NET */

    sleptflag=0;
    if(sleepintrvl)
    {   if(!starttime)
            strttme=tietime(NULL);

        while(!KEYHIT())
        {
#ifdef BGL
            if (MC_step == -1L)
                TBglRecvFunc(BGL_NONBLOCK);
            else
                break;
#endif /* BGL */
            if(starttime)
                strttme=(*starttime)();
            if(((tietime(NULL)-strttme)<sleepintrvl)&&
                (strttme>tiestrtme))
            {
#ifdef NET
#ifdef BGL
                MigrSleepMesg();
#endif /* BGL */
#endif /* NET */

#ifdef _WIN32
                Sleep(1000);
#else /* _WIN32 */
                sleep(1);
#endif /* _WIN32 */

                sleptflag=1;

            }
            else
                break;
        }
    }
#ifdef NET
    if((!sleepintrvl)||((sleepintrvl)&&(sleptflag)))
    {   rdhndlflsh(globtcibuf.tci_comrdhndl);
        DelMsgTree(&globtcibuf, &(globtcibuf.tci_msgbklog));

#ifndef CLSTRSRVR
        Apocalypse();
#endif /* CLSTRSRVR */
    }

#ifdef SIGBLOCK
    TieSignal(SIGIO, TieSigIO);
#endif /* SIGBLOCK */

#endif /* NET */
}

/*
 * initialize variables
 */
void initvar()
{
#ifdef NET
I32s i;
#endif /* NET */

#ifdef CLSTRSRVR
    clstrinitvar();
#endif /* CLSTRSRVR */

#ifdef TIERRA
    tieinitvar();
    CpuLoadLimitSleepNow=0;
#endif /* TIERRA */

    NumCells=SoupSize=InstExe.i=InstExe.m=0;
#ifdef SETENDIAN
    EndianType=SetEndianType();
#endif /* SETENDIAN */
    AgeAvg=0;
    tfp_log=NULL; /* file pointer for log */
    TC_Menu = 0;  /* flag unix sigint, to do a menu */

    FE_DV_Mode=0;

#ifdef NET
    ApocToggle=OutCapUpdTog=InstExeConnect.i=InstExeConnect.m=0;
    SubNetChgTime=ApocalypseNow=0;
    NumNodesC=0;  /* number of nodes in the local cluster */
    NumNodesS=0;  /* number of cluster servers in the net */
    OpSys=0;
    ping_fp=NULL;

    memset(&LocalClusterAddr, 0, sizeof(LocalClusterAddr));
    for(i=0;i<6;i++)
    {   MonCom[i].SNumMess=MonCom[i].SNumPack=MonCom[i].SNumByte=
        MonCom[i].RNumMess=MonCom[i].RNumPack=MonCom[i].RNumByte=0;
        FEMonCom[i].SNumMess=FEMonCom[i].SNumPack=FEMonCom[i].SNumByte=
        FEMonCom[i].RNumMess=FEMonCom[i].RNumPack=FEMonCom[i].RNumByte=0;
    }
#endif /* NET */
}

#ifdef SETENDIAN
/*
 * SetEndianType - determine processor integer byte order
 *
 * Returns : 0 = big endian (ex. Amiga, SGI, Sun)
 *           1 = little endian (ex Intel, DEC VAX, Alpha)
 */
static I32s SetEndianType()
{   int a=1;
    return (((char*)(&a))[0])?0:1;
}
#endif /* SETENDIAN */

/*
 * SubEvent - result=event1-event2
 *
 * event1 - pointer to Event structure
 * event2 - pointer to Event structure
 * result - pointer to Event structure
 *
 * Returns: *(result)=event1-event2
 *          also scalar of event1-event2
 */
I32s SubEvent(event1, event2, result)      /* subtract e2 from e1 */
    Event *event1, *event2, *result;
{   result->m = event1->m - event2->m;
    result->m += (event1->i - event2->i) / 1000000L;
    result->i = (event1->i - event2->i) % 1000000L;
    if (result->m <= 0)
        return result->i + (result->m * 1000000L);
    if (result->i < 0)
    {   --result->m;
        result->i += 1000000L;
    }
    return result->i + (result->m * 1000000L);
}

/*
 * AddEvent - result=event1+event2
 *
 * event1 - pointer to Event structure
 * event2 - pointer to Event structure
 * result - pointer to Event structure
 *
 * Returns: *(result)=event1+event2
 *          also scalar of event1+event2
 */
I32s AddEvent(event1, event2, result)      /* add e2 to e1 */
    Event *event1, *event2, *result;
{   result->m = event1->m + event2->m;
    result->m += (event1->i + event2->i) / 1000000L;
    result->i = (event1->i + event2->i) % 1000000L;
    return result->i + (result->m * 1000000L);
}

/*
 * tusleep - sleep for given interval
 * usec - sleep time in microseconds
 */
void tusleep(usec)
unsigned long usec;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
    usleep(usec);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * tsleep - sleep for given interval
 * sec - sleep time in seconds
 */
void tsleep(sec)
unsigned long sec;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
#ifdef _WIN32
                Sleep(sec*1000);
#else /* _WIN32 */
                sleep(sec);
#endif /* _WIN32 */
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

#ifdef _WIN32
void usleep(usec)
unsigned long usec;
{   struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=usec;
    select(0, 0, 0, 0, &tv);
}
#endif /* _WIN32 */

#ifdef NET
int tgetsockopt(s, level, optname, optval, optlen)
int s, level, optname;
char *optval;
int *optlen;
{   int rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef AMIGA
    rtnval=getsockopt(s, level, optname, optval, (long *)optlen);
#else /* AMIGA */
    rtnval=getsockopt(s, level, optname, optval, optlen);
#endif /* AMIGA */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}
/*
 * txdr_TMessage - encode/decode Tierra TMessage structure
 *
 * xdrs - XDR handle
 * objp - pointer to UDPASM Tierra message packet
 * Returns: return from xdr_TMessage()
 *
 */
bool_t txdr_TMessage(xdrs, objp)
XDR *xdrs;
TMessage *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_TMessage(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

#if NETTYPE == UDPASM
/*
 * txdr_datpkt - encode/decode UDPASM Tierra message packet
 *
 * xdrs - XDR handle
 * objp - pointer to UDPASM Tierra message packet
 * Returns: return from xdr_datpkt()
 *
 */
bool_t txdr_datpkt(xdrs, objp)
XDR *xdrs;
datpkt *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_datpkt(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_pkthdr - encode/decode Tierra pkthdr structure
 *
 * xdrs - XDR handle
 * objp - pointer to pkthdr structure
 * Returns: return from xdr_pkthdr()
 *
 */
bool_t txdr_pkthdr(xdrs, objp)
XDR *xdrs;
pkthdr *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_pkthdr(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}
#endif /* NETTYPE == UDPASM */
#endif /* NET */
#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

I8s *getdirnam(filnam)
I8s *filnam;
{   I8s *dirnam=NULL;
    I32s filnamlen=strlen((const char *)filnam),i;

    /* this malloc should be replaced with a signal protected
       allocation call using memory profiling if Tierra uses
       this function */
    if((dirnam=malloc(filnamlen+1)))
    {   if(filnamlen) 
        {   for(i=filnamlen-1;i>=0;i--)
                if(filnam[i]=='/')
                    break;
            if(i>0)
            {   strncpy((char *)dirnam,(const char *)filnam,i);
                dirnam[i]='\0';
            }
            else
            {   strncpy((char *)dirnam,(const char *)((i==0)?"/":""),1);
                dirnam[1]='\0';
            }
        }
        else
            dirnam[0]='\0';
    }
    else
        perror("getdirnam");
    return dirnam;
}

/*
 * txdr_head_t - encode/decode Tierra head_t structure
 *
 * xdrs - XDR handle
 * objp - pointer to head_t structure
 * Returns: return from xdr_head_t()
 *
 */
bool_t txdr_head_t(xdrs, objp)
XDR *xdrs;
head_t *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_head_t(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_GBindx_t - encode/decode Tierra GBindx_t structure
 *
 * xdrs - XDR handle
 * objp - pointer to GBindx_t structure
 * Returns: return from xdr_GBindx_t()
 *
 */
bool_t txdr_GBindx_t(xdrs, objp)
XDR *xdrs;
GBindx_t *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_GBindx_t(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_indx_t - encode/decode indx_t structure
 *
 * xdrs - XDR handle
 * objp - pointer to index_t structure
 * Returns: return from txdr_indx_t()
 *
 */
bool_t txdr_indx_t(xdrs, objp)
XDR *xdrs;
indx_t *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_indx_t(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_FilThrdAnaDat - encode/decode Tierra FilThrdAnaDat structure
 *
 * xdrs - XDR handle
 * objp - pointer to FilThrdAnaDat structure
 * Returns: return from xdr_FilThrdAnaDat()
 *
 */
void txdr_FilThrdAnaDat(xdrs, objp)
XDR *xdrs;
FilThrdAnaDat *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_FilThrdAnaDat(xdrs, objp))
        porterrmsg(1207,"txdr_FilThrdAnaDat failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_BaseCallLvlIntrv - encode/decode Tierra BaseCallLvlIntrv structure
 *
 * xdrs - XDR handle
 * objp - pointer to BaseCallLvlIntrv structure
 * Returns: return from xdr_BaseCallLvlIntrv()
 *
 */
void txdr_BaseCallLvlIntrv(xdrs, objp)
XDR *xdrs;
BaseCallLvlIntrv *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_BaseCallLvlIntrv(xdrs, objp))
        porterrmsg(1208,"txdr_BaseCallLvlIntrv failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_IdxThreadDat - encode/decode Tierra IdxThreadDat structure
 *
 * xdrs - XDR handle
 * objp - pointer to IdxThreadDat structure
 * Returns: return from xdr_IdxThreadDat()
 *
 */
void txdr_IdxThreadDat(xdrs, objp)
XDR *xdrs;
IdxThreadDat *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_IdxThreadDat(xdrs, objp))
        porterrmsg(1209,"txdr_FilThrdAnaDat failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_ThrdAnaEvent - encode/decode Tierra ThrdAnaEvent structure
 *
 * xdrs - XDR handle
 * objp - pointer to ThrdAnaEvent structure
 * Returns: return from xdr_ThrdAnaEvent()
 *
 */
void txdr_ThrdAnaEvent(xdrs, objp)
XDR *xdrs;
ThrdAnaEvent *objp;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_ThrdAnaEvent(xdrs, objp))
        porterrmsg(1210,"txdr_ThrdAnaEvent failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_SoupUpdEvent - encode/decode Tierra SoupUpdEvent structure
 *
 * xdrs - XDR handle
 * objp - pointer to SoupUpdEvent structure
 * Returns: return from xdr_SoupUpdEvent()
 *
 */
void txdr_SoupUpdEvent(xdrs, objp)
XDR *xdrs;
SoupUpdEvent *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_SoupUpdEvent(xdrs, objp))
        porterrmsg(1211,"txdr_SoupUpdEvent failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_FilGenThrdDat - encode/decode Tierra FilGenThrdDat structure
 *
 * xdrs - XDR handle
 * objp - pointer to FilGenThrdDat structure
 * Returns: return from xdr_FilGenThrdDat()
 *
 */
void txdr_FilGenThrdDat(xdrs, objp)
XDR *xdrs;
FilGenThrdDat *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_FilGenThrdDat(xdrs, objp))
        porterrmsg(1212,"txdr_FilGenThrdDat failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_FilInstrThrd - encode/decode Tierra FilInstrThrd structure
 *
 * xdrs - XDR handle
 * objp - pointer to FilInstrThrd structure
 *
 */
void txdr_FilInstrThrd(xdrs, objp)
XDR *xdrs;
FilInstrThrd *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_FilInstrThrd(xdrs, objp))
        porterrmsg(1213,"txdr_FilInstrThrd failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_GBGenome - encode/decode GBGenome structure
 *
 * xdrs - XDR handle
 * objp - pointer to GBGenome structure
 * Returns: return from txdr_GBGenome()
 *
 */
bool_t txdr_GBGenome(xdrs, objp)
XDR *xdrs;
GBGenome *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_GBGenome(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_GBGenBits - encode/decode GBGenBits structure
 *
 * xdrs - XDR handle
 * objp - pointer to GBGenBits structure
 * Returns: return from txdr_GBGenBits()
 *
 */
bool_t txdr_GBGenBits(xdrs, objp)
XDR *xdrs;
GBGenBits *objp;
{   bool_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    rtnval=xdr_GBGenBits(xdrs, objp);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(CLSTRSRVR)||\
    defined(SOUPUPDTRC)
/*
 * ByteXdrWrdSize - convert length in bytes to length in xdrwrd structures
 *
 * bytelen - length in bytes
 */
I32s ByteXdrWrdSize(bytelen)
I32s bytelen;
{   return(((bytelen-1)/sizeof(xdrwrd))+1);
}
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(CLSTRSRVR)||
          defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(PROBE)

/*
 * Lbl2Int - convert genotype label to gene bank index
 *
 *  s - genotype label
 *
 * returns the gene bank index integer
 *
 */
I32s Lbl2Int(s)
I8s *s;
{   I32s index;
#ifdef BIGNAMES
I32s i;
#endif /* BIGNAMES */
#ifdef BIGNAMES
    if (s[0]=='-')
        index=-1;
    else
        for(index=i=0;i<3;i++)
            index=(index*52)+((s[i]>='a')?(s[i]-'a'):(26+s[i]-'A'));
#else /* BIGNAMES */
    index=(s[0]=='-')?-1:((s[2]-'a')+(26*(s[1]-'a'))+(676*(s[0]-'a')));
#endif /* BIGNAMES */
    return index;
}

/*
 * Int2Lbl - convert gene bank index to genotype label
 *
 *  i - gene bank index
 *
 * returns the genotype label in a 4 character static array
 *
 */
I8s *Int2Lbl(i)
    I32s i;
{   static I8s s[4];

#ifdef BIGNAMES
    I32s index;

    if (i < 0)
    {   strcpy(s, "---");
        return s;
    }
    index = i / (52 * 52);
    if (index >= 26)
        s[0] = 'A' + index - 26;
    else
        s[0] = 'a' + index;
    i %= (52 * 52);
    index = i / 52;
    if (index >= 26)
        s[1] = 'A' + index - 26;
    else
        s[1] = 'a' + index;
    i %= 52;
    index = i;
    if (index >= 26)
        s[2] = 'A' + index - 26;
    else
        s[2] = 'a' + index;
    s[3] = 0;
#else /* BIGNAMES */
    if(i<0)
        strcpy((char *)s, (const char *)"---");
    else
    {   s[0]='a'+((I32s)(i/676));
        i%=676;
        s[1]='a'+((I32s)(i/26));
        i%=26;
        s[2]='a'+(I32s)i;
        s[3]=0;
    }
#endif /* BIGNAMES */
    return s;
}
#endif /* defined(TIERRA)||defined(PROBE) */
#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR)

#ifdef SIGBLOCK
/*
 * SetSigSet - construct global composite signal mask from SIGIO and SIGINT
 *
 * Returns: none
 */
void SetSigSet()
{   sigemptyset(&SigSet);
    sigaddset(&SigSet,SIGIO);
    sigaddset(&SigSet,SIGINT);
}

/*
 * TSigBlock - add signals to current set of blocked signals
 *
 * value - signals to be added
 *
 * Returns: previous set of blocked signals
 */
sigset_t TSigBlock(value)
sigset_t value;
{   sigset_t oset;
    while(1)
        if(!sigprocmask(SIG_BLOCK, &value, &oset))
            break;
    return oset;
}

/*
 * TSigRelease - remove signals from current set of blocked signals
 *
 * value - signals to be removed
 *
 * Returns: none
 */
void TSigRelease(value)
sigset_t value;
{   while(1)
        if(!sigprocmask(SIG_SETMASK, &value, NULL))
            break;
}

/*
 * TSigQuery - query current set of blocked signals
 *
 * Returns: previous set of blocked signals
 */
sigset_t TSigQuery()
{   sigset_t oset,value;
    memset(&value,0,sizeof(value));
    while(1)
        if(!sigprocmask(SIG_BLOCK, &value, &oset))
            break;
    return oset;
}

void SetAsyncSkt(sock)
int sock;
{   I32s sktflg;
    errno=0;
    if((sktflg=tfcntl(sock, F_GETFL, 0))>=0)
        if(tfcntl(sock, F_SETFL,sktflg|FASYNC)>=0)
            tfcntl(sock, F_SETOWN, getpid());
    if(errno)
        porterrmsg(1215,NULL,1);
}

/*
 * TieSignal - associate handler with given signal
 *
 * sigval - signal number
 * sighndl - pointer to signal handler
 *
 * Returns:
 */
void TieSignal(sigval, sighndl)
I32s sigval;
#ifdef unix
#ifndef TSysV
void (*sighndl)();
#else /* TSysV */
void (*sighndl)(int);
#endif /* TSysV */
#else /* unix */
#if (OPSYS == DECVMS)
void(*sighndl)(int);
#endif /* (OPSYS == DECVMS) */
#endif /* unix */
{
#ifdef unix
    struct sigaction act;
#endif

#ifdef unix
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler=sighndl;
    sigaction(sigval, &act, NULL);
#else /* unix */
#if (OPSYS == DECVMS)
    signal(sigval, sighndl);
#endif /* (OPSYS == DECVMS) */
#endif /* unix */
}
#ifdef unix
#ifdef QRYSIGNALHNDLR
__sighandler_t QrySignalHndlr(sigval)
I32s sigval;
{   struct sigaction oldact;
    sigaction(sigval,NULL,&oldact);
    return oldact.sa_handler;
}
#endif /* QRYSIGNALHNDLR */
#endif /* unix */
#endif /* SIGBLOCK */
#endif /* defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR)
#if defined(NET)||defined(BGL)

/*
 * tgethostbyaddr - get hostent structure given host ip address
 *
 * addr - pointer to host address buffer
 * len - size of host address buffer
 * type- host address type
 *
 */
struct hostent *tgethostbyaddr(addr, len, type)
I8s *addr;
I32s len, type;
{   struct hostent *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=gethostbyaddr(addr, (int)len, (int)type);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tsendto - send message using given socket to given address
 * s - socket descriptor
 * msg - pointer to message buffer
 * len - size of message buffer
 * flags - see sendto(2)
 * Returns: return from sendto()
 * to - structure containing destination address
 * tolen - size of destination address buffer
 * Returns: return fron sendto()
 */
I32s tsendto(s, msg, len, flags, to, tolen)
I32s s, len, flags, tolen;
I8s *msg;
struct sockaddr *to;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef _WIN32
   int wsaerrtn;
#endif /* _WIN32 */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    for(;;)
    {   if((rtnval=sendto(s, msg, len, flags, to, tolen))<0)
        {
#ifdef _WIN32
            wsaerrtn=WSAGetLastError();
            if(!((wsaerrtn==WSAECONNREFUSED)||(wsaerrtn==WSAENOBUFS)))
#else /* _WIN32 */
            if(!((errno==ECONNREFUSED)||(errno==ENOBUFS)))
#endif /* _WIN32 */
               break;
        }
        else
           break;
    }

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * trecv - receive message using given socket
 * s - socket descriptor
 * buf - buffer to hold incoming message
 * len - size of incoming message buffer
 * flags - see recv(2)
 * Returns: return from recv()
 */
I32s trecv(s, buf, len, flags)
I32s s, len, flags;
I8s *buf;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=recv(s, buf, len, flags);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * trecvfrom - receive message using given socket
 * s - socket descriptor
 * buf - buffer to hold incoming message
 * len - size of incoming message buffer
 * flags - see recv(2)
 * Returns: return from recvfrom()
 */
int trecvfrom(s, buf, len, flags, from, fromlen)
I32s s, len, flags;
I8s *buf;
struct sockaddr *from;
I32s *fromlen;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=recvfrom(s, buf, len, flags, from, fromlen);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tconnect - connect given local socket to specified remote address
 * s - socket descriptor
 * name - pointer to remote address buffer
 * namelen - size of remote address buffer
 * remote flag - 1 - use long timeouts
 *               0 - use short timeouts
 * Returns: return from connect()
 */
I32s tconnect(s, name, namelen, remoteflag)
I32s s, namelen, remoteflag;
struct sockaddr *name;
{   I32s rtnval, cnctrtn;
    int cncterrno;

#ifdef NET
#if NETTYPE == TCP
#ifndef BGL_CLNT
    fd_set wrtfds, rdfds;
    struct timeval tmeout;
#endif /* BGL_CLNT */
#endif /* NETTYPE == TCP */
#endif /* NET */

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=0;

#ifdef NET
#if NETTYPE == TCP
#ifndef BGL_CLNT
    rtnval=SockBlkCtl(s, 0);
#endif /* BGL_CLNT */
#endif /* NETTYPE == TCP */
#endif /* NET */

    if(rtnval>=0)
    {   cnctrtn=connect(s, name, namelen);
#ifdef _WIN32
        cncterrno=WSAGetLastError();
#else /* _WIN32 */
        cncterrno=errno;
#endif /* _WIN32 */

#if (defined(NET)&&(NETTYPE == TCP))||defined(BGL_CLNT)
#ifndef BGL_CLNT
        if((rtnval=SockBlkCtl(s, 1))>=0)
        {
#endif /* BGL_CLNT */
            if(cnctrtn<0)
#ifdef _WIN32
                if(cncterrno==WSAEINPROGRESS)
#else /* _WIN32 */
                if(cncterrno==EINPROGRESS)
#endif /* WIN32 */
                {
#ifndef BGL_CLNT
                    FD_ZERO(&rdfds);
                    FD_ZERO(&wrtfds);
                    FD_SET(s, &rdfds);
                    FD_SET(s, &wrtfds);
                    if(remoteflag)
                    {   tmeout.tv_sec=TCPRemSelTmeSec;
                        tmeout.tv_usec=TCPRemSelTmeUSec;
                    }
                    else
                    {   tmeout.tv_sec=TCPLocSelTmeSec;
                        tmeout.tv_usec=TCPLocSelTmeUSec;
                    }
                    tselect(Nfds, &rdfds, &wrtfds, 0, &tmeout);
                    if(FD_ISSET(s,&wrtfds))
                        if(FD_ISSET(s,&rdfds))
                            rtnval=-1;
                        else
                           rtnval=0;
                    else
                        rtnval=0;
#else /* BGL_CLNT */
                    rtnval=cnctrtn;
#endif /* BGL_CLNT */
                }
                else
                    rtnval=cnctrtn;
            else
                rtnval=cnctrtn;
#ifndef BGL_CLNT
        }
#endif /* BGL_CLNT */
#endif /* (defined(NET)&&(NETTYPE == TCP))||defined(BGL_CLNT) */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

struct servent *tgetservbyname(name, proto)
I8s *name, *proto;
{   struct servent *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=getservbyname((const char *)name, (const char *)proto);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}
#endif /* defined(NET)||defined(BGL) */

#if defined(NET)||defined(BGL)||defined(TIEAUDIO)

/*
 * tgetprotobyname - get protoent structure by protocol name
 *
 * name - pointer to protocol name
 * Returns: return from getprotobyname()
 *             (pointer to protoent structure)
 */
struct protoent *tgetprotobyname(name)
I8s *name;
{   struct protoent *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=getprotobyname((const char *)name);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tsocket - create socket
 *
 * domain - normally AF_INET, see socket(2)
 * type - socket type (ex SOCK_STREAM, SOCK_DGRAM)
 * protocol - socket protocol
 *
 * Returns: return from socket()
 *
 */
I32s tsocket(domain, type, protocol)
I32s domain, type, protocol;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=socket(domain, type, protocol);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tbind - link socket to ip address
 * 
 * s - socket descriptor
 * name - structure containing address to be linked to socket
 * namelen - size of address structure
 *
 * Returns: return from bind()
 *
 */
I32s tbind(s, name, namelen)
I32s s, namelen;
struct sockaddr *name;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=bind(s, name, namelen);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tlisten - link socket to ip address
 * 
 * s - socket descriptor
 * bklog - size of listen backlog
 *
 * Returns: return from listen()
 *
 */
I32s tlisten(s, bklog)
I32s s, bklog;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=listen(s,bklog);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * taccept - accept incoming connection
 * s - socket descriptor
 * addr - source of incoming connection
 * addrlen - length of addr buffer
 * Returns: return from accept()
 */
I32s taccept(s, addr, addrlen)
I32s s;
struct sockaddr *addr;
I32s *addrlen;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=accept(s, addr, addrlen);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * tclosesocket - close socket
 *
 * fd - socket descriptor
 * Returns: return from close() or closesocket() (Win32)
 */
I32s tclosesocket(fd)
I32s fd;
{   I32s rtnval;

#ifdef _WIN32
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
#endif /* _WIN32 */

#ifdef _WIN32

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

      rtnval=closesocket(fd);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#else /* _WIN32 */
      rtnval=tclose(fd);
#endif /* _WIN32 */

   return rtnval;
}

/*
 * tsend - send message using given socket
 * s - socket descriptor
 * msg - pointer to message buffer
 * len - size of message buffer
 * flags - see send(2)
 * Returns: return from send()
 */
I32s tsend(s, msg, len, flags)
I32s s, len, flags;
I8s *msg;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef _WIN32
   int wsaerrtn;
#endif /* _WIN32 */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    for(;;)
    {   if((rtnval=send(s, msg, len, flags))<0)
        {
#ifdef _WIN32
            wsaerrtn=WSAGetLastError();
            if(!((wsaerrtn==WSAECONNREFUSED)||(wsaerrtn==WSAENOBUFS)))
#else /* _WIN32 */
            if(!((errno==ECONNREFUSED)||(errno==ENOBUFS)))
#endif /* _WIN32 */
               break;
        }
        else
           break;
    }

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tselect - select socket for I/O ready
 *
 * width - number of bits in select mask (1/socket to be checked)
 * readfds - fd_set select mask for "read" sockets
 * writefds - fd_set select mask for "read" sockets
 * exceptfds - fd_set select mask for "read" sockets
 * tmeout - timeval structure holding maximum wait for ready socket
 *
 * Returns: return from select()
 */
I32s tselect(width, readfds, writefds, exceptfds, tmeout)
I32s width;
fd_set *readfds, *writefds, *exceptfds;
struct timeval *tmeout;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=select(width, readfds, writefds, exceptfds, tmeout);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * tsetsockopt - setsockopt socket for I/O ready
 *
 */
I32s tsetsockopt(s,level,optname,optval,optlen)
int s,level,optname;
void *optval;
socklen_t optlen;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=setsockopt(s,level,optname,optval,optlen);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}
/*
 * tgethostbyname - get ip address using host name
 *
 * name - pointer to host name string
 *
 * Returns: return from gethostbyname()
 *
 */
struct hostent *tgethostbyname(name)
I8s *name;
{   struct hostent *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=gethostbyname((const char *)name);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tgethostname - get name of local host
 *
 * name - buffer to hold host name
 * namelen - size of host name buffer
 *
 * Returns:  0 - successful
 *          -1 - failure
 *
 */
I32s tgethostname(name, namelen)
I8s *name;
I32s namelen;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#if defined(AMIGA) || defined (__linux__)||defined(__mips__)
    I8s *hostnamenv;
#endif /* defined(AMIGA) || defined (__linux__)||defined(__mips__) */

#ifdef POSIX
    struct utsname buf;
#endif /* POSIX */

   rtnval=0;

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef POSIX
    if(uname(&buf))  
        rtnval=-1;
    else
        strncpy(name, buf.nodename, namelen);
#else /* POSIX */

#if defined(AMIGA)||defined(__linux__)||defined(__mips__)
    if((hostnamenv=getenv("HOSTNAME")))
        strcpy(name,hostnamenv);
    else
        rtnval=-1;
#else /* defined(AMIGA) || defined (__linux__)||defined(__mips__) */
    if(!(rtnval=gethostname((char *)name, namelen)))
    {   if(!gethostbyname((const char *)name))
            if(gethostbyname((const char *)"localhost"))
                tsprintf((char *)name,"%s","localhost");
            else
                rtnval=-1;
    }
    else
        rtnval=-1;
#endif /* defined(AMIGA) || defined (__linux__)||defined(__mips__) */
#endif /* POSIX */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}
#endif /* defined(NET)||defined(BGL)||defined(TIEAUDIO) */

#ifdef AMIGA
/*
 * tgettimeofday - set current universal time (GMT) in timeval structure
 *               - Amiga
 *
 * time     - pointer to timeval structure to contain current universal time
 * timezone - not used
 *
 * Returns: 0
 */
I32s tgettimeofday(struct timeval *time, struct timezone *zone)
{   unsigned int clock[2];
    int local2gmt, gmtofst, tgmtofst, scanfrtn;
    char *tzone;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    gmtofst=0;
    if(tzone=getenv("TZ"))
    {

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        scanfrtn=sscanf(tzone,"%*c%*c%*c%d", &tgmtofst);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if(scanfrtn==1)
            gmtofst=tgmtofst;
    }
    local2gmt=((((8*365)+8/4)*24)+gmtofst)*60*60;

    timer(&clock[0]);
    clock[0]+=local2gmt;

    time->tv_sec = (long)clock[0];
    time->tv_usec = (long)clock[1];

    if(tzone){
        free(tzone);
    }
    return 0;
}
#endif /* AMIGA */

#ifdef __MWERKS__
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s tgettimeofday(struct timeval *time, struct timezone *zone)
{   unsigned long *seconds;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
    GetDateTime(seconds);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    time->tv_sec = (long) seconds;
    time->tv_usec = 0L;
}
#endif /* __MWERKS__ */

#ifdef _WIN32
/*
 * tgettimeofday - set current universal time (GMT) in timeval structure
 *                 (currently returns local time instead)
 *
 * time     - pointer to timeval structure to contain current universal time
 * timezone - not used
 *
 * Returns: 0
 */
I32s tgettimeofday(struct timeval *timearg, struct timezone *zone)
{   timearg->tv_sec=(long)tietime((time_t *)0);
    timearg->tv_usec=0L;
    return 1;
}
#endif /* _WIN32 */

#ifdef DJGPP
/*
 * tgettimeofday - set current universal time (GMT) in timeval structure
 *                 (currently returns local time instead)
 *
 * time     - pointer to timeval structure to contain current universal time
 * timezone - not used
 *
 * Returns: 0
 */
I32s tgettimeofday(struct timeval *timearg, struct timezone *zone)
{   timearg->tv_sec=(long)tietime((time_t *)0);
    timearg->tv_usec=0L;
    return 1;
}
#endif /* DJGPP */

#ifdef unix
I32s tgettimeofday(tp, tzp)
struct timeval *tp;
struct timezone *tzp;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=gettimeofday(tp, tzp);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * tfcntl - manipulate file descriptor
 *
 * fd - file descriptor
 * cmd - command according to fcntl(2)
 * arg - command data if any according to fcntl(2)
 *
 */
I32s tfcntl(fd, cmd, arg)
I32s fd, cmd, arg;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=fcntl(fd, cmd, arg);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}
#endif /* unix */
#endif /* defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(CLSTRSRVR)||defined(SOUPUPDTRC)

#ifdef HAVE_STDARG_H
I32s tsprintf(char *buf,char *fmt, ...)
#else /* HAVE_STDARG_H */
#ifdef _WIN32
I32s tsprintf(char *buf,char *fmt,...)
#else /* _WIN32 */
#ifdef AMIGA
I32s tsprintf(char *buf,char *fmt, ...)
#else /* AMIGA */
I32s tsprintf(va_alist)
va_dcl
#endif /* AMIGA */
#endif /* _WIN32 */
#endif /* HAVE_STDARG_H */
{   va_list args;
    I32s rtnval;
#ifndef HAVE_STDARG_H
#ifndef _WIN32
#ifndef AMIGA
    I8s *fmt, *buf;
#endif /* AMIGA */
#endif /* WIN32 */
#endif /* HAVE_STDARG_H */

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef HAVE_STDARG_H
    va_start(args, fmt);
#else /* HAVE_STDARG_H */
#ifdef _WIN32
    va_start(args, fmt);
#else /* _WIN32 */
#ifdef AMIGA
    va_start(args, fmt);
#else /* AMIGA */
    va_start(args);
#endif /* AMIGA */
#endif /* _WIN32 */
#endif /* HAVE_STDARG_H */

#ifndef _WIN32
#ifndef AMIGA
#ifndef HAVE_STDARG_H
    buf=va_arg(args, char *);
    fmt=va_arg(args, char *);
#endif /* HAVE_STDARG_H */
#endif /* AMIGA */
#endif /* _WIN32 */

    rtnval=vsprintf((char *)buf, (const char *)fmt, args);
    rtnval=strlen((const char *)buf);
    va_end(args);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * tcalloc - calloc for arrays not greater than 64K
 *
 * num  - number of elements to be allocated
 * siz  - single element size in bytes
 * label - MemProf label
 *
 * Returns:
 * tp - pointer to new buffer.
 */
I8s Fp tcalloc(num, siz, label)
    I32u num;
    I32u siz;
    I32s label;
{   I8s Fp tp;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    tp = (I8s Fp) calloc(num, siz);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    if (!tp)
    {   tsprintf((char *)(&(Fbuf[0])),
    "tcalloc() error: calloc failed, num: %d siz: %d label: %d ",
            num, siz, label);
        porterrmsg(1201,(char *)(&(Fbuf[0])),1);
    }

#ifdef MEM_PROF
    MemStats(label, 1, num * siz);
#endif /* MEM_PROF */
    return tp;
}

/*
 * tfgets - get line from file
 * 
 * s - pointer to line buffer
 * n - size of line buffer 
 * filehndl - pointer to FILE structure forfile being read from
 *
 * Returns: pointer to line buffer
 *
 */
I8s *tfgets(s, n, filhndl)
I8s *s;
I32s n;
FILE *filhndl;
{   I8s *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=(I8s *)fgets((char *)s, n, filhndl);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tfclose - close file
 *
 * filhndl - pointer to FILE structure
 *
 */
I32s tfclose(filhndl)
FILE *filhndl;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=fclose(filhndl);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tfree - free block of 64k or less
 *
 * ptr - block pointer
 * siz - block size
 * label - block MEM_PROF label
 *
 * Returns: none
 */
/* free for arrays not greater than 64K */
void tfree(ptr, siz, label)
    void *ptr;
    I32s siz;
    I32s label;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    free(ptr);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef MEM_PROF
    MemStats(label, 0, siz);
#endif /* MEM_PROF */
}

/*
 * thcalloc - calloc for arrays of greater than 64K
 *
 * num  - number of elements to be allocated
 * siz  - single element size in bytes
 * label - MemProf label
 *
 * Returns:
 * hp - pointer to new buffer.
 */
I8s Hp thcalloc(num, siz, label)
    I32u num;
    I32u siz;
    I32s label;
{   I8s Hp hp;
#ifdef SIGBLOCK
     sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    hp = (I8s Hp) farcalloc(num, siz);
#else /* __TURBOC__ */
    hp = (I8s *) calloc(num, siz);
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    if (!hp)
    {   tsprintf((char *)(&(Fbuf[0])),
    "thcalloc() error: calloc failed, num: %d siz: %d label: %d ",
            num, siz, label);
        porterrmsg(1202,(char *)(&(Fbuf[0])),1);
    }

#ifdef MEM_PROF
    MemStats(label, 1, num*siz);
#endif /* MEM_PROF */
    return hp;
}

/*
 * tfread - read array of elements from file
 *
 * ptr - array buffer to hold elements read
 * size - single element size
 * n - number of elements to read
 * stream - pointer to FILE structure
 *
 * Returns:
 * r_size - bytes read
 */
I32s tfread(ptr, size, n, stream)
    I8s *ptr;
    I32s size, n;
    FILE *stream;
{   I32s r_size = 0;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    I32s segs, rem, i = 0;
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    segs = (I32s) ((unsigned int) n / MaxSizeT);
    rem = (I32s) ((unsigned int) n % MaxSizeT);
    if (segs)
        for (i = 0; i < segs; i++)
            r_size += fread((I8s *) ((I8s Hp) ptr + ((unsigned int) i *
                (unsigned int) size * MaxSizeT)), (size_t) size,
                (size_t) MaxSizeT,
                stream);
    if (rem)
        r_size += fread((I8s *) ((I8s Hp) ptr + ((unsigned int) segs *
            (unsigned int) size * MaxSizeT)), (size_t) size, (size_t) rem,
            stream);
#else  /* __TURBOC__ */
    r_size = fread(ptr, size, n, stream);
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    if (r_size != n)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra tfread() inconsistency: n = %d  r_size = %d",
            n, r_size);
        porterrmsg(1203,(char *)(&(Fbuf[0])),1);
    }
    return r_size;
}

/*
 * tfwrite - write array of elements to file
 *
 * ptr - array to be written
 * size - single element size
 * n - number of elements to write
 * stream - pointer to FILE structure
 *
 * Returns:
 * r_size - bytes written
 */
I32s tfwrite(ptr, size, n, stream)
    I8s *ptr;
    I32s size, n;
    FILE *stream;
{   I32s r_size = 0;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
#ifdef __TURBOC__
    I32s segs, rem, i = 0;
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    segs = (I32s) ((unsigned int) n / MaxSizeT);
    rem = (I32s) ((unsigned int) n % MaxSizeT);
    if (segs)
        for (i = 0; i < segs; i++)
            r_size += fwrite((const I8s *) ((I8s Hp) ptr + ((unsigned int) i *
                (unsigned int) size * MaxSizeT)), (size_t) size,
                (size_t) MaxSizeT, stream);
    if (rem)
        r_size += fwrite((const I8s *) ((I8s Hp) ptr + ((unsigned int) segs *
            (unsigned int) size * MaxSizeT)), (size_t) size, (size_t) rem,
            stream);
#else  /*  __TURBOC__ */
    r_size = fwrite(ptr, size, n, stream);
#endif /*  __TURBOC__ */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    if (r_size != n)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra tfwrite() inconsistency: n = %d  r_size = %d",
            n, r_size);
        porterrmsg(1204,(char *)(&(Fbuf[0])),1);
    }
    return r_size;
}

/*
 * tseek - set current file position as specified
 *
 * fp - pointer to FILE structure
 * rpos - specified file position
 * mode - not used
 *
 * Returns: error - return from fseek()
 */
I32s tseek(fp, rpos, mode)
FILE * fp;
long rpos;
int mode;
{   int error;
#ifdef AMIGA
    long filsiz, extlen;
    char *extbuf;
#endif /* AMIGA */
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef AMIGA
    error = fseek(fp, (long) 0, SEEK_END);
    if(error>=0)
    {   if (!((filsiz = tftell(fp)) < 0))
        {   if (rpos <= filsiz)
                error = fseek(fp, rpos, SEEK_SET);
            else
            {   extlen = rpos - filsiz;
                if (extbuf = calloc((size_t) extlen,
                        (size_t) sizeof(char)))
                {   if (tfwrite(extbuf, (size_t) extlen,
                            (size_t) sizeof(char), fp))
                        error = fseek(fp, rpos, SEEK_SET);
                    else
                        error = -1;
                    free(extbuf);
                }
                else
                    error = -1;
            }
        }
        else
            error = -1;
    }
#else /* AMIGA */
    error = fseek(fp, rpos, SEEK_SET);
#endif /* AMIGA */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return (error);
}

/*
 * tfprintf - formatted print to file
 *
 * filhndl - pointer to FILE structure
 * fmt - format string
 *
 * Returns: return from vfprintf()
 *
 */
#ifdef HAVE_STDARG_H
I32s tfprintf(FILE *filhndl, const char *fmt, ...)
#else /* HAVE_STDARG_H */
#ifdef _WIN32
I32s tfprintf(FILE *filhndl, const char *fmt,...)
#else /* _WIN32 */
#ifdef AMIGA
I32s tfprintf(FILE *filhndl, const char *fmt, ...)
#else /* AMIGA */
I32s tfprintf(va_alist)
va_dcl
#endif /* AMIGA */
#endif /* _WIN32 */
#endif /* HAVE_STDARG_H */
{   va_list args;
    I32s rtnval;

#ifndef _WIN32
#ifndef AMIGA
#ifndef HAVE_STDARG_H
FILE *filhndl;
I8s *fmt;
#endif /* HAVE_STDARG_H */
#endif /* AMIGA */
#endif /* _WIN32 */

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef HAVE_STDARG_H
    va_start(args, fmt);
#else /* HAVE_STDARG_H */
#ifdef _WIN32
    va_start(args, fmt);
#else /* _WIN32 */
#ifdef AMIGA
    va_start(args, fmt);
#else /* AMIGA */
    va_start(args);
#endif /* AMIGA */
#endif /* _WIN32 */
#endif /* HAVE_STDARG_H */

#ifndef _WIN32
#ifndef AMIGA
#ifndef HAVE_STDARG_H
    filhndl=va_arg(args, FILE *);
    fmt=va_arg(args, char *);
#endif /* HAVE_STDARG_H */
#endif /* AMIGA */
#endif /* _WIN32 */

    rtnval=vfprintf(filhndl, (const char *)fmt, args);
    va_end(args);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * tietime - get current time in seconds
 *
 * tloc - pointer to time_t structure to hold time
 *
 * Returns: current time in time_t structure
 *
 */
time_t tietime(tloc)
time_t *tloc;
{   time_t rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=time(tloc);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tgets - get line from stdin
 *
 * s - buffer to hold input line
 * size - size of input line buffer
 *
 */
I8s *tgets(s, size)
I8s *s;
I32s size;
{   I8s *rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=(I8s *)fgets((char *)s, size, stdin);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tfflush - flush file stream
 *
 * filhndl - pointer to FILE structure for stream to be flushed
 *
 * Returns: return from fflush()
 *
 */
I32s tfflush(filhndl)
FILE *filhndl;
{   I32s rtnval=0;
#ifndef AMIGA
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=fflush(filhndl);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
#endif /* AMIGA */
   return rtnval;
}

/*
 * txdrmem_create - link encode/decode buffer and XDR encode or decode
 *                     operation to XDR handle
 * xdrs - XDR handle
 * addr - pointer to encode/decode buffer
 * size - size of encode/decode buffer
 * op - XDR_ENCODE or XDR_DECODE
 */
void txdrmem_create(xdrs, addr, size, op)
XDR *xdrs;
I8s *addr;
I32u size;
enum xdr_op op;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    xdrmem_create(xdrs, (caddr_t)addr, (u_int)size, op);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_destroy - disassociate encode/decode buffer and
 *                XDR encode/decode operation from XDR handle
 *
 * xdrs - XDR handle
 * addr - pointer to encode/decode buffer
 * size - size of encode/decode buffer
 * op - XDR_ENCODE or XDR_DECODE
 */
void txdr_destroy(xdrs)
XDR *xdrs;
{   

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    xdr_destroy(xdrs);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * txdr_getpos - get current position in encode/decode buffer
 *               for given XDR handle
 *
 * xdrs - XDR handle
 * Returns: return from xdr_getpos()
 */
I32u txdr_getpos(xdrs)
XDR *xdrs;
{   I32u rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=xdr_getpos(xdrs);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * txdr_free - free buffer data decoded by XDR ENCODE
 *
 * proc - pointer to XDR encode/decode function
 * objp - decoded data buffer
 *
 */
void txdr_free(proc, objp)
xdrproc_t proc;
I8s *objp;
{

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    xdr_free(proc, (void *)objp);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * twrite - write to file descriptor
 *
 * fd - file descriptor
 * buf - pointer to buffer to be written
 * nbyte - number of bytes to be written
 *
 */
I32s twrite(fd, buf, nbyte)
I32s fd, nbyte;
char *buf;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=write(fd, buf, nbyte);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tclose - close file descriptor
 *
 * fd - file descriptor
 *
 */
I32s tclose(fd)
I32s fd;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=close(fd);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return rtnval;
}

/*
 * tftell - get current position in file for given stream
 * stream - pointer to FILE structure for given file stream
 * Returns: return from ftell()
 */
I32s tftell(stream)
FILE *stream;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=ftell(stream);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tstat - get file status
 *
 * path - path name of given file
 * buf - pointer to stat structure to contain returned status
 * Returns: return from stat() - 0=success, -1=error
 */
I32s tstat(path, buf)
char *path;
struct stat *buf;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=stat((const char *)path, buf);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tread - read from file descriptor
 *
 * fd - file descriptor
 * buf - pointer to buffer to hold data read
 * nbyte - number of bytes to be read
 * Return: return from read() - number of bytes read
 */
I32s tread(fd, buf, nbyte)
I32s fd, nbyte;
I8s *buf;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=read(fd, buf, nbyte);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tunlink - delete given file
 * path - pathname of given file
 * Returns: return from unlink - 0=success, -1=error
 */
I32s tunlink(path)
I8s *path;
{   I32s rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef DECVAX
    rtnval=delete(path);
#else
    rtnval=unlink((const char *)path);
#endif

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return rtnval;
}

/*
 * tisatty - test for interactive terminal linked to file descriptor
 * fd - given file descriptor
 * Returns: from isatty() - 1=linked to interactive terminal, 0=not linked
 */
int tisatty(fd)
int fd;
{   int rtnval;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtnval=isatty(fd);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return rtnval;
}

/*
 * trecalloc - recalloc for Unix, or for DOS arrays not greater than 64K
 *
 * ptr   - pointer to buffer to be reallocated
 * nsiz  - new buffer size
 * osiz  - old buffer size
 * label - MemProf label
 *
 * Returns:
 * tp - pointer to new buffer.
 *
 */
void *trecalloc(ptr, nsiz, osiz, label)
    void *ptr;
    I32u nsiz;                  /* new array size */
    I32u osiz;                  /* old array size */
    I32s label;
{   I8s Fp tp;
    I32u swapsiz = (osiz < nsiz) ? osiz : nsiz;

    tp=tcalloc(nsiz, 1, label);
    if (ptr)
    {   memcpy(tp, ptr, swapsiz);
        tfree(ptr, osiz, label);
        ptr = NULL;
    }
    return (void *) tp;
}

void porterrmsg(errlbl, errmsg, fatal)
I32s errlbl,fatal;
char *errmsg;
{	int sverrno;
#if !defined(ARGTIE)&&!defined(PROBE)&&!defined(THREADTREE)&&\
    !defined(GENALIGN)&&!defined(BGL_CLNT)&&!defined(SOUPUPDTRC)
    char syserrtxt[256];
#endif /* !defined(ARGTIE)&&!defined(PROBE)&&!defined(THREADTREE)&&
       !defined(GENALIGN)&&!defined(BGL_CLNT)&&!defined(SOUPUPDTRC) */

    sverrno=errno;
#if defined(ARGTIE)||defined(PROBE)||defined(THREADTREE)||\
    defined(GENALIGN)||defined(BGL_CLNT)||defined(SOUPUPDTRC)
    tfprintf(stderr,"%s\n", (errmsg)?errmsg:(strerror(sverrno)));
    if(fatal)
        exit(sverrno);
#else /* defined(ARGTIE)||defined(PROBE)||
         defined(THREADTREE)||defined(GENALIGN)||
         defined(BGL_CLNT)||defined(SOUPUPDTRC) */
#ifndef BGL_CLNT
    if(!errmsg)
        strcpy(&(syserrtxt[0]),(errmsg)?errmsg:(strerror(sverrno)));
    FEError(-1*errlbl, (fatal)?EXIT:NOEXIT,NOWRITE,&(syserrtxt[0]));
#else /* BGL_CLNT */
    BglError(-1*errlbl, (fatal)?EXIT:NOEXIT,
        (fatal)?WRITE:NOWRITE,&(syserrtxt[0]));
#endif /* BGL_CLNT */
#endif /* defined(ARGTIE)||defined(PROBE)||
          defined(THREADTREE)||defined(GENALIGN)||
          defined(BGL_CLNT)||defined(SOUPUPDTRC) */
}

/*
 * InitDynArr - initialize empty dynamic array
 *
 * dynarr - pointer to dynamic array structure
 * elsize - dynamic array element size
 */
void InitDynArr(dynarr, elsize, indir)
DynArr *dynarr;
I32s elsize, indir;
{   memset(dynarr, 0, sizeof(DynArr));
    dynarr->dyna_ctrl.dync_elsize=elsize;
    dynarr->dyna_ctrl.dync_indir=indir;
}

/*
 * AddDynArrEl - add entry to dynamic array
 *
 * dynarr - pointer to dynamic array structure
 * newelem - element of dynamic array to allocate
 *           -1 = next available
 * allocincr - amount by which to increase array size
 * memprof1 - memory profile label for main(direct)/pointer(indirect) array
 * memprof2 - memory profile label for structure of indirect array
 */
I32s AddDynArrEl(dynarr, newelem, allocincr, memprof1, memprof2)
DynArr *dynarr;
I32s allocincr, memprof1, newelem, memprof2;
{   I32s oldsize, rtnnewelem, alocelmsize;

    rtnnewelem=(newelem<0)?(dynarr->dyna_ctrl.dync_elmused):newelem;
    alocelmsize=(dynarr->dyna_ctrl.dync_indir)?
        sizeof(void *):(dynarr->dyna_ctrl.dync_elsize);
    if((dynarr->dyna_ctrl.dync_elmaloc)<=rtnnewelem)
    {   oldsize=dynarr->dyna_ctrl.dync_elmaloc;
        (dynarr->dyna_ctrl.dync_elmaloc)=rtnnewelem+allocincr;
        (dynarr->dyna_array)=
            (void *)threcalloc(dynarr->dyna_array,
            (dynarr->dyna_ctrl.dync_elmaloc)*alocelmsize,
            oldsize*alocelmsize, memprof1);
    }
    if(dynarr->dyna_ctrl.dync_indir)
    {   if(!(((void **)(dynarr->dyna_array))[rtnnewelem]))
        {   (((void **)(dynarr->dyna_array))[rtnnewelem])=
                (void *)tcalloc(1, dynarr->dyna_ctrl.
                dync_elsize, memprof2);
            (dynarr->dyna_ctrl.dync_elmused)++;
        }
        else
            rtnnewelem=(-1);
    }
    return rtnnewelem;
}

/*
 * FreeDynArr - free dynamic array
 * dynarr - pointer to dynamic array
 * memprof - MemProf label
 */
void FreeDynArr(dynarr, memprof1, memprof2)
DynArr *dynarr;
I32s memprof1, memprof2;
{   I32s i;
    if(dynarr->dyna_array)
    {   if(dynarr->dyna_ctrl.dync_indir)
        {   for(i=0;i<(dynarr->dyna_ctrl.dync_elmaloc);i++)
                if(((void **)(dynarr->dyna_array))[i])
                    thfree(((void **)(dynarr->dyna_array))[i],
                        dynarr->dyna_ctrl.dync_elsize, memprof2);
            thfree(dynarr->dyna_array,
                (dynarr->dyna_ctrl.dync_elmaloc)*sizeof(void *), memprof1);
        }
        else
            thfree(dynarr->dyna_array,
                (dynarr->dyna_ctrl.dync_elmaloc)*
                (dynarr->dyna_ctrl.dync_elsize), memprof1);
    }
    InitDynArr(dynarr, dynarr->dyna_ctrl.dync_elsize,
        dynarr->dyna_ctrl.dync_indir);
}

/*
 * tfopen - open file
 *
 * name - name of file to open
 * type - "open" mode supplied to fopen()
 *
 * Returns:
 * filhndl - pointer to FILE structure for opened file
 */
FILE *tfopen(filename, type)
I8s *filename, *type;
{   FILE *filhndl;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    filhndl=fopen((const char *)filename, (const char *)type);

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

   return filhndl;
}

/*
 * txdr_I32s - encode/decode Tierra I32s structure
 *
 * xdrs - XDR handle
 * objp - pointer to I32s structure
 *
 */
void txdr_I32s(xdrs, objp)
XDR *xdrs;
I32s *objp;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if(!xdr_I32s(xdrs, objp))
        porterrmsg(1214,"txdr_I32s failed",1);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * thfree - free for arrays of greater than 64K
 *
 * ptr - block pointer
 * siz - block size
 * label - block MEM_PROF label
 *
 * Returns: none
 */
void thfree(ptr, siz, label)
    void *ptr;
    I32s siz;
    I32s label;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;

    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    farfree(ptr);
#else  /* __TURBOC__ */
    free(ptr);
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef MEM_PROF
    MemStats(label, 0, siz);
#endif /* MEM_PROF */
}

/*
 * threcalloc - huge recalloc for arrays of greater than 64K
 * 
 * ptr   - pointer to buffer to be reallocated
 * nsiz  - new buffer size
 * osiz  - old buffer size
 * label - MemProf label
 *
 * Returns:
 * hp - pointer to new buffer.
 */
void *threcalloc(ptr, nsiz, osiz, label)
    void *ptr;
    I32u nsiz;                  /* new array size */
    I32u osiz;                  /* old array size */
    I32s label;
{
    I8s Hp hp;

#ifdef __TURBOC__
    hp = (void *) thcalloc(nsiz, 1, label);
    if (ptr)
    {   segs = (I32s) (swapsiz / (I32u) MaxSizeT);
        rem = (I32s) (swapsiz % (I32u) MaxSizeT);
        dp = hp;
        sp = ptr;
        if (segs)
            for (i = 0; i < segs; i++)
            {   memcpy(dp, sp, MaxSizeT);
                dp += MaxSizeT;
                sp += MaxSizeT;
            }
        if (rem)
            memcpy(dp, sp, rem);
        thfree(ptr, osiz, label);
        ptr = NULL;
    }
#else /* __TURBOC__ */
    if((hp=realloc(ptr, nsiz)))
    {   if(nsiz>osiz)
            memset(&(hp[osiz]), 0, nsiz-osiz);
    }
    else
    {   tsprintf((char *)(&(Fbuf[0])),
    "threcalloc() error: realloc failed, num: %d siz: %d label: %d ",
            nsiz, 1, label);
        porterrmsg(1202,(char *)(&(Fbuf[0])),1);
    }
#endif /* __TURBOC__ */
    return (void *) hp;
}

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(CLSTRSRVR)||defined(SOUPUPDTRC) */
