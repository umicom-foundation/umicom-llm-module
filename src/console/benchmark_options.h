/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: src/console/benchmark_options.h
 *
 * PURPOSE:
 *   Parse product-specific benchmark presentation options without loading files, allocating memory or starting inference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_LLM_BENCHMARK_OPTIONS_H
#define UMICOM_LLM_BENCHMARK_OPTIONS_H
#include <stddef.h>
#include <string.h>
typedef struct LlmBenchmarkOptions {
    const char *checkpoint;
    const char *suite;
    int evidence_json;
    int memory_observations;
} LlmBenchmarkOptions;
/* Borrow argument strings only for the synchronous benchmark invocation. The
 * optional evidence and memory switches do not change the selected decoder. */
static int LlmBenchmarkOptionsParse(int argc,char **argv,LlmBenchmarkOptions *out)
{
    if(out==NULL || argv==NULL || argc<2 || argc>5) return 0;
    for(int i=0;i<argc;++i) if(argv[i]==NULL) return 0;
    if(argv[1][0]=='\0') return 0;
    LlmBenchmarkOptions candidate={argv[1],NULL,0,0};
    for(int i=2;i<argc;++i) {
        if(strcmp(argv[i],"--evidence-json")==0) {
            if(candidate.evidence_json) return 0;
            candidate.evidence_json=1;
        } else if(strcmp(argv[i],"--memory-observations")==0) {
            if(candidate.memory_observations) return 0;
            candidate.memory_observations=1;
        } else {
            /* Preserve the former optional suite-path position. Do not infer
             * another option merely because a filename starts with a dash. */
            if(candidate.suite!=NULL || argv[i][0]=='\0') return 0;
            candidate.suite=argv[i];
        }
    }
    *out=candidate;return 1;
}
#endif
