#define INCLUDED	// Define externs here
#include "sst.h"
#include <ctype.h>
#ifdef MSDOS
#include <dos.h>
#endif
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define OK 0      // linux return value: 0 = successful
#define ENOMEM 12 // linux return value: 12 = out of memory

// outLineBuffer collects one output line by several calls to tprntf
#define INITIAL_SIZE_OF_RETURNBUFFER 10 // reduced for tests (would be 16*1024)
#define INCR_SIZE_OF_RETURNBUFFER 5 // reduced for testing (would be 1024*1024)
#define OUTLINE_BUFFER_MAXSIZE 4095
char outLineBuffer[sizeof(char)*OUTLINE_BUFFER_MAXSIZE] = "";
char *tReturnString;
size_t sizeOfReturnBuffer, curPosOutBuffer = 0, lenOutLine = 0;

// printf replacement to collect the parts of one output line in outLineBuffer.
static int tprntf(const char *fmt, ...)
{
    const size_t maxLen = sizeof(char)*OUTLINE_BUFFER_MAXSIZE;
    va_list arg;
    int rVal;
    
    va_start (arg, fmt);
    rVal = vsnprintf (&outLineBuffer[lenOutLine], maxLen-lenOutLine, fmt, arg);
    va_end (arg);
    lenOutLine += strlen(&outLineBuffer[lenOutLine]);
    
    return rVal;
}

// fputs replacement to collect the parts of one output line in outLineBuffer.
static int tPuts(const char *s, FILE *stream)
{
    const size_t maxLen = sizeof(char)*OUTLINE_BUFFER_MAXSIZE;
    int rVal;
    
    if (stream == stdout) {
        rVal = snprintf (&outLineBuffer[lenOutLine], maxLen-lenOutLine, "%s",s);
        lenOutLine += strlen(&outLineBuffer[lenOutLine]);
        if (rVal < 0) {
            return EOF;
        } else {
            return rVal;
        }
    } else {
        return fputs(s, stream);
    }
}

// Output line is now complete: copy to return buffer and reset line buffer.
//   Don't forget to call this (especially for the last prints) so the line
//   is added to the return string!
static void tprntNewLine()
{
    size_t newSize;
    long remainingLenOutBuffer, neededSize;
    char *newOutBuffer;
    
    remainingLenOutBuffer = sizeOfReturnBuffer-curPosOutBuffer-1;
    lenOutLine++; // + newline character (\n)
    remainingLenOutBuffer -= lenOutLine;
    
    if (remainingLenOutBuffer < 0) {
        //newSize = sizeOfReturnBuffer + sizeof(char)*INCR_SIZE_OF_RETURNBUFFER;
        neededSize = -remainingLenOutBuffer;
        if (neededSize < sizeof(char)*INCR_SIZE_OF_RETURNBUFFER)
            neededSize = sizeof(char)*INCR_SIZE_OF_RETURNBUFFER;
        newSize = sizeOfReturnBuffer + neededSize;
        
        if ((newOutBuffer = realloc(tReturnString, newSize)) != 0) {
            tReturnString = newOutBuffer;
            sizeOfReturnBuffer = newSize;
        } else {
            // just write part that is still available:
            lenOutLine += remainingLenOutBuffer;
            //remainingLenOutBuffer = 0;
        }
    }
    
    snprintf(&tReturnString[curPosOutBuffer],lenOutLine+1,"%s\n",outLineBuffer);
    
    curPosOutBuffer += lenOutLine;
    outLineBuffer[0] = 0;
    lenOutLine = 0;
}

void tFreeMemory ()
{
    free(tReturnString);
}

// printf replacement: necessary because #define replaces all printf with tprntf
int prf(char *fmt, ...)
{
    int result;
    va_list args;
    va_start(args, fmt);
    result = vprintf(fmt, args);
    va_end(args);
    return result;
}

#ifndef COLLECT_STDOUT_IN_BUFFER
#define COLLECT_STDOUT_IN_BUFFER
//#define printf tprntf
//#define fputs tPuts
#endif

/*int getch(void);
 */

static char line[128], *linep = line;
static int linecount;	/* for paging */

static void clearscreen(void);

/* Compared to original version, I've changed the "help" command to
 "call" and the "terminate" command to "quit" to better match
 user expectations. The DECUS version apparently made those changes
 as well as changing "freeze" to "save". However I like "freeze".
 
 When I got a later version of Super Star Trek that I was converting
 from, I added the emexit command.
 
 That later version also mentions srscan and lrscan working when
 docked (using the starbase's scanners), so I made some changes here
 to do this (and indicating that fact to the player), and then realized
 the base would have a subspace radio as well -- doing a Chart when docked
 updates the star chart, and all radio reports will be heard. The Dock
 command will also give a report if a base is under attack.
 
 Movecom no longer reports movement if sensors are damaged so you wouldn't
 otherwise know it.
 
 Also added:
 
 1. Better base positioning at startup
 
 2. deathray improvement (but keeping original failure alternatives)
 
 3. Tholian Web
 
 4. Enemies can ram the Enterprise. Regular Klingons and Romulans can
 move in Expert and Emeritus games. This code could use improvement.
 
 5. The deep space probe looks interesting! DECUS version
 
 6. Cloaking (with contributions from Erik Olofsen) and Capturing (BSD version).
 
 */

// I don't like the way this is done, relying on an index. But I don't
// want to invest the time to make this nice and table driven.

static char *commands[] = {
    "srscan",
    "lrscan",
    "phasers",
    "photons",
    "move",
    "shields",
    "dock",
    "damages",
    "chart",
    "impulse",
    "rest",
    "warp",
    "status",
    "sensors",
    "orbit",
    "transport",
    "mine",
    "crystals",
    "shuttle",
    "planets",
    "request",
    "report",
    "computer",
    "commands",
    "emexit",
    "probe",
    "cloak",
    "capture",
    "score",
    "abandon",
    "destruct",
    "freeze",
    "deathray",
    "debug",
    "call",
    "quit",
    "help"
    
};

#define NUMCOMMANDS (sizeof(commands)/sizeof(char *))

static void listCommands(int x) {
    prout("   SRSCAN    MOVE      PHASERS   CALL\n"
          "   STATUS    IMPULSE   PHOTONS   ABANDON\n"
          "   LRSCAN    WARP      SHIELDS   DESTRUCT\n"
          "   CHART     REST      DOCK      QUIT\n"
          "   DAMAGES   REPORT    SENSORS   ORBIT\n"
          "   TRANSPORT MINE      CRYSTALS  SHUTTLE\n"
          "   PLANETS   REQUEST   DEATHRAY  FREEZE\n"
          "   COMPUTER  EMEXIT    PROBE     COMMANDS");
    proutn("   ");
#ifdef SCORE
    proutn("SCORE     ");
#endif
#ifdef CLOAKING
    proutn("CLOAK     ");
#endif
#ifdef CAPTURE
    proutn("CAPTURE   ");
#endif
    if (x) proutn("HELP     ");
    prout("");
}

static void helpme(void) {
    int i, j;
    char cmdbuf[32];
    char linebuf[132];
    FILE *fp;
    /* Give help on commands */
    int key;
    key = scan();
    while (TRUE) {
        if (key == IHEOL) {
            proutn("Help on what command?");
            key = scan();
        }
        if (key == IHEOL) return;
        for (i = 0; i < NUMCOMMANDS; i++) {
            if (strcmp(commands[i], citem)==0) break;
        }
        if (i != NUMCOMMANDS) break;
        skip(1);
        prout("Valid commands:");
        listCommands(FALSE);
        key = IHEOL;
        chew();
        skip(1);
    }
    if (i == 23) {
        strcpy(cmdbuf, " ABBREV");
    }
    else {
        strcpy(cmdbuf, "  Mnemonic:  ");
        j = 0;
        while ((cmdbuf[j+13] = toupper(commands[i][j])) != 0) j++;
    }
    fp = fopen("sst.doc", "r");
    if (fp == NULL) {
        prout("Spock-  \"Captain, that information is missing from the");
        prout("   computer. You need to find SST.DOC and put it in the");
        prout("   current directory.\"");
        return;
    }
    i = strlen(cmdbuf);
    do {
        if (fgets(linebuf, 132, fp) == NULL) {
            prout("Spock- \"Captain, there is no information on that command.\"");
            fclose(fp);
            return;
        }
    } while (strncmp(linebuf, cmdbuf, i) != 0);
    
    skip(1);
    prout("Spock- \"Captain, I've found the following information:\"");
    skip(1);
    
    do {
        if (linebuf[0]!=12) { // ignore page break lines
            linebuf[strlen(linebuf)-1] = '\0'; // No \n at end
            prout(linebuf);
        }
        fgets(linebuf,132,fp);
    } while (strstr(linebuf, "******")==NULL);
    fclose(fp);
}

static void makemoves(void) {
    int i, hitme;
    //	char ch;
    while (TRUE) { /* command loop */
        hitme = FALSE;
        justin = 0;
        Time = 0.0;
        i = -1;
        while (TRUE)  { /* get a command */
            chew();
            skip(1);
            proutn("COMMAND> ");
            if (scan() == IHEOL) continue;
            for (i=0; i < 29; i++) // Abbreviations allowed for the first 29 commands, only.
                if (isit(commands[i]))
                    break;
            if (i < 29) break;
            for (; i < NUMCOMMANDS; i++)
                if (strcmp(commands[i], citem) == 0) break;
            if (i < NUMCOMMANDS
#ifndef CLOAKING
                && i != 26 // ignore the CLOAK command
#endif
#ifndef CAPTURE
                && i != 27 // ignore the CAPTURE command
#endif
#ifndef SCORE
                && i != 28 // ignore the SCORE command
#endif
#ifndef DEBUG
                && i != 33 // ignore the DEBUG command
#endif
                ) break;
            
            if (skill <= SFAIR)  {
                prout("UNRECOGNIZED COMMAND. LEGAL COMMANDS ARE:");
                listCommands(TRUE);
            }
            else prout("UNRECOGNIZED COMMAND.");
        }
        switch (i) { /* command switch */
            case 0:			// srscan
                srscan(1);
                break;
            case 1:			// lrscan
                lrscan();
                break;
            case 2:			// phasers
                phasers();
                if (ididit) {
#ifdef CLOAKING
                    if (irhere && d.date >= ALGERON && !isviolreported) {
                        prout("The Romulan ship discovers you are breaking the Treaty of Algeron!");
                        ncviol++;
                        isviolreported = TRUE;
                    }
#endif
                    hitme = TRUE;
                }
                break;
            case 3:			// photons
                photon();
                if (ididit) {
#ifdef CLOAKING
                    if (irhere && d.date >= ALGERON && !isviolreported) {
                        prout("The Romulan ship discovers you are breaking the Treaty of Algeron!");
                        ncviol++;
                        isviolreported = TRUE;
                    }
#endif
                    hitme = TRUE;
                }
                break;
            case 4:			// move
                warp(1);
                break;
            case 5:			// shields
                sheild(1);
                if (ididit) {
                    attack(2);
                    shldchg = 0;
                }
                break;
            case 6:			// dock
                dock();
                break;
            case 7:			// damages
                dreprt();
                break;
            case 8:			// chart
                chart(0);
                break;
            case 9:			// impulse
                impuls();
                break;
            case 10:		// rest
                waiting();
                if (ididit) hitme = TRUE;
                break;
            case 11:		// warp
                setwrp();
                break;
            case 12:		// status
                srscan(3);
                break;
            case 13:			// sensors
                sensor();
                break;
            case 14:			// orbit
                orbit();
                if (ididit) hitme = TRUE;
                break;
            case 15:			// transport "beam"
                beam();
                break;
            case 16:			// mine
                mine();
                if (ididit) hitme = TRUE;
                break;
            case 17:			// crystals
                usecrystals();
                break;
            case 18:			// shuttle
                shuttle();
                if (ididit) hitme = TRUE;
                break;
            case 19:			// Planet list
                preport();
                break;
            case 20:			// Status information
                srscan(2);
                break;
            case 21:			// Game Report
                report(0);
                break;
            case 22:			// use COMPUTER!
                eta();
                break;
            case 23:
                listCommands(TRUE);
                break;
            case 24:		// Emergency exit
                clearscreen();	// Hide screen
                freeze(TRUE);	// forced save
                exit(1);		// And quick exit
                break;
            case 25:
                probe();		// Launch probe
                break;
#ifdef CLOAKING
            case 26:
                cloak();        // turn on/off cloaking
                if (iscloaking) {
                    attack(2); // We will be seen while we cloak
                    iscloaking = FALSE;
                    iscloaked = TRUE;
                }
                break;
#endif
#ifdef CAPTURE
            case 27:
                capture();      // Attempt to get Klingon ship to surrender
                if (ididit) hitme = TRUE;
                break;
#endif
#ifdef SCORE
            case 28:
                score(1);    // get the score
                break;
#endif
            case 29:			// Abandon Ship
                abandn();
                break;
            case 30:			// Self Destruct
                dstrct();
                break;
            case 31:			// Save Game
                freeze(FALSE);
                if (skill > SGOOD)
                    prout("WARNING--Frozen games produce no plaques!");
                break;
            case 32:			// Try a desparation measure
                deathray();
                if (ididit) hitme = TRUE;
                break;
#ifdef DEBUG
            case 33:			// What do we want for debug???
                debugme();
                break;
#endif
            case 34:		// Call for help
                help();
                break;
            case 35:
                alldone = 1;	// quit the game
#ifdef DEBUG
                if (idebug) score(0);
#endif
                break;
            case 36:
                helpme();	// get help
                break;
        }
        for (;;) {
            if (alldone) break;		// Game has ended
#ifdef DEBUG
            if (idebug) prout("2500");
#endif
            if (Time != 0.0) {
                events();
                if (alldone) break;		// Events did us in
            }
            if (d.galaxy[quadx][quady] == 1000) { // Galaxy went Nova!
                atover(0);
                continue;
            }
            if (nenhere == 0) movetho();
            if (hitme && justin==0) {
                attack(2);
                if (alldone) break;
                if (d.galaxy[quadx][quady] == 1000) {	// went NOVA!
                    atover(0);
                    hitme = TRUE;
                    continue;
                }
            }
            break;
        }
        if (alldone) break;
    }
}

void cmb_start(){
    memset(citem, 0, sizeof(citem));
}

void cmb_add(int a){
    citem[strlen(citem)] = a;
    
}

void cmb_prompt(){
    //	while (TRUE) { /* Play a game */
    ////////    setup();
    //    if (strlen(citem)==0)return;
    switch (cmbQuestionIndex) {
        case 0:
        {
            
            alldone = gamewon = 0;
#ifdef DEBUG
            idebug = 0;
#endif
            //  Decide how many of everything
            //            if (choose()) return; // frozen game
            ////////////choose////////
            
            tourn = 0;
            thawed = 0;
            skill = 0;
            length = 0;
            if (fromcommandline) /* Can start with command line options */
                fromcommandline = 0;
            else
                proutn("Would you like a regular, tournament, or frozen game?");
            cmbQuestionIndex=1;
            return;
        }
            break;
        case 1:
        {
            
            if (isit("tournament")) {
                while (scan() == IHEOL) {
                    proutn("Type in tournament number-");
                }
                if (aaitem == 0) {
                    chew();
                    return;
                }
                tourn = (int)aaitem;
                thingx = -1;
                srand((unsigned int)(int)aaitem);
                break;
            }
            if (isit("frozen")) {
                thaw();
                chew();
                if (*passwd==0) return;
                randomize();
                Rand(); Rand(); Rand(); Rand();
                if (!alldone) thawed = 1; // No plaque if not finished
                report(1);
                return ;
            }
            if (isit("regular")) {
                skip(2);
                randomize();
                Rand(); Rand(); Rand(); Rand();
                break;
            }
            //            proutn("What is \"");
            //            proutn(citem);
            //            prout("\"?");
            char result[1000];
            sprintf(result, "What is \"%s\"?",citem);
            proutn(result);
            
            chew();
            return;
            
        }
            break;
        default:
            break;
    }
    
    
    
    //    }
    
    
    if (cmbQuestionIndex==1) {
        cmbQuestionIndex=2;
        cmb_start();
    }
    if (cmbQuestionIndex==2) {
        if (scan() == IHALPHA) {
            if (isit("short")) length = 1;
            else if (isit("medium")) length = 2;
            else if (isit("long")) length = 4;
            else if (isit("novice")) skill = SNOVICE;
            else if (isit("fair")) skill = SFAIR;
            else if (isit("good")) skill = SGOOD;
            else if (isit("expert")) skill = SEXPERT;
            else if (isit("emeritus")) skill = SEMERITUS;
            else {
                char result[1000];
                sprintf(result, "What is \"%s\"?",citem);
                proutn(result);
                
                //                proutn("What is \"");
                //                proutn(citem);
                //                prout("\"?");
            }
        }
        chew();
        if (length==0) proutn("Would you like a Short, Medium, or Long game? ");
        else if (skill == 0) proutn("Are you a Novice, Fair, Good, Expert, or Emeritus player?");
        
    }
    if (length==0 || skill==0) {
        return;
        
    }
    int i, hitme = 0;
    
    switch (cmbQuestionIndex) {
        case 2:
            cmbQuestionIndex=3;
            cmb_start();
            //return;
            // break;
        case 3:
        {
            strcpy(passwd, citem);
            chew();
            if (*passwd != 0) {
                cmbQuestionIndex=4;
            }else{
                proutn("Please type in a secret password (9 characters maximum)-");
                return;
            }
        }
        case 4:
        {
            
#ifdef DEBUG
            if (strcmp(passwd, "debug")==0) idebug = 1;
#endif
            
            // Use parameters to generate initial values of things
            damfac = 0.5 * skill;
            d.rembase = 3.0*Rand()+2.0;
            inbase = d.rembase;
            inplan = (PLNETMAX/2) + (PLNETMAX/2+1)*Rand();
            d.nromrem = (2.0+Rand())*skill;
            d.nscrem = (skill > SFAIR? 1 : 0);
            d.remtime = 7.0 * length;
            intime = d.remtime;
            d.remkl = 2.0*intime*((skill+1 - 2*Rand())*skill*0.1+.15); // d.remkl and inkling includes commanders and SC
            inkling = d.remkl;
            incom = skill + 0.0625*inkling*Rand();
            d.remcom= min(10, incom);
            incom = d.remcom;
            d.remres = (inkling+4*incom)*intime;
            inresor = d.remres;
            if (inkling > 50) {
                inbase = (d.rembase += 1);
            }
#ifdef CAPTURE
            brigcapacity = 400;
            brigfree = brigcapacity;
            kcaptured = 0; // TAA fix 6/2015
#endif
#ifdef CLOAKING
            ncviol = 0; // TAA fix 6/2015
            iscloaked = FALSE;
            iscloaking = FALSE;
#endif
            
            /////////////end choose//////////
            
            int i,j, krem, klumper;
            int ix, iy;
            
            // Prepare the Enterprise
            ship = IHE;
            energy = inenrg = 5000.0;
            shield = inshld = 2500.0;
            shldchg = shldup = 0;
            inlsr = 4.0;
            lsupres = 4.0;
            iran8(&quadx, &quady);
            iran10(&sectx, &secty);
            torps = intorps = 10;
            nprobes = (int)(3.0*Rand() + 2.0);	/* Give them 2-4 of these wonders */
            warpfac = 5.0;
            wfacsq = warpfac * warpfac;
            for (i=0; i <= ndevice; i++) damage[i] = 0.0;
            // Set up assorted game parameters
            batx = baty = 0;
            d.date = indate = 100.0*(int)(31.0*Rand()+20.0);
            d.killk = d.killc = nkinks = nhelp = resting = casual = d.nromkl = 0;
            // Added d.starkl=0 6/2015
            isatb = iscate = imine = icrystl = icraft = d.nsckill = d.nplankl = d.starkl = 0;
            iscraft = 1;
            landed = -1;
            alive = 1;
            docfac = 0.25;
            for (i = 1; i <= 8; i++)
                for (j = 1; j <= 8; j++) d.newstuf[i][j] = starch[i][j] = 0;
            // Initialize times for extraneous events
            future[FSNOVA] = d.date + expran(0.5 * intime);
            future[FTBEAM] = d.date + expran(1.5 * (intime / d.remcom));
            future[FSNAP] = d.date + 1.0 + Rand(); // Force an early snapshot
            future[FBATTAK] = d.date + expran(0.3*intime);
            future[FCDBAS] = 1e30;
            future[FSCMOVE] = d.nscrem ? d.date+0.2777 : 1e30;
            future[FSCDBAS] = 1e30;
            future[FDSPROB] = 1e30;
            // Starchart is functional
            stdamtim = 1e30;
            // Put stars in the galaxy
            instar = 0;
            for (i=1; i<=8; i++)
                for (j=1; j<=8; j++) {
                    int k = Rand()*9.0 + 1.0;
                    instar += k;
                    d.galaxy[i][j] = k;
                }
            // Locate star bases in galaxy
            for (i = 1; i <= inbase; i++) {
                int contflag;
                do {
                    do iran8(&ix, &iy);
                    while (d.galaxy[ix][iy] >= 10);
                    contflag = FALSE;
                    for (j = i-1; j > 0; j--) {
                        /* Improved placement algorithm to spread out bases */
                        double distq = square(ix-d.baseqx[j]) + square(iy-d.baseqy[j]);
                        if (distq < 6.0*(6-inbase) && Rand() < 0.75) {
                            contflag = TRUE;
#ifdef DEBUG
                            printf("DEBUG: Abandoning base #%d at %d-%d\n", i, ix, iy);
#endif
                            break;
                        }
#ifdef DEBUG
                        else if (distq < 6.0 * (6-inbase)) {
                            printf("DEBUG: saving base #%d, close to #%d\n", i, j);
                        }
#endif
                    }
                } while (contflag);
                
                d.baseqx[i] = ix;
                d.baseqy[i] = iy;
                starch[ix][iy] = -1;
                d.galaxy[ix][iy] += 10;
            }
            // Position ordinary Klingon Battle Cruisers
            krem = inkling - incom - d.nscrem;
            klumper = 0.25*skill*(9.0-length)+1.0;
            if (klumper > 9) klumper = 9; // Can't have more than 9 in quadrant
            do {
                double r = Rand();
                int klump = (1.0 - r*r)*klumper;
                if (klump > krem) klump = krem;
                krem -= klump;
                klump *= 100;
                do iran8(&ix, &iy);
                while (d.galaxy[ix][iy] + klump >= 1000);
                d.galaxy[ix][iy] += klump;
            } while (krem > 0);
            // Position Klingon Commander Ships
#ifdef DEBUG
            klumper = 1;
#endif
            for (i = 1; i <= incom; i++) {
                do {
                    do { /* IF debugging, put commanders by bases, always! */
#ifdef DEBUG
                        if (idebug && klumper <= inbase) {
                            ix = d.baseqx[klumper];
                            iy = d.baseqy[klumper];
                            klumper++;
                        }
                        else
#endif
                            iran8(&ix, &iy);
                    }
                    while ((d.galaxy[ix][iy] < 99 && Rand() < 0.75)||
                           d.galaxy[ix][iy]>899);
                    // check for duplicate
                    for (j = 1; j < i; j++)
                        if (d.cx[j]==ix && d.cy[j]==iy) break;
                } while (j < i);
                d.galaxy[ix][iy] += 100;
                d.cx[i] = ix;
                d.cy[i] = iy;
            }
            // Locate planets in galaxy
            for (i = 1; i <= inplan; i++) {
                do iran8(&ix, &iy);
                while (d.newstuf[ix][iy] > 0);
                d.newstuf[ix][iy] = 1;
                d.plnets[i].x = ix;
                d.plnets[i].y = iy;
                d.plnets[i].pclass = Rand()*3.0 + 1.0; // Planet class M N or O
                d.plnets[i].crystals = 1.5*Rand();		// 1 in 3 chance of crystals
                d.plnets[i].known = 0;
            }
            // Locate Romulans
            for (i = 1; i <= d.nromrem; i++) {
                iran8(&ix, &iy);
                d.newstuf[ix][iy] += 10;
            }
            // Locate the Super Commander
            if (d.nscrem > 0) {
                do iran8(&ix, &iy);
                while (d.galaxy[ix][iy] >= 900);
                d.isx = ix;
                d.isy = iy;
                d.galaxy[ix][iy] += 100;
            }
            // Place thing (in tournament game, thingx == -1, don't want one!)
            if (Rand() < 0.1 && thingx != -1) {
                iran8(&thingx, &thingy);
            }
            else {
                thingx = thingy = 0;
            }
            
            //	idate = date;
            skip(3);
            d.snap = 0;
            char result[10000];
            if (skill == SNOVICE) {
                sprintf(result,"It is stardate %d. The Federation is being attacked by\n",
                        (int)d.date);
                proutn(result);
                sprintf(result,"a deadly Klingon invasion force. As captain of the United\n"
                        "Starship U.S.S. Enterprise, it is your mission to seek out\n"
                        "and destroy this invasion force of %d battle cruisers.\n",
                        inkling);
                proutn(result);
                
                sprintf(result,"You have an initial allotment of %d stardates to complete\n"
                        "your mission.  As you proceed you may be given more time.\n\n"
                        "You will have %d supporting starbases.\n"
                        "Starbase locations-  ",
                        (int)intime, inbase);
                proutn(result);
                
            }
            else {
                sprintf(result,"Stardate %d.\n\n"
                        "%d Klingons,\nan unknown number of Romulans\n",
                        (int)d.date, inkling);
                prout(result);
                
                if (d.nscrem){ sprintf(result,"and one (GULP) Super-Commander.\n"); prout(result);}
                sprintf(result,"%d stardates\n%d starbases in  ",(int)intime, inbase);
                prout(result);
            }
            for (i = 1; i <= inbase; i++) {
                cramlc(0, d.baseqx[i], d.baseqy[i]);
                if (i < inbase) proutn("  ");
            }
            skip(2);
            proutn("The Enterprise is currently in");
            cramlc(1, quadx, quady);
            proutn(" ");
            cramlc(2, sectx, secty);
            skip(2);
            prout("Good Luck!");
            if (d.nscrem) proutn("  YOU'LL NEED IT.");
            skip(1);
            newqad(0);
            if (nenhere) shldup=1.0;
            if (neutz) attack(0);	// bad luck to start in a Romulan Neutral Zone
            
            
            /////////end setup
            
            cmbQuestionIndex=5;
        }
        case 5:
        {
            
            
            if (alldone) {
                score(0);
                alldone = 0;
                cmbQuestionIndex=9;
                break;
            }
            else {
                cmbQuestionIndex=6;
                return;
            }
        }
            
            
        case 6:
        {
            /////// makemoves();
            hitme = FALSE;
            justin = 0;
            Time = 0.0;
            i = -1;
            chew();
            skip(1);
            proutn("COMMAND> ");
            cmbQuestionIndex=7;
            return;
            ////////end makemoves();
        }
        case 7:{
            
            if (scan() == IHEOL) return;
            for (i=0; i < 29; i++) // Abbreviations allowed for the first 29 commands, only.
                if (isit(commands[i]))
                    break;
            if (i < 29) break;
            for (; i < NUMCOMMANDS; i++)
                if (strcmp(commands[i], citem) == 0) break;
            if (!(i < NUMCOMMANDS
#ifndef CLOAKING
                  && i != 26 // ignore the CLOAK command
#endif
#ifndef CAPTURE
                  && i != 27 // ignore the CAPTURE command
#endif
#ifndef SCORE
                  && i != 28 // ignore the SCORE command
#endif
#ifndef DEBUG
                  && i != 33 // ignore the DEBUG command
#endif
                  )){
                if (skill <= SFAIR)  {
                    prout("UNRECOGNIZED COMMAND. LEGAL COMMANDS ARE:");
                    listCommands(TRUE);
                }
                else prout("UNRECOGNIZED COMMAND.");
                return;
            }
            cmbQuestionIndex=8;
        }
            
        case 8:
        {
            
            
            switch (i) { /* command switch */
                case 0:			// srscan
                    srscan(1);
                    break;
                case 1:			// lrscan
                    lrscan();
                    break;
                case 2:			// phasers
                    phasers();
                    if (ididit) {
#ifdef CLOAKING
                        if (irhere && d.date >= ALGERON && !isviolreported) {
                            prout("The Romulan ship discovers you are breaking the Treaty of Algeron!");
                            ncviol++;
                            isviolreported = TRUE;
                        }
#endif
                        hitme = TRUE;
                    }
                    break;
                case 3:			// photons
                    photon();
                    if (ididit) {
#ifdef CLOAKING
                        if (irhere && d.date >= ALGERON && !isviolreported) {
                            prout("The Romulan ship discovers you are breaking the Treaty of Algeron!");
                            ncviol++;
                            isviolreported = TRUE;
                        }
#endif
                        hitme = TRUE;
                    }
                    break;
                case 4:			// move
                    warp(1);
                    break;
                case 5:			// shields
                    sheild(1);
                    if (ididit) {
                        attack(2);
                        shldchg = 0;
                    }
                    break;
                case 6:			// dock
                    dock();
                    break;
                case 7:			// damages
                    dreprt();
                    break;
                case 8:			// chart
                    chart(0);
                    break;
                case 9:			// impulse
                    impuls();
                    break;
                case 10:		// rest
                    waiting();
                    if (ididit) hitme = TRUE;
                    break;
                case 11:		// warp
                    setwrp();
                    break;
                case 12:		// status
                    srscan(3);
                    break;
                case 13:			// sensors
                    sensor();
                    break;
                case 14:			// orbit
                    orbit();
                    if (ididit) hitme = TRUE;
                    break;
                case 15:			// transport "beam"
                    beam();
                    break;
                case 16:			// mine
                    mine();
                    if (ididit) hitme = TRUE;
                    break;
                case 17:			// crystals
                    usecrystals();
                    break;
                case 18:			// shuttle
                    shuttle();
                    if (ididit) hitme = TRUE;
                    break;
                case 19:			// Planet list
                    preport();
                    break;
                case 20:			// Status information
                    srscan(2);
                    break;
                case 21:			// Game Report
                    report(0);
                    break;
                case 22:			// use COMPUTER!
                    eta();
                    break;
                case 23:
                    listCommands(TRUE);
                    break;
                case 24:		// Emergency exit
                    clearscreen();	// Hide screen
                    freeze(TRUE);	// forced save
                    exit(1);		// And quick exit
                    break;
                case 25:
                    probe();		// Launch probe
                    break;
#ifdef CLOAKING
                case 26:
                    cloak();        // turn on/off cloaking
                    if (iscloaking) {
                        attack(2); // We will be seen while we cloak
                        iscloaking = FALSE;
                        iscloaked = TRUE;
                    }
                    break;
#endif
#ifdef CAPTURE
                case 27:
                    capture();      // Attempt to get Klingon ship to surrender
                    if (ididit) hitme = TRUE;
                    break;
#endif
#ifdef SCORE
                case 28:
                    score(1);    // get the score
                    break;
#endif
                case 29:			// Abandon Ship
                    abandn();
                    break;
                case 30:			// Self Destruct
                    dstrct();
                    break;
                case 31:			// Save Game
                    freeze(FALSE);
                    if (skill > SGOOD)
                        prout("WARNING--Frozen games produce no plaques!");
                    break;
                case 32:			// Try a desparation measure
                    deathray();
                    if (ididit) hitme = TRUE;
                    break;
#ifdef DEBUG
                case 33:			// What do we want for debug???
                    debugme();
                    break;
#endif
                case 34:		// Call for help
                    help();
                    break;
                case 35:
                    alldone = 1;	// quit the game
#ifdef DEBUG
                    if (idebug) score(0);
#endif
                    break;
                case 36:
                    helpme();	// get help
                    break;
            }
            for (;;) {
                if (alldone) break;		// Game has ended
#ifdef DEBUG
                if (idebug) prout("2500");
#endif
                if (Time != 0.0) {
                    events();
                    if (alldone) break;		// Events did us in
                }
                if (d.galaxy[quadx][quady] == 1000) { // Galaxy went Nova!
                    atover(0);
                    continue;
                }
                if (nenhere == 0) movetho();
                if (hitme && justin==0) {
                    attack(2);
                    if (alldone) break;
                    if (d.galaxy[quadx][quady] == 1000) {	// went NOVA!
                        atover(0);
                        hitme = TRUE;
                        continue;
                    }
                }
                break;
            }
            if (!alldone){
                cmbQuestionIndex=6;
                return;
                
            }
            
            skip(2);
            stars();
            skip(1);
            cmbQuestionIndex=9;
        }
            
        case 9:{
            
            if (tourn && alldone) {
                printf("Do you want your score recorded?");
                switch (ja()) {
                    case 1:
                        chew2();
                        freeze(FALSE);
                        break;
                    case -1:
                        return;
                    default:
                        break;
                }
                cmbQuestionIndex=10;
                printf("Do you want to play again?");
            }
        }
        case 10:{
            switch (ja()) {
                case 1:
                    cmbQuestionIndex=0;
                    return;
                    break;
                    
                default:
                    break;
            }
            skip(1);
            prout("May the Great Bird of the Galaxy roost upon your home planet.");
        }
            
            
        default:
            break;
    }
    
    
    
}

int mainer(long argc, char *argv[]) {
    cmbResult="\0";
    cmbQuestionIndex=0;
    
    int i;
    prelim();
    sizeOfReturnBuffer = INITIAL_SIZE_OF_RETURNBUFFER*sizeof(char);
    if ((tReturnString = malloc(sizeOfReturnBuffer)) == 0) {
        // "Sorry we are out of memory. Please close other apps and try again!"
        return ENOMEM;
    }
    tReturnString[0] = 0;
    curPosOutBuffer = 0;
    
    if (argc > 1) { // look for -f option
        if (strcmp(argv[1], "-f")== 0) {
            coordfixed = 1;
            argc--;
            argv++;
        }
    }
    
    
    if (argc > 1) {
        fromcommandline = 1;
        line[0] = '\0';
        for (i = 0; i < argc; i++) printf("%s ", argv[i]);
        tprntNewLine();
        
    }
    else fromcommandline = 0;
    
    ///////////////////////////////
    
    alldone = gamewon = 0;
#ifdef DEBUG
    idebug = 0;
#endif
    //  Decide how many of everything
    //            if (choose()) return; // frozen game
    ////////////choose////////
    
    tourn = 0;
    thawed = 0;
    skill = 0;
    length = 0;
    if (fromcommandline) /* Can start with command line options */
        fromcommandline = 0;
    else
        proutn("Would you like a regular, tournament, or frozen game?");
    cmbQuestionIndex=1;
    //    //	while (TRUE) { /* Play a game */
    //    setup();
    //    if (alldone) {
    //        score(0);
    //        alldone = 0;
    //    }
    //    else makemoves();
    //    skip(2);
    //    stars();
    //    skip(1);
    //
    //    if (tourn && alldone) {
    //        printf("Do you want your score recorded?");
    //        if (ja()) {
    //            chew2();
    //            freeze(FALSE);
    //        }
    //    }
    //    printf("Do you want to play again?");
    //    //		if (!ja()) break;
    //    //	}
    //    skip(1);
    //    prout("May the Great Bird of the Galaxy roost upon your home planet.");
    return 0;
}

void cmb_main(){
    char* parameter[]={"",""};
    
    mainer(0, parameter);
}

int cmb_check(){
    return strlen(cmbResult);
}

void cramen(int i) {
    /* return an enemy */
    char *s;
    
    switch (i) {
        case IHR: s = "Romulan"; break;
        case IHK: s = "Klingon"; break;
        case IHC: s = "Commander"; break;
        case IHS: s = "Super-commander"; break;
        case IHSTAR: s = "Star"; break;
        case IHP: s = "Planet"; break;
        case IHB: s = "Starbase"; break;
        case IHBLANK: s = "Black hole"; break;
        case IHT: s = "Tholean"; break;
        case IHWEB: s = "Tholean web"; break;
        default: s = "Unknown??"; break;
    }
    proutn(s);
}

void cramlc(int key, int x, int y) {
    if (key == 1) proutn(" Quadrant");
    else if (key == 2) proutn(" Sector");
    proutn(" ");
    crami(x, 1);
    proutn(" - ");
    crami(y, 1);
}

void crmena(int i, int enemy, int key, int x, int y) {
    if (i == 1) proutn("***");
    cramen(enemy);
    proutn(" at");
    cramlc(key, x, y);
}

void crmshp(void) {
    char *s;
    switch (ship) {
        case IHE: s = "Enterprise"; break;
        case IHF: s = "Faerie Queene"; break;
        default:  s = "Ship???"; break;
    }
    proutn(s);
}

void stars(void) {
    prouts("******************************************************");
    skip(1);
}

double expran(double avrage) {
    return -avrage*log(1e-7 + Rand());
}

double Rand(void) {
    return rand()/(1.0 + (double)RAND_MAX);
}

void iran8(int *i, int *j) {
    *i = Rand()*8.0 + 1.0;
    *j = Rand()*8.0 + 1.0;
}

void iran10(int *i, int *j) {
    *i = Rand()*10.0 + 1.0;
    *j = Rand()*10.0 + 1.0;
}

void chew(void) {
    linecount = 0;
    linep = line;
    *linep = 0;
}

void chew2(void) {
    /* return IHEOL next time */
    linecount = 0;
    linep = line+1;
    *linep = 0;
}

int scan(void) {
    int i;
    char *cp;
    
    linecount = 0;
    
    // Init result
    aaitem = 0.0;
    //	*citem = 0;
    strcpy(linep, citem);
    // Read a line if nothing here
    if (*linep == 0) {
        if (linep != line) {
            chew();
            return IHEOL;
        }
        //		gets(line);
        // We should really be using fgets
        strncpy(line,sizeof(line),UserInput);
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        linep = line;
    }
    // Skip leading white space
    while (*linep == ' ') linep++;
    // Nothing left
    if (*linep == 0) {
        chew();
        return IHEOL;
    }
    if (isdigit(*linep) || *linep=='+' || *linep=='-' || *linep=='.') {
        // treat as a number
        if (sscanf(linep, "%lf%n", &aaitem, &i) < 1) {
            linep = line; // Invalid numbers are ignored
            *linep = 0;
            return IHEOL;
        }
        else {
            // skip to end
            linep += i;
            return IHREAL;
        }
    }
    // Treat as alpha
    cp = citem;
    while (*linep && *linep!=' ') {
        if ((cp - citem) < 9) *cp++ = tolower(*linep);
        linep++;
    }
    *cp = 0;
    return IHALPHA;
}

int ja(void) {
    chew();
    scan();
    chew();
    if (*citem == 'y') return 1;
    if (*citem == 'n') return 0;
    proutn("Please answer with \"Y\" or \"N\":");
    return -1;
}

void cramf(double x, int w, int d) {
    char buf[64];
    sprintf(buf, "%*.*f", w, d, x);
    proutn(buf);
}

void crami(int i, int w) {
    char buf[16];
    sprintf(buf, "%*d", w, i);
    proutn(buf);
}

double square(double i) { return i*i; }

static void clearscreen(void) {
    /* Somehow we need to clear the screen */
    proutn("\033[2J\033[0;0H");	/* Hope for an ANSI display */
}

/* We will pull these out in case we want to do something special later */

void pauser(int i) {
#ifdef CLOAKING
    if (iscloaked) return;
#endif
    putchar('\n');
    if (i==1) {
        if (skill > SFAIR)
            prout("[ANNOUNCEMENT ARRIVING...]");
        else
            prout("[IMPORTANT ANNOUNCEMENT ARRIVING -- HIT SPACE BAR TO CONTINUE]");
        getch();
    }
    else {
        if (skill > SFAIR)
            proutn("[CONTINUE?]");
        else
            proutn("[HIT SPACE BAR TO CONTINUE]");
        getch();
        proutn("\r                           \r");
    }
    if (i != 0) {
        clearscreen();
    }
    linecount = 0;
}


void skip(int i) {
    while (i-- > 0) {
        linecount++;
        if (linecount >= 23)
            pauser(0);
        else
            putchar('\n');
    }
}


void proutn(char *s) {
    //fputs(s, stdout);
    
    if (strlen(cmbResult)>0) {
        char * str3 = (char *) malloc(2 + strlen(cmbResult)+ strlen(s) );
        strcpy(str3, cmbResult);
        strcat(str3, "\n");
        strcat(str3, s);
        cmbResult=str3;
        
    }else{
        char * str3 = (char *) malloc(1 + strlen(s) );
        strcpy(str3, s);
        cmbResult=str3;
        
    }
    
    
}

void clear_cmb_result(){
    cmbResult="\0";
}

void prout(char *s) {
    proutn(s);
    skip(1);
}

void prouts(char *s) {
    clock_t endTime;
    /* print slowly! */
    while (*s) {
        endTime = clock() + CLOCKS_PER_SEC*0.05;
        while (clock() < endTime) ;
        putchar(*s++);
        fflush(stdout);
    }
}

void huh(void) {
    chew();
    skip(1);
    prout("Beg your pardon, Captain?");
}

int isit(char *s) {
    /* New function -- compares s to scaned citem and returns true if it
     matches to the length of s */
    
    return strncmp(s, citem, max(1, strlen(citem))) == 0;
    
}

#ifdef DEBUG
void debugme(void) {
    proutn("Reset levels? ");
    if (ja() != 0) {
        if (energy < inenrg) energy = inenrg;
        shield = inshld;
        torps = intorps;
        lsupres = inlsr;
    }
    proutn("Reset damage? ");
    if (ja() != 0) {
        int i;
        for (i=0; i <= ndevice; i++) if (damage[i] > 0.0) damage[i] = 0.0;
        stdamtim = 1e30;
    }
    proutn("Toggle idebug? ");
    if (ja() != 0) {
        idebug = !idebug;
        if (idebug) prout("Debug output ON");
        else prout("Debug output OFF");
    }
    proutn("Cause selective damage? ");
    if (ja() != 0) {
        int i, key;
        for (i=1; i <= ndevice; i++) {
            proutn("Kill ");
            proutn(device[i]);
            proutn("? ");
            chew();
            key = scan();
            if (key == IHALPHA &&  isit("y")) {
                damage[i] = 10.0;
                if (i == DRADIO) stdamtim = d.date;
            }
        }
    }
    proutn("Examine/change events? ");
    if (ja() != 0) {
        int i;
        for (i = 1; i < NEVENTS; i++) {
            int key;
            if (future[i] == 1e30) continue;
            switch (i) {
                case FSNOVA:  proutn("Supernova       "); break;
                case FTBEAM:  proutn("T Beam          "); break;
                case FSNAP:   proutn("Snapshot        "); break;
                case FBATTAK: proutn("Base Attack     "); break;
                case FCDBAS:  proutn("Base Destroy    "); break;
                case FSCMOVE: proutn("SC Move         "); break;
                case FSCDBAS: proutn("SC Base Destroy "); break;
            }
            cramf(future[i]-d.date, 8, 2);
            chew();
            proutn("  ?");
            key = scan();
            if (key == IHREAL) {
                future[i] = d.date + aaitem;
            }
        }
        chew();
    }
    proutn("Make universe visible? ");
    if (ja() != 0) {
        int i, j;
        for (i = 1; i < 9; i++)
        {
            for (j = 1; j < 9; j++)
            {
                starch[i][j] = 1;
            }
        }
    }
}


#endif
