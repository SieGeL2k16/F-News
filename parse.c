/*
 *  Functions to parse and display any matching news inside config.Datafile
 */

#include <exec/exec.h>
#include <Fame/fameDoorProto.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <dos/stdio.h>
#include <dos/datetime.h>
#include <proto/date.h>
#include <libraries/date.h>
#include "global_defines.h"
#include "struct_ex.h"
#include "proto.h"

void ReadAndParseNews(void);

STATIC BOOL CalculateRange(char *start, char *end);
STATIC BOOL ConvertDateStringToNumber(char *datestr, unsigned short *day, unsigned short *month, long *year,unsigned short currday,unsigned short currmonth, long curryear);

/*
 *  Variables
 */

struct  ComparedDates								// Structure used by date_Compare2Dates() function
	{
  unsigned short currday,
                 startday,
                 endday,
                 currmonth,
                 startmonth,
                 endmonth;
  long           curryear,
                 startyear,
                 endyear;
	}compareddates;

/*
 *  ReadAndParseNews() scans the given Datafile. Also validates the dates read and if any
 *  of the date range is valid to display the news, it will be shown here.
 *  Supported Tags: @START, @END
 */

void ReadAndParseNews(void)
	{
  static char *wrongdate="[37mFAILED!!![m\n\n\r[36mWrong Dateformat found: [35m%s!!![m\n\n\r";
	BPTR  fptr=NULL;
	char	errbuf[256],startdate[12],enddate[12],currdate[LEN_DATSTRING];
	BOOL 	done=FALSE,first=FALSE;
	struct DateStamp *myds=NULL;
	struct DateTime  *mydt=NULL;

	PutString("[34m.",0);
	if(!(myds = AllocVec(sizeof(struct DateStamp),MEMF_CLEAR|MEMF_PUBLIC))) wb("[37mFAILED!\n\n\rUnable to allocate memory for date structs!!!\n\r");
	if(!(mydt = AllocVec(sizeof(struct DateTime),MEMF_CLEAR|MEMF_PUBLIC)))
		{
		FreeVec(myds);
		wb("[37mFAILED!\n\n\rUnable to allocate memory for date structs!!!\n\r");
    }
  myds = DateStamp(myds);
  mydt->dat_Stamp = *myds;
  mydt->dat_Format = FORMAT_USA;
	mydt->dat_StrDate=currdate;
	DateToStr(mydt);
	FreeVec(myds);myds=NULL;
	FreeVec(mydt);mydt=NULL;
	memset(&compareddates,0,sizeof(struct ComparedDates));
  if(ConvertDateStringToNumber(currdate,&compareddates.currday,&compareddates.currmonth,&compareddates.curryear,0,0,0)==FALSE)
		{
		PutStringFormat(wrongdate,currdate);
		return;
		}
	if(!(fptr = Open(config.Datafile,MODE_OLDFILE)))
		{
		PutStringFormat("[37mFAILED!\n\n\rUnable to open %s, please check config![m\n\r",config.Datafile);
		return;
    }
	SetVBuf(fptr,NULL,BUF_FULL,4096);
	*readbuffer = '\0';
	PutString("[34m.",0);
	while(FGets(fptr,readbuffer,256))
		{
    done=FALSE;
		if(!Strnicmp(readbuffer,"@START",5))							// Start-Tag found, read next 2 lines to get start/end date
			{
			if(!(FGets(fptr,readbuffer,256)))
				{
				Fault(IoErr(),"ParseNews",errbuf,256);
				Close(fptr);
				PutStringFormat("[37mFAILED!\n\n\r%s[m\n\r",errbuf);
				return;
				}
			readbuffer[strlen(readbuffer)-1]='\0';
			FAMEStrMid(readbuffer,startdate,1,10);
			if(strlen(startdate)<10)
				{
				PutStringFormat(wrongdate,startdate);
				Close(fptr);
				return;
				}
      startdate[11]='\0';
		  if(ConvertDateStringToNumber(startdate,&compareddates.startday,&compareddates.startmonth,&compareddates.startyear,compareddates.currday,compareddates.currmonth,compareddates.curryear)==FALSE)
				{
				PutStringFormat(wrongdate,startdate);
				return;
				}
			if(!(FGets(fptr,readbuffer,256)))                // Read End-Date and calculate range etc.
				{
				Fault(IoErr(),"ParseNews",errbuf,256);
				Close(fptr);
				PutStringFormat("[37mFAILED!\n\n\r%s[m\n\r",errbuf);
				return;
				}
			readbuffer[strlen(readbuffer)-1]='\0';
			FAMEStrMid(readbuffer,enddate,1,10);
			if(strlen(enddate)<10)
				{
				PutStringFormat(wrongdate,enddate);
				Close(fptr);
				return;
				}
      enddate[11]='\0';
		  if(ConvertDateStringToNumber(enddate,&compareddates.endday,&compareddates.endmonth,&compareddates.endyear,compareddates.currday,compareddates.currmonth,compareddates.curryear)==FALSE)
				{
				PutStringFormat(wrongdate,enddate);
				return;
				}
			if(CalculateRange(startdate,enddate)==FALSE) continue;
      if(first==FALSE)
				{
				PutStringFormat("[32mfound !\r\n\n[m");
				first=TRUE;
				}
      while(FGets(fptr,readbuffer,256))
				{
				if(!Strnicmp(readbuffer,"@END",4))
					{
					done = TRUE;
          break;
          }
  			else
					{
					PutString(readbuffer,0);
					}
				if(done == TRUE) break;
				}
			}
		}
	Close(fptr);
	if(first==FALSE) PutStringFormat("[34mno news found!\r\n\n[m");
	}

/*
 *  This function checks all required dates, get the current date and
 *  performs additional error checking. Also '*' will be replaced to
 *  get valid dates.
 */

STATIC BOOL CalculateRange(char *start, char *end)
	{
	short startcode=0,endcode=0;

//  PutStringFormat("\nCurrent date: DAY: %ld - MONTH: %ld - YEAR: %ld\n",compareddates.currday,compareddates.currmonth,compareddates.curryear);
//  PutStringFormat("  Start date: DAY: %ld - MONTH: %ld - YEAR: %ld\n",compareddates.startday,compareddates.startmonth,compareddates.startyear);
//  PutStringFormat("    End date: DAY: %ld - MONTH: %ld - YEAR: %ld\n",compareddates.endday,compareddates.endmonth,compareddates.endyear);

	startcode = date_Compare2Dates(compareddates.currday,compareddates.currmonth,compareddates.curryear,compareddates.startday,compareddates.startmonth,compareddates.startyear);
	endcode   = date_Compare2Dates(compareddates.currday,compareddates.currmonth,compareddates.curryear,compareddates.endday,compareddates.endmonth,compareddates.endyear);

	if(startcode>=0 && endcode<=0) return(TRUE);
	else return(FALSE);
	}

/*
 *  Function to parse an AmigaDOS Datestring in FORMAT_USA into three integer vars. Required
 *  to work with date.library functions. Also extends year to 4-digits according to AmigaOS
 *  conventions: <=70 => 2000, >70 => 1900. You have to pass all integers as pointers as this
 *  function directly modifies the passed vars.
 *
 *  DateStr Format: MM-DD-YY or MM-DD-YYYY
 *  Also supported are Wildcars for MM, DD or YY(YY). In this case, the current date will be
 *  used in place of any matching '*'. You can pass 0 for current date, but you cannot
 *  use then any of the wildcards of course!
 */

STATIC BOOL ConvertDateStringToNumber(char *datestr, unsigned short *day, unsigned short *month, long *year,unsigned short currday,unsigned short currmonth, long curryear)
	{
	char	dbuf[4],mbuf[4],ybuf[6];
	long	testy=0;

  if(datestr[2]!='-' || datestr[5]!='-') return(FALSE);				// Wrong date!
	FAMEStrMid(datestr,mbuf,1,2);
	FAMEStrMid(datestr,dbuf,4,2);
	FAMEStrMid(datestr,ybuf,7,-1);
  if(FAMEStrChr(dbuf,'*')) (*day) = currday;									// Wildcard found, using current day
	else (*day) = atoi(dbuf);
	if(FAMEStrChr(mbuf,'*')) (*month) = currmonth;              // Wildcard found, using current month
	else (*month) = atoi(mbuf);
	if(FAMEStrChr(ybuf,'*')) (*year) = curryear;								// Wildcard found, using current year
	else
		{
 		testy=atol(ybuf);
  	if(strlen(ybuf)==2)																				// AmigaDOS Date found (2-Digit year)
			{
			if(testy<=70) testy+=2000;                             	// Below or equal 70, must be 2K
    	else testy+=1900;																				// Greater than 70, must be 1900+
			}
  	(*year) = testy;
    }
	return(TRUE);
	}
