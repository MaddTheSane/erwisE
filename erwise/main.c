#include "Includes.h"



int main(int argc, char *argv[])
{
    FILE *fp;

    ConfigInit();

    if ((fp = OpenErwiserc("r"))) {
	ConfigRestore(fp);
	fclose(fp);
    }
    if (UiInitialize(argc, argv, ConfigGetValue, ConfigSetValue) != UI_OK) {
	DisplayFatal("Error initializing Ui-toolkit");
    }

    AttachCallbacks();
    BindKeys();
    BindVariables();

    UiMainLoop();
    return EXIT_SUCCESS;
}

#define RCPOSTFIX ".old"
FILE *
OpenErwiserc(char *type)
{
    char *tmpstr;
    char *bustr;
    FILE *fp;
    char *homeEnv;
    homeEnv = getenv("HOME");

    if (homeEnv == NULL)
	return NULL;

    tmpstr = strdup(homeEnv);

    tmpstr = (char *) ReAlloc((void *) tmpstr, strlen(tmpstr) +
			      strlen(ERWISERC) + 2);
    strcat(tmpstr, "/");
    strcat(tmpstr, ERWISERC);
    if (!strcmp(type, "w")) {
	bustr = (char *) Malloc((strlen(tmpstr) + strlen(RCPOSTFIX) + 1) *
				sizeof(char));
	strcpy(bustr, tmpstr);
	strcat(bustr, RCPOSTFIX);
	rename(tmpstr, bustr);
	free(bustr);
    }
    fp = fopen(tmpstr, type);
    Free(tmpstr);

    return fp;
}


#ifdef mips
char *
 strdup(s)
char *s;
{
    char *p = (char *) malloc(strlen(s) + 1);

    strcpy(p, s);

    return p;
}

#endif
