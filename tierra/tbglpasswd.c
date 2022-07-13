/* @(#)tbglpasswd.c   2.00 8/1 /96                                         */
/* Beagle Password Entry                                                   */
/* Tsukasa Kimezawa                                                        */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#define MAXNAMLEN      31
#define MAXPASSWDLEN   15
#define MAXLINLEN      83
#define MAXUSER      1024
#define SALTSIZE        3

#include <stdio.h>
#if HAVE_SYS_TIME_H
#include <time.h>
#endif /* HAVE_SYS_TIME_H */

#include <stdlib.h>
#include <string.h>
#ifdef unix
#include <sys/stat.h>
#endif /* unix */

#include "portable.h"

#ifdef unix
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#else /* HAVE_CRYPT_H */
#ifdef __linux__
#define _XOPEN_SOURCE
#endif /* __linux__ */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#endif /* HAVE_CRYPT_H */
#if defined(NEED_CRYPT_PROTO)
extern char *crypt P_((const char *key, const char *salt));
#endif /* defined(NEED_CRYPT_PROTO) */
#endif /* unix */

struct Id
{   I8s name[MAXNAMLEN+1];      /* E-mail address is preferable */
    I8s passwd[MAXPASSWDLEN+1]; /* Password is less than 8 characters */
};
typedef struct Id Id;

static void update_passwd P_((I32s pos, I32s max_id, I8s *name, Id *id));
static void new_user P_((I32s *usercnt, I8s *name, Id *id));
static void updatefiles P_((I32s usercnt, Id *id));
static void tty_raw P_((I32s fd));
static void tty_reset P_((I32s fd));
static void passwd_encode P_((I8s *input, I8s *insalt, I8s *output));

#ifdef TBSD
#ifdef HAVE_SGTTY_H
#include <sgtty.h>
#endif /* HAVE_SGTTY_H */
struct sgttyb tty_mode;         /* save tty mode here */
#endif /* TBSD */

#ifdef TSysV
#ifdef HAVE_TERMIO_H
#include <termio.h>
#endif /* HAVE_TERMIO_H */
struct termio tty_mode;         /* save tty mode here */
#endif /* TSysV */

FILE *fd_passwd, *fd_tmp;
int main P_((int argc, char **argv));

int main(argc, argv)
int argc;
char **argv;
{   I32s i, max_id;
    I8s BufFile[MAXLINLEN+1];
    I8s name[MAXNAMLEN+1];
    Id id[MAXUSER];

    if((fd_tmp=fopen("etc/tmp", "w")))
    {   if((fd_passwd=fopen("etc/passwd","a+")))
        {   if(!(fseek(fd_passwd, 0, SEEK_SET)))
            {   for(max_id=0;fgets((char *)(&(BufFile[0])),
                   sizeof(BufFile), fd_passwd);max_id++)
                {   sprintf((char *)(&(id[max_id].name[0])),
                        strtok((char *)(&(BufFile[0])), ":"));
                    sprintf((char *)(&(id[max_id].passwd[0])), strtok(NULL, ":"));
                }
                printf("Input your username: ");
        
                fgets((char *)(&(name[0])), sizeof(name), stdin);
                name[strlen((const char *)(&(name[0])))-1]='\0';
            
                for (i=0;i<max_id;i++)
                {   if(!(strcmp((const char *)(&(id[i].name[0])),
                        (const char *)(&(name[0])))))
                    {   update_passwd(i, max_id, &(name[0]), &(id[0]));
                        break;
                    }
                }
                if(i==max_id)
                    new_user(&max_id, &(name[0]), &(id[0]));
    
                updatefiles(max_id, &(id[0]));
            }
            else
                perror("fseek :etc/passwd");
        }
        else
            perror("fopen :etc/passwd");
    }
    else
        perror("fopen :etc/tmp");
    return 0;
}

/* Update password */
static void update_passwd(pos, max_id, name, id)
I32s pos, max_id;
I8s *name;
Id *id;
{   I8s input_passwd1[MAXLINLEN+1], input_passwd2[MAXLINLEN+1];
#ifdef unix
    I8s salt[SALTSIZE];
#endif

    tty_raw(0);
    printf("Changing Tierra Monitor password.\n");
    printf("Old password : ");
    fgets((char *)(&(input_passwd1[0])), sizeof(input_passwd1), stdin);
    input_passwd1[strlen((const char *)(&(input_passwd1[0])))-1]='\0';

    printf("\n");
#ifdef unix
    salt[0]=id[pos].passwd[0];
    salt[1]=id[pos].passwd[1];
    salt[2]='\0';
    passwd_encode(&(input_passwd1[0]), &(salt[0]),
        &((input_passwd2)[0]));
#else
    passwd_encode(&(input_passwd1[0]), &(name[0]),
        &((input_passwd2)[0]));
#endif
    if(strcmp((const char *)(&(input_passwd2[0])),
        (const char *)(&((id[pos].passwd)[0]))))
        printf("Mismatch - password unchanged.\n");
    else
    {   printf("New password : ");
        fgets((char *)(&(input_passwd1[0])), sizeof(input_passwd1), stdin);
        input_passwd1[strlen((const char *)(&(input_passwd1[0])))-1]='\0';
        printf("\n");

        if(strlen((const char *)(&(input_passwd1[0])))<5)
        {   printf("\n      Input password is too short.\n");
            printf("    It should be more than 4 charactors.\n");
        }
        else
        {   printf("Retype new password : ");
            fgets((char *)(&(input_passwd2[0])), sizeof(input_passwd2), stdin);
            input_passwd2[strlen((char *)(&(input_passwd2[0])))-1]='\0';
            printf("\n");
        
            if(!(strcmp((const char *)(&(input_passwd1[0])),
                (const char *)(&(input_passwd2[0])))))
            {   strcpy((char *)(&((id[pos].name)[0])), (const char *)(&(name[0])));
#ifdef unix
                passwd_encode(&(input_passwd2[0]), NULL,
                    &((id[pos].passwd)[0]));
#else
                passwd_encode(&(input_passwd2[0]), &(name[0]),
                    &((id[pos].passwd)[0]));
#endif
                printf("    Tierra Monitor password changed.\n");
            }
            else
                printf("Mismatch - password unchanged.\n");
        }
    }
    tty_reset(0);
}

/* Enter new network monitor user */
static void new_user(usercnt, name, id)
I32s *usercnt;
I8s *name;
Id *id;
{   I8s input_passwd1[MAXLINLEN+1], input_passwd2[MAXLINLEN+1];

    tty_raw(0);
    printf("enter Tierra password  : ");
    fgets((char *)(&(input_passwd1[0])), sizeof(input_passwd1), stdin);
    input_passwd1[strlen((const char *)(&(input_passwd1[0])))-1]='\0';
    if(strlen((const char *)(&(input_passwd1[0])))<5)
    {   printf("\nentered password is too short.\n");
        printf("    It should be more than 4 charactors.\n");
    }
    else
    {   printf("\n");
        printf("Retype password : ");
        fgets((char *)(&(input_passwd2[0])), sizeof(input_passwd2), stdin);
        input_passwd2[strlen((const char *)(&(input_passwd2[0])))-1]='\0';
        printf("\n");
        tty_reset(0);
        if(strcmp((const char *)(&(input_passwd1[0])),
            (const char *)(&(input_passwd2[0]))))
            printf("Mismatch - password unadded.\n");
        else
        {   strcpy((char *)(&((id[*usercnt].name)[0])), (const char *)(&(name[0])));
#ifdef unix
            passwd_encode(&(input_passwd2[0]), NULL,
                &((id[(*usercnt)++].passwd)[0]));
#else
            passwd_encode(&(input_passwd2[0]), &(name[0]),
                &((id[(*usercnt)++].passwd)[0]));
#endif /* unix */
            printf("    Tierra Monitor password entered.\n");
        }
    }
}

static void updatefiles(usercnt, id)
I32s usercnt;
Id *id;
{   I32s i;
    for (i=0;i<usercnt;i++)
        fprintf(fd_tmp, "%s:%s:\n", id[i].name, id[i].passwd);
    fclose(fd_tmp);
    fclose(fd_passwd);
#ifdef unix
    chmod("etc/tmp", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif /* unix */
    remove("etc/passwd");
    rename("etc/tmp", "etc/passwd");
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * Hide the key input from display
 */
static void tty_raw(fd)
    I32s fd;                    /* of terminal device */
{
#ifdef TBSD
    struct sgttyb temp_mode;
    ioctl(fd, TIOCGETP, (char *) &temp_mode);
    tty_mode = temp_mode;  /* save for restoring later */
    temp_mode.sg_flags &= ~ECHO;  /* turn ECHO off */
    ioctl(fd, TIOCSETP, (char *) &temp_mode);
#endif /* TBSD */

#ifdef TSysV
    struct termio temp_mode;

    ioctl(fd, TCGETA, (char *) &temp_mode);
    tty_mode = temp_mode;  /* save for restoring later */
    temp_mode.c_lflag &= ~ECHO;
    ioctl(fd, TCSETA, (char *) &temp_mode);
#endif /* TSysV */
}

/*
 * Reset tty mode
 */
static void tty_reset(fd)
I32s fd;
{
#ifdef TBSD
    ioctl(fd, TIOCSETP, (char *) &tty_mode);
#endif /* TBSD */

#ifdef TSysV
    ioctl(fd, TCSETA, (char *) &tty_mode);
#endif /* TSysV */
}

/* encrypt the password */
static void passwd_encode(input, insalt, output)
I8s *input, *insalt, *output;
{
#ifdef unix
    I8s salt[SALTSIZE];
    static I8s saltset[]=
        "./0123456789ABCDEFGHIJKLMNOPQRTSUVWXYZabcdefghijklmnopqrstuvwxyz";
#else
    I32s i;
#endif /* unix */

#ifdef unix
    if(!insalt)
    {   srand(time(NULL));
        salt[0]=saltset[rand()%(sizeof(saltset)-1)];
        salt[1]=saltset[rand()%(sizeof(saltset)-1)];
        salt[2]='\0';
    }
    else
        strcpy((char *)(&(salt[0])), (const char *)insalt);
    strcpy((char *)output, crypt((const char *)input, (const char *)&(salt[0])));
#else
    for(i=0;i<(int)(strlen((const char *)(&(input[0]))));i++)
        output[i]=(I8s)(((I32s)input[i])+((I32s)(insalt[i]%10)));
    output[i]='\0';
#endif
}
