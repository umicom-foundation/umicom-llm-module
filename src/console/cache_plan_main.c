/*-----------------------------------------------------------------------------
 * Umicom LLM
 * File: src/console/cache_plan_main.c
 *
 * PURPOSE:
 *   Expose Framework cache-budget planning through a thin native LLM console.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/memory_command.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>

/* Framework owns parsing, overflow checks and planning. This console only
 * exposes the shared service under the LLM product and decodes native argv. */
static int CachePlanCommand(int argc,char **argv)
{
    return UmiModelDenseKvCommand(argc-1,(const char *const *)(argv+1),stdout,stderr);
}
int main(int argc,char **argv)
{
    int result=1;
    const UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,CachePlanCommand,&result);
    if(status!=UMI_STATUS_OK) {
        fputs("Unable to decode Umicom LLM cache planner arguments.\n",stderr);
        return 1;
    }
    return result;
}
