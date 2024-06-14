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
*  -EmrldStruct*: A newly allocated emrldd context.                         *
*                                                                              *
\******************************************************************************/
EmrldStruct *EmrldCreateContext();

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

void EmrldAddTsk(char *String,
                 int Priority,
                 BoolType IsScheduled,
                 DateType Date);
void EmrldPrintOverdueTasks();
void EmrldPrintAll();
void EmrldPrintTasksByDate(DateType Date);
void EmrldRemoveTsk(int Id);
void EmrldPerformCleanup();

#endif /* EMERALD_H */
