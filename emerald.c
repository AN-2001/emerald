#include <stdint.h>
#include <stdlib.h>
#include "emerald.h"
#include "defs.h"
#include "memory.h"
#include "utills.h"
#include <stdio.h>
#include <time.h>

static inline int32_t AllocTsk();
static inline MsgEnum FreeTsk(int32_t Id);
static inline TskStruct *GetTsk(int32_t Id);
static inline void PrintTsk(TskStruct *Tsk);
static inline void PrintList(int32_t Start);
static inline void ZeroOutTasks(EmrldStruct *Cntxt, int32_t Start);
static inline void RemoveTskFromList(int32_t Id);

void EmrldAddTsk(char *String,
                 int Priority,
                 BoolType IsScheduled,
                 DateType Date)
{
    TskStruct *Tsk;
    int32_t DateIndex, *OldTsk,
        NewTsk = AllocTsk();


    Tsk = GetTsk(NewTsk);

    strcpy(Tsk -> String, String);
    Tsk -> Type = IsScheduled ? TASK_SCHEDULED : TASK_OVERDUE;
    Tsk -> Priority = Priority;

    if (IsScheduled) {
        DateIndex = EmeraldDateToIndex(Date);
        memcpy(Tsk -> OriginalDate, Date, sizeof(Tsk -> OriginalDate));
        OldTsk = &EmrldCntxt -> Schedule[DateIndex];
    } else
        OldTsk = &EmrldCntxt -> FirstOverdueTask;

    if (*OldTsk != EMERALD_TASK_FREE)
        EmrldCntxt -> TskPool[*OldTsk].PrevTsk = NewTsk;

    Tsk -> NextTsk = *OldTsk;
    Tsk -> PrevTsk = EMERALD_TASK_FREE;

    *OldTsk = NewTsk;
}


void EmrldPrintAll()
{
    int i;

    for (i = 0; i < EmrldCntxt -> NumTasks; i++) 
        if (EmrldCntxt -> TskPool[i].Id != EMERALD_TASK_FREE) 
            PrintTsk(EmrldCntxt -> TskPool + i);
}

void EmrldPrintOverdueTasks()
{
    if (EmrldCntxt -> FirstOverdueTask == EMERALD_TASK_FREE)
        return;
    printf("Emerald: Printing overdue tasks\n");
    PrintList(EmrldCntxt -> FirstOverdueTask);
    printf("\n\n\n");
}

void EmrldPrintTasksByDate(DateType Date)
{
    int32_t
        AsIndex = EmeraldDateToIndex(Date);

    if (EmrldCntxt -> Schedule[AsIndex] == EMERALD_TASK_FREE)
        return;

    printf("Emerald: Printing tasks for %s\n", Date);
    PrintList(EmrldCntxt -> Schedule[AsIndex]);
    printf("\n\n\n");
}

static inline void PrintTsk(TskStruct *Tsk)
{
    printf("*** Task #%d ***\n", Tsk -> Id);
    printf("%s\n", Tsk -> String);
}

static inline void PrintList(int32_t Start)
{
    TskStruct *Tsk;
    int32_t TskId;

    TskId = Start;
    Tsk = GetTsk(TskId);
    while (Tsk) {
        PrintTsk(Tsk);
        TskId = Tsk -> NextTsk;
        Tsk = GetTsk(TskId);
    }

}


static inline void RemoveTskFromList(int32_t Id)
{
    TskStruct
        *Tsk = GetTsk(Id);
    if (!Tsk) {
        fprintf(stderr, "Emerald: bad task, ignoring.\n");
        return;
    }

    if (Tsk -> PrevTsk == EMERALD_TASK_FREE) {
        if (Tsk -> Type == TASK_OVERDUE)
            EmrldCntxt -> FirstOverdueTask = Tsk -> NextTsk;
        else
            EmrldCntxt -> 
                Schedule[EmeraldDateToIndex(Tsk ->
                                    OriginalDate)] = Tsk -> NextTsk;
    } else
        GetTsk(Tsk -> PrevTsk) -> NextTsk = Tsk -> NextTsk;

    Tsk -> PrevTsk = Tsk -> NextTsk = EMERALD_TASK_FREE;
}

void EmrldRemoveTsk(int Id)
{
    RemoveTskFromList(Id);
    FreeTsk(Id);
}

EmrldStruct *EmrldCreateContext()
{
    EmrldStruct *Ret;

    Ret = malloc(sizeof(*Ret) + sizeof(TskStruct) * EMERALD_TASK_STARTER_SIZE);
    if (!Ret)
        return NULL;
    Ret -> FirstOverdueTask = -1;
    memset(Ret -> Schedule, -1, sizeof(Ret -> Schedule));
    memcpy(Ret -> MagicNumber,
           EMERALD_MAGIC_NUMBER,
           sizeof(Ret -> MagicNumber));
    Ret -> NumTasks = EMERALD_TASK_STARTER_SIZE;
    Ret -> Extended = EMERALD_FALSE;
    ZeroOutTasks(Ret, 0);
    return Ret;
}

void EmrldPerformCleanup()
{
    TskStruct *Tsk;
    int32_t i, TodayIndex, Index;
    time_t 
        Time = time(NULL);
    struct tm tm = *localtime(&Time);
    char Today[512];

    sprintf(Today, "%02d/%02d", tm.tm_mday, tm.tm_mon + 1);
    TodayIndex = EmeraldDateToIndex(Today);
    for (i = 0; i < EmrldCntxt -> NumTasks; i++) {
        Tsk = GetTsk(i);
        if (Tsk -> Id == -1 || Tsk -> Type == TASK_OVERDUE ||
                EmeraldDateToIndex(Tsk -> OriginalDate) >= TodayIndex)
            continue;

        RemoveTskFromList(Tsk -> Id);
        Tsk -> Type = TASK_OVERDUE;
        Tsk -> PrevTsk = EMERALD_TASK_FREE;
        Tsk -> NextTsk = EmrldCntxt -> FirstOverdueTask;
        if (EmrldCntxt -> FirstOverdueTask != EMERALD_TASK_FREE)
            GetTsk(EmrldCntxt -> FirstOverdueTask) -> PrevTsk = Tsk -> Id;
        EmrldCntxt -> FirstOverdueTask = Tsk -> Id;
    }
}

static inline int32_t AllocTsk()
{
    int32_t 
        i = 0;
    TskStruct *Tsk;
    EmrldStruct *Tmp;

    while (EmrldCntxt -> TskPool[i].Id != -1 &&
            i < EmrldCntxt -> NumTasks)
        i++;

    /* Maybe don't use realloc here since the file is always mmapped when     */
    /* we add tasks. */
    if (i == EmrldCntxt -> NumTasks) {
        Tmp = EmrldCntxt;
        Tmp -> NumTasks *= 2;
        Tmp = realloc(Tmp,
                sizeof(*EmrldCntxt) + sizeof(TskStruct) * Tmp -> NumTasks);
        EmrldCntxt = Tmp;
        EmrldCntxt -> Extended = EMERALD_TRUE;
        printf("WOOOOO WTF?\n");
        ZeroOutTasks(EmrldCntxt, i);
    }

    Tsk = &EmrldCntxt -> TskPool[i];
    Tsk -> Id = i;
    Tsk -> IsDone = Tsk -> IsOverdue = EMERALD_FALSE;
    Tsk -> Priority = 0;
    memset(Tsk -> String, 0, sizeof(Tsk -> String));
    memset(Tsk -> OriginalDate, 0, sizeof(Tsk -> OriginalDate));
    return i;
}

static inline MsgEnum FreeTsk(int32_t Id)
{
    if (Id < 0 || Id >= EmrldCntxt -> NumTasks)
        return EMERALD_MSG_ID_OUT_OF_RANGE;
    EmrldCntxt -> TskPool[Id].Id = EMERALD_TASK_FREE;
    return EMERALD_MSG_SUCCESS;
}

static inline TskStruct *GetTsk(int32_t Id)
{
    if (Id < 0 || Id >= EmrldCntxt -> NumTasks || Id == EMERALD_TASK_FREE)
        return NULL;
    return EmrldCntxt -> TskPool + Id;
}

size_t EmrldGetPhysicalSize()
{
    return sizeof(EmrldStruct) + sizeof(TskStruct) * EmrldCntxt -> NumTasks;
}

static inline void ZeroOutTasks(EmrldStruct *Cntxt, int32_t Start)
{
    for (; Start < Cntxt -> NumTasks; Start++)
        Cntxt -> TskPool[Start].Id = -1;
}
