/******************************************************************************\
*  emerald.h                                                                   *
*                                                                              *
*  Defines the utility emerald functions.                                      *
*                                                                              *
*  1.0: First version.                                                         *
*                                                                              *
*              Written by Abed Naa'ran                          June 2024      *
*                                                                              *
\******************************************************************************/

#ifndef EMERALD_H
#define EMERALD_H

#include "defs.h"
#include "stddef.h"

extern EmrldStruct *EmrldCntxt;

/******************************************************************************\
* EmrldCreateContext                                                           *
*                                                                              *
*  Creates a new empty emrldd context.                                        *
*                                                                              *
* Return                                                                       *
*                                                                              *
*  -EmrlddStruct*: A newly allocated emrldd context.                         *
*                                                                              *
\******************************************************************************/
EmrldStruct *EmrldCreateContext();

/******************************************************************************\
* EMeraldAllocTsk                                                              *
*                                                                              *
*  Allocates a new task and returns its id.                                    *
*                                                                              *
* Parameters                                                                   *
*                                                                              *
*  -Emrld: The emerald context.                                                *
*                                                                              *
* Return                                                                       *
*                                                                              *
*  -int: The Id of the new task.                                               *
*                                                                              *
\******************************************************************************/
int EmrldAllocTsk(EmrldStruct *Emrld);


/******************************************************************************\
* EmrldFreeTask                                                                *
*                                                                              *
*  Frees the task at Id.                                                       *
*                                                                              *
* Parameters                                                                   *
*                                                                              *
*  -Emrld: The emrldd context.                                                 *
*  -Id: The id of the task to free.                                            *
*                                                                              *
* Return                                                                       *
*                                                                              *
*  -void.                                                                      *
*                                                                              *
\******************************************************************************/
void EmrldFreeTask(EmrldStruct *Emrld, int Id);

/******************************************************************************\
* EmrlddGetTask                                                                *
*                                                                              *
*  Retrieves the task placed at Id.                                            *
*                                                                              *
* Parameters                                                                   *
*                                                                              *
*  -Emrld: The emrldd context.                                                 *
*  -Id: The Id of the requested task.                                          *
*                                                                              *
* Return                                                                       *
*                                                                              *
*  -TskStruct*: The task struct.                                               *
*                                                                              *
\******************************************************************************/
TskStruct *EmrlddGetTask(EmrldStruct *Emrld, int Id);

/******************************************************************************\
* EmrldGetPhysicalSize                                                         *
*                                                                              *
*  Return the physical size of the current context in bytes.                   *
*                                                                              *
* Return                                                                       *
*                                                                              *
*  -size_t: The size of the context in bytes.                                  *
*                                                                              *
\******************************************************************************/
size_t EmrldGetPhysicalSize();

#endif /* EMERALD_H */
