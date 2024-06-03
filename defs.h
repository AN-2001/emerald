/******************************************************************************\
*  defs.h                                                                      *
*                                                                              *
*  Contains the main structs of emerald, tasks and schedules.                  *
*                                                                              *
*  1.0: First version.                                                         *
*                                                                              *
*              Written by Abed Naa'ran                          <date>         *
*                                                                              *
\******************************************************************************/
#ifndef EMERALD_DEFS_H
#define EMERALD_DEFS_H

#include <stdint.h>

typedef uint8_t BoolType;
typedef char DateType[5];

/* A max of 4096 active tasks. */
#define EMERALD_TASK_TITLE_LEN (128)
#define EMERALD_TASK_DESC_LEN (4096)
#define EMERALD_TASK_STARTER_SIZE (64)
#define EMERALD_DAY_COUNT (372) /* 12 * 31, an upper bound on the number of days. */

typedef enum TaskTypeEnum {
    TASK_PENDING,
    TASK_SCHEDULED
} TaskTypeEnum;

typedef struct TaskStruct {
    char Title[EMERALD_TASK_TITLE_LEN]; /* The task's title.                  */
    char Desc[EMERALD_TASK_DESC_LEN]; /* The task's description.              */
    uint8_t Priority; /* A byte representing how important a task is.         */
    int32_t Id; /* The task's ID, used for retrieving.                        */
    /* An Id of -1 indicates that this task is not in use.                    */
    BoolType IsDone, IsOverdue; 
    DateType OriginalDate; /* The original date the task was scheduled in.    */
    TaskTypeEnum Type; 
    int32_t NextTask; /* An index in the task pool, points to the next task.  */
                      /* Use -1 as a terminator.                              */
} TaskStruct;

/* The emerald header, this is fetched first from disk and is used to load    */
/* the entire emerald header.                                                 */
typedef struct EmeraldHeaderStruct {
    int32_t MagicNumber; /* Should be the bytes EM00                          */
    int32_t NumTasks;
} EmeraldHeaderStruct;

/* The main emerald struct, this is serialized on disk to store the program   */
/* state.                                                                     */
typedef struct EmeralStruct {
    EmeraldHeaderStruct Header;
    int32_t Schedule[EMERALD_DAY_COUNT]; 
    int32_t FirstPendingTask;
    TaskStruct TaskPool[1];
} EmeralStruct;

#endif /* EMERALD_DEFS_H */
