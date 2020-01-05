/*
 *  --- Global Vars used by F-News.FIM ---
 */

/* System structures */

struct 	FAMELibrary		*FAMEBase=NULL;
struct 	Library 			*UtilityBase=NULL;
struct  Library       *DateBase=NULL;
struct	LocaleBase		*LocaleBase=NULL;


long 	node=NULL;								// Current Nodenumber

/*
 *  Internal config structure
 */

struct Config
	{
	char	Datafile[256];
	}config;


/*
 *  Memory pointer for readbuffer, currently set to 1k (V1.2)
 */

char *readbuffer=NULL;
