/*
 *  F-News by Sascha 'SieGeL' Pfalz of (tRSi-iNNOVATiONs)
 *
 *  Displays News for a given date (-range) much like Hydra-News
 */

#include <exec/exec.h>
#include <Fame/fameDoorProto.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/date_pragmas.h>
#include <libraries/date.h>
#include "global_defines.h"
#include "struct.h"
#include "proto.h"

STATIC char *ver=VERSTAG" ["CPU_TYPE"]\0";

extern char *_ProgramName;

/*
 * --- Prototypes ----
 */

void __autoopenfail(void) { _XCEXIT(0);}	// Dummy function for SAS
long 	main(void);
void CloseLibs(void);
void wb(char *err);

/*
 *  --- Main Entry Point ---
 */

long main(void)
	{
	struct 	RDArgs *rda=NULL;
	long    ArgArray[1]={0L};
	char		TitleBuf[202],*retbuf;


	if(rda=ReadArgs("NODE-NR./A/N",ArgArray,rda))
		{
		node=*(LONG *)ArgArray[0];
		SPrintf((STRPTR)FAMEDoorPort,"FAMEDoorPort%ld",node);
		FreeArgs(rda);rda=NULL;
		}
	else
		{
		FreeArgs(rda);
		PrintFault(IoErr(),(FilePart(_ProgramName)));
		Printf("\n%s is a FAME BBS-Door and is only usable via the BBS !\n\n",(FilePart(_ProgramName)));
		exit(0);
		}
	if(!(UtilityBase=(struct Library *) OpenLibrary("utility.library",37L))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);exit(20);}
	if(!(FAMEBase=(struct FAMELibrary *) 	OpenLibrary(FAMENAME,0))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);CloseLibs();}
	if(!(LocaleBase=(struct LocaleBase *)OpenLibrary("locale.library",38L))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);CloseLibs();}
	if(!(DateBase=OpenLibrary(DATE_NAME,0L))) { SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);CloseLibs();}
	if(!(readbuffer = (char *)AllocVec(READBUF,MEMF_CLEAR|MEMF_PUBLIC))) { SetIoErr(ERROR_NO_FREE_STORE);CloseLibs();}
	if(FIMStart(0,0UL)) CloseLibs();
	PutString("\f\n\r",1);
	SPrintf(TitleBuf,"F-News [mv%ld.%ld [36mby SieGeL [m([36mtRSi[m-[36miNNOVATiONs[m)",VERSION,REVISION);
  Center(TitleBuf,1,35);
	PutString("\n\n\r[36mChecking for news[34m.",0);
	if(retbuf=ReadConfig())
		{
    PutStringFormat("\n\n\r[37mError reading config: %s[0m\r\n",retbuf);
		wb("");
		}
	else PutString(".",0);
	ReadAndParseNews();
	wb("");
	}


/*
 *  --- Termination Routines ---
 */

void wb(char *err)
	{
	if(*err) NC_PutString(err,1);
	PutStringFormat("[m\n\r");
	FIMEnd(0);
	}

void ShutDown(long error)
	{
	CloseLibs();
	}

void CloseLibs(void)
	{
	if(UtilityBase)		CloseLibrary(UtilityBase); UtilityBase=NULL;
	if(FAMEBase) 			CloseLibrary((struct Library *)FAMEBase); FAMEBase=NULL;
  if(LocaleBase)
		{
		CloseLibrary((struct Library *)LocaleBase);LocaleBase=NULL;
		};
	if(DateBase)			CloseLibrary(DateBase); DateBase=NULL;
	if(readbuffer)		FreeVec(readbuffer); readbuffer = NULL;
	exit(0);
	}
