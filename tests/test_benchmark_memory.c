/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: tests/test_benchmark_memory.c
 *
 * PURPOSE:
 *   Verify memory stage ordering and decoder/suite cleanup in the thin benchmark without loading a model or sampling the host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ai/training/benchmark.h"
#include "umicom/ai/training/benchmark_suite.h"
#include "umicom/ai/training/benchmark_evidence.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/desktop_system/self_memory_report.h"
struct UmiAiTrainingDecoder { int unused; };
struct UmiAiTrainingBenchmarkSuite { int unused; };
static struct UmiAiTrainingDecoder fixture_model;
static struct UmiAiTrainingBenchmarkSuite fixture_suite;
static const char *mode,*stages[8];
static unsigned stage_count,model_owned,suite_owned,model_releases,suite_releases;
static int is(const char *name) { return strcmp(mode,name)==0; }
static UmiStatus load_suite(const char *file,UmiAiTrainingBenchmarkSuite **out)
{ assert(strcmp(file,"suite")==0);if(is("suite-failure"))return UMI_STATUS_IO_ERROR;*out=&fixture_suite;suite_owned=1U;return UMI_STATUS_OK; }
static UmiStatus count_suite(const UmiAiTrainingBenchmarkSuite *value,size_t *out)
{ assert(value==&fixture_suite);if(is("count-failure"))return UMI_STATUS_INVALID_STATE;*out=1U;return UMI_STATUS_OK; }
static UmiStatus case_suite(const UmiAiTrainingBenchmarkSuite *value,size_t index,UmiAiTrainingBenchmarkCase *out)
{ assert(value==&fixture_suite && index==0U);if(is("case-failure"))return UMI_STATUS_INVALID_STATE;*out=(UmiAiTrainingBenchmarkCase){"case",(const uint8_t *)"a",1U,(const uint8_t *)"b",1U};return UMI_STATUS_OK; }
static void release_suite(UmiAiTrainingBenchmarkSuite *value)
{ if(value!=NULL) { assert(value==&fixture_suite && suite_owned==1U);suite_owned=0U;++suite_releases; } }
static UmiStatus load_model(const char *file,UmiAiTrainingDecoder **out)
{ assert(strcmp(file,"checkpoint")==0);if(is("model-failure"))return UMI_STATUS_IO_ERROR;*out=&fixture_model;model_owned=1U;return UMI_STATUS_OK; }
static void release_model(UmiAiTrainingDecoder *value)
{ if(value!=NULL) { assert(value==&fixture_model && model_owned==1U);model_owned=0U;++model_releases; } }
static UmiStatus run_model(UmiAiTrainingDecoder *value,const UmiAiTrainingBenchmarkCase *cases,size_t count,
    const UmiAiTrainingSamplingOptions *options,uint64_t seed,UmiAiTrainingBenchmarkCaseResult *out,size_t capacity,UmiAiTrainingBenchmarkReport *report)
{ assert(value==&fixture_model && cases!=NULL && options!=NULL && seed==UINT64_C(20261022) && capacity==count);if(is("run-failure"))return UMI_STATUS_INVALID_STATE;memset(out,0,count*sizeof(*out));memset(report,0,sizeof(*report));report->caseCount=count;return UMI_STATUS_OK; }
static UmiStatus inspect_evidence(const UmiAiTrainingDecoder *value,const UmiAiTrainingBenchmarkCase *cases,
    const UmiAiTrainingBenchmarkCaseResult *results,size_t count,const UmiAiTrainingBenchmarkReport *report,
    const UmiAiTrainingSamplingOptions *options,uint64_t seed,UmiAiTrainingBenchmarkEvidence *out)
{ (void)cases;(void)results;(void)count;(void)report;(void)options;(void)seed;assert(value==&fixture_model);if(is("evidence-failure"))return UMI_STATUS_INVALID_STATE;memset(out,0,sizeof(*out));return UMI_STATUS_OK; }
static UmiStatus format_evidence(const UmiAiTrainingBenchmarkCase *cases,const UmiAiTrainingBenchmarkCaseResult *results,
    size_t count,const UmiAiTrainingBenchmarkReport *report,const UmiAiTrainingBenchmarkEvidence *evidence,char *out,size_t capacity,size_t *written)
{ (void)cases;(void)results;(void)count;(void)report;(void)evidence;assert(capacity>=4U);if(is("json-failure"))return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(out,"{}\n",4U);*written=3U;return UMI_STATUS_OK; }
static UmiStatus observe_memory(const char *stage,FILE *stream)
{
    assert(stream==stderr && stage_count<8U);stages[stage_count++]=stage;
    if(strcmp(stage,"before-load")==0) assert(!model_owned && !suite_owned);
    if(strcmp(stage,"loaded")==0 || strcmp(stage,"generated")==0) assert(model_owned);
    if(strncmp(stage,"released",8U)==0) assert(!model_owned && !suite_owned);
    return is("memory-unavailable")?UMI_STATUS_UNAVAILABLE:UMI_STATUS_OK;
}
#define UmiAiTrainingBenchmarkSuiteLoadFile load_suite
#define UmiAiTrainingBenchmarkSuiteCount count_suite
#define UmiAiTrainingBenchmarkSuiteCase case_suite
#define UmiAiTrainingBenchmarkSuiteDestroy release_suite
#define UmiAiTrainingDecoderCheckpointLoadFile load_model
#define UmiAiTrainingDecoderDestroy release_model
#define UmiAiTrainingBenchmarkRun run_model
#define UmiAiTrainingBenchmarkEvidenceInspect inspect_evidence
#define UmiAiTrainingBenchmarkEvidenceJson format_evidence
#define UmiDesktopSelfMemoryReport observe_memory
#define main LlmBenchmarkEntryForTest
#include "../src/console/benchmark_main.c"
#undef main
int main(int argc,char **argv)
{
    assert(argc==2);mode=argv[1];
    char *args[]={"benchmark","checkpoint","suite","--memory-observations","--evidence-json"};int count=5;
    if(is("default")) count=2;
    else if(is("text")) count=4;
    int failed=is("suite-failure") || is("count-failure") || is("case-failure") || is("model-failure") ||
        is("run-failure") || is("evidence-failure") || is("json-failure");
    assert(failed || is("default") || is("text") || is("json") || is("memory-unavailable"));
    assert(BenchmarkCommand(count,args)==(failed?1:0));
    assert(!model_owned && !suite_owned);
    int before_model=is("suite-failure") || is("count-failure") || is("case-failure") || is("model-failure");
    assert(model_releases==(before_model?0U:1U));
    assert(suite_releases==((is("default") || is("suite-failure"))?0U:1U));
    if(is("default")) { assert(stage_count==0U);return 0; }
    assert(strcmp(stages[0],"before-load")==0);
    if(is("suite-failure")) { assert(stage_count==2U && strcmp(stages[1],"load-failed")==0);return 0; }
    if(before_model) assert(stage_count==2U);
    else { assert(strcmp(stages[1],"loaded")==0);assert(stage_count==(is("run-failure")?3U:4U));if(!is("run-failure"))assert(strcmp(stages[2],"generated")==0); }
    assert(strcmp(stages[stage_count-1U],failed?"released-after-failure":"released")==0);
    return 0;
}
