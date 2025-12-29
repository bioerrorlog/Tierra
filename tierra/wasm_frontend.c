/* wasm_frontend.c - WebAssembly Frontend for Tierra */
/* Provides browser-compatible interface for Tierra simulation */

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include "tierra.h"
#include "globals.h"

/* Stats structure for JavaScript export */
typedef struct {
    I32s num_cells;
    I32s generations;
    I32s inst_exe_m;
    I32s inst_exe_i;
    I32s soup_size;
} TierraStats;

/* Frontend initialization - stub for WASM */
void FEStartup(void)
{
    /* No X11/curses initialization needed for WASM */
    sgfile = stderr; /* Use console for debug output */
}

/* Frontend menu - stub for WASM */
void FEMenu(void)
{
    /* No interactive menu in WASM */
    /* JavaScript controls the simulation */
}

/* Frontend exit - stub for WASM */
void FEExit(I32s code)
{
    /* Notify JavaScript of exit */
    EM_ASM_({
        if (Module.onExit) Module.onExit($0);
    }, code);
}

/* Frontend statistics update - stub for WASM */
void FEStats(void)
{
    /* Statistics are pulled by JavaScript via tierra_get_stats() */
    /* No need to push updates */
}

/* Frontend error handler - stub for WASM */
void FEError(I32s errno1, I32s exit, I32s write, char * buff)
{
    /* Report error to JavaScript console */
    EM_ASM_({
        console.error('Tierra Error', $0, UTF8ToString($3));
    }, errno1, exit, write, buff);
}

/* Keyboard hit check - always false for WASM */
int FEKeyHit(void)
{
    return 0; /* No keyboard input in WASM */
}

/* WASM-specific exported API functions */

/**
 * Initialize Tierra simulation
 * Returns 0 on success, non-zero on error
 */
EMSCRIPTEN_KEEPALIVE
int tierra_init(void)
{
    char* argv[] = {"tierra", "soup_in"};

    /* Initialize variables */
    initvar();

    /* Load soup configuration and inoculate */
    GetSoup(2, argv);

    return 0;
}

/**
 * Execute simulation steps
 * @param steps Number of slicer iterations to execute
 */
EMSCRIPTEN_KEEPALIVE
void tierra_step(int steps)
{
    int i;
    for(i = 0; i < steps; i++) {
        /* Call the slicer function (time-slicing algorithm) */
        (*slicer)();

        /* Check if reaper needs to run */
        ReapCheck();
    }
}

/**
 * Get pointer to soup memory
 * @param size Output parameter for soup size
 * @return Pointer to soup memory array
 */
EMSCRIPTEN_KEEPALIVE
unsigned char* tierra_get_soup(int* size)
{
    *size = SoupSize;
    return (unsigned char*)soup;
}

/**
 * Get current simulation statistics
 * @return Pointer to static stats structure
 */
EMSCRIPTEN_KEEPALIVE
TierraStats* tierra_get_stats(void)
{
    static TierraStats stats;

    stats.num_cells = NumCells;
    stats.generations = (I32s)Generations;
    stats.inst_exe_m = InstExe.m;
    stats.inst_exe_i = InstExe.i;
    stats.soup_size = SoupSize;

    return &stats;
}

/**
 * Reset simulation to initial state
 */
EMSCRIPTEN_KEEPALIVE
void tierra_reset(void)
{
    /* Save current soup before reset */
    WriteSoup(1);

    /* Reinitialize */
    tierra_init();
}

/**
 * Get size histogram data
 * @param hist_out Output array (must be allocated by caller)
 * @param max_size Maximum size to include in histogram
 * @return Number of entries written
 */
EMSCRIPTEN_KEEPALIVE
int tierra_get_size_histogram(int* hist_out, int max_size)
{
    int i;
    int count = 0;

    /* sl[] array contains size histogram, use siz_sl for bounds */
    for(i = 0; i < max_size && i < siz_sl; i++) {
        if(sl[i] != NULL) {
            hist_out[i] = sl[i]->num_c;  /* number of creatures of this size */
            if(sl[i]->num_c > 0) count++;
        } else {
            hist_out[i] = 0;
        }
    }

    return count;
}

/**
 * Pause simulation (for future use)
 */
EMSCRIPTEN_KEEPALIVE
void tierra_pause(void)
{
    /* Set flag or perform pause operations */
    /* Currently just a placeholder */
}

/**
 * Resume simulation (for future use)
 */
EMSCRIPTEN_KEEPALIVE
void tierra_resume(void)
{
    /* Clear pause flag or perform resume operations */
    /* Currently just a placeholder */
}

/* Micromon stubs - interactive debugger not needed for WASM */
void GoBrkClr(void)
{
    /* No interactive debugger in WASM */
}

void Micro_Spy(Pcells cp)
{
    /* No interactive debugger in WASM */
}

void ChkBrkBd(void)
{
    /* No interactive debugger in WASM */
}

Pcells FindHost(I32s soupaddr, I32s *hosttype)
{
    /* No interactive debugger in WASM */
    *hosttype = 0;
    return NULL;
}

/* Frontend UI stubs - no curses/X11 UI in WASM */
void FEBanner(void)
{
    /* No banner display in WASM */
}

void FEPrintf(I32s scr_x, I32s scr_y, I32s scr_a, I32s slee, char * buff)
{
    /* Log to console instead of displaying on screen */
    EM_ASM_({
        console.log('FE:', UTF8ToString($0));
    }, buff);
}

void FEPlan(I32s tielog)
{
    /* No plan view in WASM */
}

void ChangeGeneBnker(I32s gb)
{
    /* No interactive gene banker selection in WASM */
}

/* Signal handler stub */
void T_sig_int(int sig)
{
    /* Signal handling not needed in WASM browser environment */
}

#endif /* __EMSCRIPTEN__ */
