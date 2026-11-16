/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: tests/test_benchmark_options.c
 *
 * PURPOSE:
 *   Check backward-compatible benchmark argument forms, optional memory observations and malformed native vectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Assertions remain active in release qualification too. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "../src/console/benchmark_options.h"
#define MODE(name) (strcmp(argv[1],name)==0)
int main(int argc,char **argv)
{
    assert(argc==2);
    char *args[]={"benchmark","checkpoint","suite","--evidence-json","--memory-observations"};
    char **input=args;int count=5,accepted=1,evidence=1,memory=1,has_suite=1;
    LlmBenchmarkOptions out,saved;memset(&out,0x5a,sizeof(out));memcpy(&saved,&out,sizeof(out));
    LlmBenchmarkOptions *destination=&out;
    if(MODE("checkpoint")) { count=2;evidence=memory=has_suite=0; }
    else if(MODE("suite")) { count=3;evidence=memory=0; }
    else if(MODE("json")) { count=3;args[2]="--evidence-json";memory=has_suite=0; }
    else if(MODE("memory")) { count=3;args[2]="--memory-observations";evidence=has_suite=0; }
    else if(MODE("suite-json")) { count=4;memory=0; }
    else if(MODE("suite-memory")) { count=4;args[3]="--memory-observations";evidence=0; }
    else if(MODE("both-options")) { count=4;args[2]="--memory-observations";args[3]="--evidence-json";has_suite=0; }
    else if(MODE("reordered")) { args[2]="--memory-observations";args[3]="--evidence-json";args[4]="suite"; }
    else if(MODE("dash-suite")) args[2]="--custom-suite";
    else if(MODE("duplicate-memory")) { args[3]="--memory-observations";accepted=0; }
    else if(MODE("duplicate-json")) { args[4]="--evidence-json";accepted=0; }
    else if(MODE("two-suites")) { args[4]="second-suite";accepted=0; }
    else if(MODE("empty-checkpoint")) { args[1]="";accepted=0; }
    else if(MODE("empty-suite")) { args[2]="";accepted=0; }
    else if(MODE("null-element")) { args[3]=NULL;accepted=0; }
    else if(MODE("null-vector")) { input=NULL;accepted=0; }
    else if(MODE("null-output")) { destination=NULL;accepted=0; }
    else if(MODE("too-few")) { count=1;accepted=0; }
    else if(MODE("too-many")) { count=6;accepted=0; }
    else if(MODE("negative-count")) { count=-1;accepted=0; }
    else assert(MODE("all"));
    assert(LlmBenchmarkOptionsParse(count,input,destination)==accepted);
    if(!accepted) { assert(memcmp(&out,&saved,sizeof(out))==0);return 0; }
    assert(strcmp(out.checkpoint,"checkpoint")==0);
    assert(out.evidence_json==evidence && out.memory_observations==memory);
    assert((out.suite!=NULL)==has_suite);
    if(out.suite!=NULL) assert(strcmp(out.suite,MODE("dash-suite")?"--custom-suite":"suite")==0);
    return 0;
}
