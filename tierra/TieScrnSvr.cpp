/* Windows application - start screen blanker
                         send TIERRAWAKEUP message
                         wait for screen blanker to terminate
                         send TIERRASLEEP message
                         pass handle screen blanker modes
*/

#define ERMSGBUFSIZE 80
#define CMDNAMBUFSIZE 80
#define CMDBUFSIZE 80
#define SWBUFSIZE 10
#define SCRNSVR "C:\\Windows\\Channel Screen Saver.scr"
#define TIERRAKEYMSEMON "TierraKeyMseMon"
#define TIERRAWAKEUP    "TIERRAWAKEUP"
#define TIERRASLEEP     "TIERRASLEEP"
#define TIERRACLASS     "Tierra"

#include <windows.h>
#include <stdio.h>
#include <memory.h>

void DisErrMsg(char *ermsg, int *errflag);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{   STARTUPINFO lpStartupInfo;
    PROCESS_INFORMATION lpProcessInformation;
    const char *CLASSNAME=TIERRAKEYMSEMON;
    int argc;
    int argparwnd=0;
    int errflag=0;
    const char *cmdline=GetCommandLine();
    char switchbuf[SWBUFSIZE], cmdnambuf[CMDNAMBUFSIZE];
    char cmdbuf[CMDBUFSIZE];
    enum TScrMode {smNone,smConfig,smPassword,smPreview,smSaver};
    TScrMode ScrMode=smNone;
    const char *argparwndbuf, *cmdarg;;
    UINT tie_wkup_id, tie_sleep_id;
    HWND tie_hWnd;

    cmdnambuf[0]='\0';
    switchbuf[0]='\0';
    argc=sscanf(cmdline, "%s %s %d", 
        &(cmdnambuf[0]), &(switchbuf[0]), &argparwnd);
    cmdarg=&(cmdline[strlen(&(cmdnambuf[0]))]);
    sprintf(&(cmdbuf[0]),"%s%s", SCRNSVR, cmdarg);

    if(argc>1)
    {   if(switchbuf[0]=='/')
        {   if(strlen(&(switchbuf[0]))==2)
                if(argc==3)
                {   if(switchbuf[1]=='p')
                        ScrMode=smPreview;
                }
                else if(argc==2)
                    if(switchbuf[1]=='s')
                        ScrMode=smSaver;
        }
        else if(switchbuf[0]=='-')
            if(argc==2)
                if(strlen(&(switchbuf[0]))>3)
                {   argparwndbuf=&(switchbuf[3]);
                    if(switchbuf[2]==':')
                        if(switchbuf[1]=='p')
                            if(sscanf(argparwndbuf,
                                "%d", &argparwnd))
                                ScrMode=smPreview;
                }
                else
                    if(strlen(&(switchbuf[0]))==2)
                        if(switchbuf[1]=='s')
                            ScrMode=smSaver;
    }

    if(tie_wkup_id=RegisterWindowMessage(TIERRAWAKEUP))
        if(tie_sleep_id=RegisterWindowMessage(TIERRASLEEP))
        {   if(ScrMode==smSaver)
                if(tie_hWnd=FindWindow(TIERRACLASS, NULL))
                    if(!PostMessage(tie_hWnd, tie_wkup_id,
                        (WPARAM)0, (LPARAM)0))
                        DisErrMsg("TIERRASLEEP post failed",
                            &errflag);
            if(!errflag)
            {   memset(&lpStartupInfo, 0, sizeof(STARTUPINFO));
                lpStartupInfo.cb=sizeof(STARTUPINFO);
                if(CreateProcess(NULL, &(cmdbuf[0]),
                    NULL, NULL, FALSE, NULL, NULL, NULL, 
                    &lpStartupInfo, &lpProcessInformation))
                    if(WaitForSingleObject(
                        lpProcessInformation.hProcess,
                        INFINITE)!=WAIT_FAILED)
                    {   if(ScrMode==smSaver)
                            if(tie_hWnd=FindWindow(
                                TIERRACLASS, NULL))
                                if(!PostMessage(
                                    tie_hWnd, tie_sleep_id,
                                    (WPARAM)0, (LPARAM)0))
                                    DisErrMsg(
                                        "TIERRASLEEP post failed",
                                        &errflag);
                    }
                    else
                        DisErrMsg("WaitForSingleObject failed",
                            &errflag);
                else
                    DisErrMsg("CreateProcess failed", &errflag);
            }
        }
        else
            DisErrMsg("TIERRASLEEP register failed", &errflag);
    else
        DisErrMsg("TIERRAWAKEUP register failed", &errflag);

    return 0;
}

void DisErrMsg(char *ermsg, int *errflag)
{   char ermsgbuf[ERMSGBUFSIZE];
    sprintf(&(ermsgbuf[0]),"%s - %d", ermsg, GetLastError());
    MessageBox(NULL, &(ermsgbuf[0]), NULL, MB_OK);
    (*errflag)=1;
}
