/******************************************************************************\
*  main.c                                                                      *
*                                                                              *
*  Emrldd, the c todo list and goal manager.                                   *
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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include "sys/wait.h"

#define EMERALD_FILE_NAME "tasks.emr"
#define EMERALD_VERSION "1.0"
#define HELP_TEXT \
    "%s: the todo list manager by abed naa'ran\n" \
    "Commands:\n" \
    "- help: prints this message.\n" \
    "- init: initializes the emerald todo list file.\n" \
    "- add [date]: add a new task to the todo list, date is optional, if it's not set then the task is not scheduled.\n" \
    "- edit id: edit an existing task in the todo list, given an id.\n" \
    "- remove id: removes a given task.\n" \
    "- print [date]: print tasks in the todo list, date is optional, if it's not set the overdue tasks are printed.\n" \
    

static char CntxtPath[EMERALD_LONG_STR_SIZE];
static struct stat StatBuffer;
static char *SaveDir = ".local/share/emerald";
EmrldStruct *EmrldCntxt = NULL;

static void VerifyEmeraldDir();
static int ParseArgs(int argc, const char *Argv[]);
static void WriteBackContext();
static void ObtainString(char *String);
static void OpenEditor(char *Path, char *String);
static void PerformEdit(int32_t Id);

int main(int argc, const char *Argv[])
{
    return ParseArgs(argc, Argv);
}

static int ParseArgs(int argc, const char *Argv[])
{
    int fd;
    FILE *File;
    DateType Date;
    char String[EMERALD_LONG_STR_SIZE];
    BoolType
        IsScheudled = EMERALD_FALSE;

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
    fstat(fd, &StatBuffer);
    EmrldCntxt =
        mmap(NULL, StatBuffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (strcmp(EmrldCntxt -> MagicNumber, EMERALD_MAGIC_NUMBER) != 0) {
        fprintf(stderr, "Emerald: emerald file is corrupted, aborting.\n");
        return 1;
    }

    /* Cleanup before we do anything. */
    EmrldPerformCleanup();
    if (argc > 1 && strcmp(Argv[1], "add") == 0) {
        ObtainString(String);
        if (argc > 2) {
            IsScheudled = EMERALD_TRUE;
            memcpy(Date, Argv[2], sizeof(Date));
        }

        EmrldAddTsk(String, 0, IsScheudled, Date);
    }

    if (argc > 1 && strcmp(Argv[1], "edit") == 0) {
        /* IMPLEMENT EDIT */
        if (argc == 2) {
            fprintf(stderr, "Emerald: didn't specify the task id to edit.\n");
            return 1;
        } else
            PerformEdit(atoi(Argv[2]));
    }

    if (argc > 1 && strcmp(Argv[1], "print") == 0) {
        /* IMPLEMENT PRINT */
        if (argc == 2)
            EmrldPrintOverdueTasks();
        else {
            memcpy(Date, Argv[2], sizeof(Date));
            EmrldPrintTasksByDate(Date);
        }
    }

    if (argc > 1 && strcmp(Argv[1], "remove") == 0) {
        /* IMPLEMENT PRINT */
        if (argc == 2) {
            fprintf(stderr, "Emerald: didn't specify the task id to remove.\n");
            return 1;
        } else
            EmrldRemoveTsk(atoi(Argv[2]));
    }

    /* Write back the context back to disk. */
    WriteBackContext();
    return 0;
}

static void VerifyEmeraldDir()
{
    char Directory[EMERALD_STR_SIZE];
    struct stat StatBuffer;

    snprintf(Directory, EMERALD_STR_SIZE, "%s/%s",
             getpwuid(getuid()) -> pw_dir, SaveDir);
    /* Create the directory if it doesn't exist.                              */
    if (stat(Directory, &StatBuffer) && !S_ISDIR(StatBuffer.st_mode))
        mkdir(Directory, 0777);
    snprintf(CntxtPath, EMERALD_LONG_STR_SIZE,
             "%s/" EMERALD_FILE_NAME, Directory);
}

static void WriteBackContext()
{
    if (!EmrldCntxt -> Extended) {
        munmap(EmrldCntxt, StatBuffer.st_size);
    } else {
        // TODO: IMPLEMENT ME.
        asm("int3");
    }
}

static void OpenEditor(char *Path, char *String)
{
    char *Args[] = {"nvim", Path, NULL};
    int Child, fd, Size;

    Child = fork();
    if (Child < 0) {
        perror("fork");
        exit(1);
    }

    if (!Child) {
        execvp("nvim", (char *const *)Args);
        perror("execv");
    }

    wait(NULL);
    fd = open(Path, O_RDONLY);
    memset(String, 0, EMERALD_LONG_STR_SIZE);
    read(fd, String, EMERALD_LONG_STR_SIZE);
    close(fd);
    unlink(Path);
}

static void ObtainString(char *String)
{
    char
        Path[] = "EMRLDXXXXXX";
    int 
        fd = mkstemp(Path);

    if (fd < 0) {
        perror("mkstemp");
        exit(1);
    }

    /* Weird bug, we have to reopen the fd later.                             */
    close(fd);
    OpenEditor(Path, String);
}

static void PerformEdit(int32_t Id)
{
    char
        Path[] = "EMRLDXXXXXX";
    int 
        fd = mkstemp(Path);
    TskStruct
        *Tsk = &EmrldCntxt -> TskPool[Id];

    write(fd, Tsk -> String, strlen(Tsk -> String));
    close(fd);
    OpenEditor(Path, Tsk -> String);
}
