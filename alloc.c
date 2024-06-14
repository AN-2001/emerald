#include <stdint.h>
#include <stdlib.h>
#include "emerald.h"
#include "defs.h"
#include "memory.h"

static inline void ZeroOutTasks(EmrldStruct *Cntxt, int32_t Start)
{
    memset(Cntxt -> TskPool + Start * sizeof(TskStruct), -1, 
            (Cntxt -> Header.NumTasks - Start) * sizeof(TskStruct));
}

EmrldStruct *EmrldCreateContext()
{
    EmrldStruct *Ret;

    Ret = malloc(sizeof(*Ret) + sizeof(TskStruct) * EMERALD_TASK_STARTER_SIZE);
    if (!Ret)
        return NULL;
    Ret -> FirstPendingTask = -1;
    memset(Ret -> Schedule, -1, sizeof(Ret -> Schedule));
    memcpy((unsigned char*)(&Ret -> Header.MagicNumber),
            EMERALD_MAGIC_NUMBER,
            sizeof(int32_t));
    Ret -> Header.NumTasks = EMERALD_TASK_STARTER_SIZE;
    ZeroOutTasks(Ret, 0);
    return Ret;
}

int EmrldAllocateTask()
{
    int32_t 
        i = 0;
    TskStruct *Tsk;
    EmrldStruct *Tmp;

    while (EmrldCntxt -> TskPool[i].Id != -1 &&
            i < EmrldCntxt -> Header.NumTasks)
        i++;

    if (i == EmrldCntxt -> Header.NumTasks) {
        Tmp = EmrldCntxt;
        Tmp -> Header.NumTasks *= 2;
        Tmp = realloc(Tmp,
                sizeof(*EmrldCntxt) + sizeof(TskStruct) * Tmp -> Header.NumTasks);
        EmrldCntxt = Tmp;
        ZeroOutTasks(EmrldCntxt, i);
    }

    Tsk = &EmrldCntxt -> TskPool[i];
    Tsk -> Id = i;
    Tsk -> IsDone = Tsk -> IsOverdue = EMERALD_FALSE;
    Tsk -> Priority = 0;
    memset(Tsk -> Title, 0, sizeof(Tsk -> Title));
    memset(Tsk -> Desc, 0, sizeof(Tsk -> Desc));
    memset(Tsk -> OriginalDate, 0, sizeof(Tsk -> OriginalDate));
    return i;
}

MsgEnum EmrlddFreeTask(int32_t Id)
{
    if (Id < 0 || Id >= EmrldCntxt -> Header.NumTasks)
        return EMERALD_MSG_ID_OUT_OF_RANGE;
    EmrldCntxt -> TskPool[Id].Id = EMERALD_TASK_FREE;
    return EMERALD_MSG_SUCCESS;
}

TskStruct *EmrldGetTask(int32_t Id)
{
    if (Id < 0 || Id >= EmrldCntxt -> Header.NumTasks)
        return NULL;
    return EmrldCntxt -> TskPool + Id;
}

size_t EmrldGetPhysicalSize()
{
    return sizeof(EmrldStruct) + sizeof(TskStruct) * EmrldCntxt -> Header.NumTasks;
}
