/*
 *  Function checks for FAME:ExternEnv/Doors/F-News.cfg and parses it to configure F-News.
 */

#include <exec/exec.h>
#include <Fame/fameDoorProto.h>
#include <pragmas/utility_pragmas.h>
#include "global_defines.h"
#include "struct_ex.h"
#include "proto.h"

char *ReadConfig(void);

char *ConfigStrings[]={"DATADIR=",NULL};

#define MAXENTRIES		1												// AMount of configuration options
#define CFG_DATAFILE	0

char *ReadConfig(void)
	{
	BPTR	cfgptr=NULL;
	char	buffer[202];
	long	count=0;

	if(!(cfgptr=Open("FAME:ExternEnv/Doors/F-News.cfg",MODE_OLDFILE)))
		{
		return("Configfile not found!\n\n\rMake sure that FAME:ExternEnv/Doors/F-News.cfg really exists!!!");
		}
	while(FGets(cfgptr,buffer,255))
		{
		count = NULL;
		while(count < MAXENTRIES)
			{
			if(Strnicmp(ConfigStrings[count],buffer,strlen(ConfigStrings[count])))
				{
				buffer[strlen(buffer)-1]='\0';
				switch(count)
					{
					case	CFG_DATAFILE:		FAMEStrMid(buffer,config.Datafile,strlen(ConfigStrings[count])+2,-1);
																break;
					}
				}
			count++;
			}
		}
	Close(cfgptr);cfgptr=NULL;
	return(NULL);
	}
