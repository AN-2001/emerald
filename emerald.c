/******************************************************************************\
*  emrldd.c                                                                   *
*                                                                              *
*  Emrldd, the c todo list and goal manager.                                  *
*                                                                              *
*  1.0: First version.                                                         *
*                                                                              *
*              Written by Abed Naa'ran                          June 2024      *
*                                                                              *
\******************************************************************************/

#include "defs.h"
#include "emerald.h"
#include "stdlib.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>

#define EMERALD_FILE_NAME "tasks.emr"
#define EMERALD_VERSION "1.0"
#define HELP_TEXT \
    "%s: the todo list manager by abed naa'ran\n" \
    "Commands:\n" \
    "- help: prints this message.\n" \
    "- init: initializes the emerald todo list file.\n" \
    "- add: add a new task to the todo list.\n" \
    "- edit: edit an existing task in the todo list, given an id.\n" \
    "- print: print tasks in the todo list.\n" \
    

char CntxtPath[EMERALD_LONG_STR_SIZE];
char *SaveDir = ".local/share/emerald";
EmrldStruct *EmrldCntxt = NULL;

static void VerifyEmeraldDir();
static int ParseArgs(int argc, const char *Argv[]);

int main(int argc, const char *Argv[])
{
    return ParseArgs(argc, Argv);
}

static int ParseArgs(int argc, const char *Argv[])
{
    int fd;
    FILE *File;
    struct stat sb;

    /* Print help if no arguments are provided.                               */
    if (argc == 1) {
        fprintf(stderr, HELP_TEXT, Argv[0]);
        return 0;
    }

    if (argc > 1 && strcmp(Argv[1], "help") == 0) {
        fprintf(stderr, HELP_TEXT, Argv[0]);
        return 0;
    }

    /* Verify that the emerald directory exists, create it if it doesn't.     */
    VerifyEmeraldDir();
    if (argc > 1 && strcmp(Argv[1], "init") == 0) {
        File = fopen(CntxtPath, "w");
        EmrldCntxt = EmrldCreateContext(); 
        fwrite(EmrldCntxt, EmrldGetPhysicalSize(), 1, File);
        fclose(File);
        free(EmrldCntxt);
        return 0;
    }

    if (access(CntxtPath, F_OK)) {
        fprintf(stderr,
                "Emerald: emerald file doesn't exist, please run emerald init.\n");
        fprintf(stderr, HELP_TEXT, Argv[0]);
        return 1;
    }


    /* Load the emerald file.                                                 */
    fd = open(CntxtPath, O_RDWR);
    fstat(fd, &sb);
    EmrldCntxt =
        mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (argc > 1 && strcmp(Argv[1], "add") == 0) {
        /* IMPLEMENT ADD */
        return 0;
    }

    if (argc > 1 && strcmp(Argv[1], "edit") == 0) {
        /* IMPLEMENT EDIT */
        return 0;
    }

    if (argc > 1 && strcmp(Argv[1], "print") == 0) {
        /* IMPLEMENT PRINT */
        return 0;
    }

    munmap(EmrldCntxt, sb.st_size);
    return 0;
}

static void VerifyEmeraldDir()
{
    char Directory[EMERALD_LONG_STR_SIZE];
    struct stat sb;

    sprintf(Directory, "%s/%s", getpwuid(getuid()) -> pw_dir, SaveDir);
    /* Create the directory if it doesn't exist.                              */
    if (stat(Directory, &sb) && !S_ISDIR(sb.st_mode))
        mkdir(Directory, 0777);
    sprintf(CntxtPath, "%s/%s", Directory, EMERALD_FILE_NAME);
}
