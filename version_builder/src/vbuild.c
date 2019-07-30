#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char * getTimeStamp()
{
    time_t          tm;
    struct tm *     localTime;
    static char     szTimeStr[32];

    tm = time(0);
    localTime = localtime(&tm);

    sprintf(
            szTimeStr,
            "%d-%02d-%02d %02d:%02d:%02d",
            localTime->tm_year + 1900,
            localTime->tm_mon + 1,
            localTime->tm_mday,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec);

    return szTimeStr;
}

int main(int argc, char * argv[])
{
    int         i;
    char        szVersionFileName[256];
    char        szTemplateFileName[256];
    char        szMajorVersion[32];
    char        szMinorVersion[32];
    char        szVersionStr[128];
    char *      pszTemplate;
    FILE *      fptrVersion;
    FILE *      fptrTemplate;
    long        templateBytes = 0L;
    int         searching = 1;

    if (argc > 3) {
		for (i = 1;i < argc;i++) {
			if (argv[i][0] == '-') {
				if (strncmp(&argv[i][1], "o", 4) == 0) {
                    strncpy(szVersionFileName, &argv[++i][0], sizeof(szVersionFileName));
				}
				else if (strncmp(&argv[i][1], "t", 4) == 0) {
                    strncpy(szTemplateFileName, &argv[++i][0], sizeof(szTemplateFileName));
				}
                else if (strncmp(&argv[i][1], "ma", 5) == 0) {
                    strncpy(szMajorVersion, &argv[++i][0], sizeof(szMajorVersion));
                }
                else if (strncmp(&argv[i][1], "mi", 5) == 0) {
                    strncpy(szMinorVersion, &argv[++i][0], sizeof(szMinorVersion));
                }
			}
		}
	}
	else {
		printf("Usage:\n");
		printf("\tvbuild -o [version file] -ma [major version] -mi [minor version] -t [template_file]\n\n");
		return -1;
	}

    strcpy(szVersionStr, szMajorVersion);
    strcat(szVersionStr, ".");
    strcat(szVersionStr, szMinorVersion);
    strcat(szVersionStr, " ");
    strcat(szVersionStr, getTimeStamp());

    fptrTemplate = fopen(szTemplateFileName, "rt");

    if (fptrTemplate == NULL) {
        printf("ERROR: Failed to open template file %s\n", szTemplateFileName);
        return -1;
    }

    fptrVersion = fopen(szVersionFileName, "wt");

    if (fptrVersion == NULL) {
        printf("ERROR: Failed to open version file %s\n", szVersionFileName);
        return -1;
    }

    /*
    ** Find the length of the template file...
    */
    fseek(fptrTemplate, 0L, SEEK_END);
    templateBytes = ftell(fptrTemplate);
    fseek(fptrTemplate, 0L, SEEK_SET);

    pszTemplate = (char *)malloc(templateBytes);

    if (pszTemplate == NULL) {
        printf("ERROR: Cannot allocate memory for template file\n");
        return -1;
    }

    fread(pszTemplate, 1, templateBytes, fptrTemplate);

    for (i = 0;i < templateBytes;i++) {
        if (searching && strncmp(&pszTemplate[i], "<VERSION>", 9) == 0) {
            fwrite(szVersionStr, 1, strlen(szVersionStr), fptrVersion);
            searching = 0;
            i += 8;
        }
        else {
            fputc((int)pszTemplate[i], fptrVersion);
        }
    }

    fclose(fptrTemplate);
    fclose(fptrVersion);

    free(pszTemplate);

    return 0;
}