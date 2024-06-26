/******************************************************************************\
*  defs.h                                                                      *
*                                                                              *
*  Contains the main structs of emrldd, tasks and schedules.                  *
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
typedef char DateType[9]; /* dd/mm/yy */

/* A max of 4096 active tasks. */
#define EMERALD_STR_SIZE (1024)
#define EMERALD_LONG_STR_SIZE (4096)
#define EMERALD_TASK_STARTER_SIZE (64)
#define EMERALD_DAY_COUNT (372) /* 12 * 31, an upper bound on the number of days. */
#define EMERALD_TASK_FREE (-1) /* The Id of a free task.                      */
#define EMERALD_FALSE (0)
#define EMERALD_TRUE (1)
#define EMERALD_FILE_NAME "tasks.emr"
#define EMERALD_MAGIC_NUMBER "EM00\0"

typedef enum TskTypEnum {
    TASK_OVERDUE,
    TASK_SCHEDULED
} TaskTypeEnum;

typedef enum MsgEnum {
    EMERALD_MSG_SUCCESS,
    EMERALD_MSG_UNKNOWN_ERROR,
    EMERALD_MSG_ID_OUT_OF_RANGE,
} MsgEnum;

typedef struct TskStruct {
    char String[EMERALD_LONG_STR_SIZE]; /* The task's stirng.                 */
    uint8_t Priority; /* A byte representing how important a task is.         */
    int32_t Id; /* The task's ID, used for retrieving.                        */
    /* An Id of -1 indicates that this task is not in use.                    */
    BoolType IsDone, IsOverdue; 
    DateType OriginalDate; /* The original date the task was scheduled in.    */
    TaskTypeEnum Type; 
    int32_t NextTsk; /* An index in the task pool, points to the next task.  */
                     /* Use -1 as a terminator.                              */
    int32_t PrevTsk; 
} TskStruct;

/* The emrldd header, this is fetched first from disk and is used to load    */
/* the entire emrldd header.                                                 */
typedef struct EmrldHeaderStruct {
    int32_t MagicNumber; /* Should be the bytes EM00                          */
    int32_t NumTasks;
} EmrldHeaderStruct;

/* The main emerald struct, this is serialized on disk to store the program   */
/* state.                                                                     */
typedef struct EmrldStruct {
    char MagicNumber[5]; /* Should be the bytes EM00\0                        */
    int32_t NumTasks;
    int32_t Schedule[EMERALD_DAY_COUNT]; 
    int32_t FirstOverdueTask;
    BoolType Extended; /* Was the struct extended? */
    TskStruct TskPool[1];
} EmrldStruct;

#endif /* EMERALD_DEFS_H */
