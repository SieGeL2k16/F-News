/*
 *  --- Global Vars used by F-News.FIM ---
 */

/* System structures */

extern struct 	FAMELibrary		*FAMEBase;
extern struct 	Library 			*UtilityBase;
extern struct   Library       *DateBase;
extern struct   Library       *LocaleBase;

extern long 		node;

/*
 *  Internal config structure
 */

extern struct Config
	{
	char	Datafile[256];
	}config;

/*
 *  Memory pointer for readbuffer, currently set to 1k (V1.2)
 */

extern char *readbuffer;
