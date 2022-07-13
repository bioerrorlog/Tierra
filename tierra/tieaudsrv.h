#ifndef TIEAUDIO_H
#define TIEAUDIO_H

#ifdef TIEAUDIO

#ifndef EXTERN
#define EXTERN extern
#endif

extern I32s AudioPopThresh;
extern I32s AudioPort; /* audio socket port */
extern I32s AudConnProcFreq; /* audio socket port */
EXTERN int audlstnskt,audcnctskt;
EXTERN I32s LastAudConnProc;

extern void OpnTieAud P_((void));
extern void SndTieAudDat P_((I32s sizcls));
extern void ClsTieAud P_((void));
extern void AcceptClient P_((void));

#ifdef TIEAUDMP
#define MAXAUDMPFILNAMLEN 255
#define AUDMPFILNAM "audio.log"
EXTERN FILE *audmptr;
extern void OpnTieAudDmp P_((void));
extern void ClsTieAudDmp P_((void));
#endif /* TIEAUDMP */

#endif /* TIEAUDIO */
#endif /* TIEAUDIO_H */
