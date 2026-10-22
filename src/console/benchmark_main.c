/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: src/console/benchmark_main.c
 *
 * PURPOSE:
 *   Provide a thin, explicitly invoked native completion-benchmark console.
 *   Framework owns checkpoints, sampling, validation and all score metrics.
 *   No generated source is executed, compiled, saved or treated as trusted.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark.h"
#include "umicom/ai/training/benchmark_suite.h"
#include "umicom/ai/training/checkpoint.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Curated teaching fragments, not an independent or hidden skill benchmark.
 * Later training/evaluation suites must be reviewed for leakage and licensed
 * independently of any prompt-completion demonstration. */
static const uint8_t PROMPT_ACCOUNT[] = "typedef struct Account {\n";
static const uint8_t ANSWER_ACCOUNT[] = "    int64_t balanceMinor;\n";
static const uint8_t PROMPT_AMOUNT[] = "/* Money is stored in integer minor units. */\n";
static const uint8_t ANSWER_AMOUNT[] = "#include <stdint.h>\n";
static const uint8_t PROMPT_CHECK[] = "UmiStatus AccountValidate(";
static const uint8_t ANSWER_CHECK[] = "const Account *account)";

int main(int argc, char **argv)
{
    const UmiAiTrainingSamplingOptions sampling = {1.0F, 1U};
    const UmiAiTrainingBenchmarkCase cases[] = {
        {"Account record completion", PROMPT_ACCOUNT,
         sizeof(PROMPT_ACCOUNT) - 1U, ANSWER_ACCOUNT,
         sizeof(ANSWER_ACCOUNT) - 1U},
        {"Integer accounting convention", PROMPT_AMOUNT,
         sizeof(PROMPT_AMOUNT) - 1U, ANSWER_AMOUNT,
         sizeof(ANSWER_AMOUNT) - 1U},
        {"API signature completion", PROMPT_CHECK,
         sizeof(PROMPT_CHECK) - 1U, ANSWER_CHECK,
         sizeof(ANSWER_CHECK) - 1U}
    };
    const size_t defaultCaseCount = sizeof(cases) / sizeof(cases[0]);
    UmiAiTrainingBenchmarkSuite *suite = NULL;
    UmiAiTrainingBenchmarkCase reviewedCases[UMI_AI_BENCHMARK_MAX_CASES];
    const UmiAiTrainingBenchmarkCase *activeCases = cases;
    size_t caseCount = defaultCaseCount;
    UmiAiTrainingBenchmarkCaseResult results[UMI_AI_BENCHMARK_MAX_CASES];
    UmiAiTrainingBenchmarkReport report;
    UmiAiTrainingDecoder *decoder = NULL;
    UmiStatus status;

    if ((argc != 2 && argc != 3) || argv[1] == NULL || argv[1][0] == '\0') {
        fprintf(stderr, "usage: umicom-llm-benchmark <approved-decoder-checkpoint> [reviewed-suite-file]\n");
        return 2;
    }
    if (argc == 3) {
        status = UmiAiTrainingBenchmarkSuiteLoadFile(argv[2], &suite);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "Reviewed benchmark suite rejected (status=%d)\n",
                    (int)status);
            return 1;
        }
        status = UmiAiTrainingBenchmarkSuiteCount(suite, &caseCount);
        if (status != UMI_STATUS_OK) goto failed;
        for (size_t i = 0U; i < caseCount; ++i) {
            status = UmiAiTrainingBenchmarkSuiteCase(suite, i, &reviewedCases[i]);
            if (status != UMI_STATUS_OK) goto failed;
        }
        activeCases = reviewedCases;
    }
    status = UmiAiTrainingDecoderCheckpointLoadFile(argv[1], &decoder);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Unable to load reviewed native decoder (status=%d)\n",
                (int)status);
        UmiAiTrainingBenchmarkSuiteDestroy(suite);
        return 1;
    }
    status = UmiAiTrainingBenchmarkRun(decoder, activeCases, caseCount,
                                     &sampling, UINT64_C(20261022),
                                     results, caseCount, &report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Completion benchmark rejected (status=%d)\n",
                (int)status);
        goto failed;
    }
    puts("Umicom LLM — Reviewed Teaching Completion Benchmark");
    printf("Checkpoint steps: %" PRIu64 "; cases: %zu\n",
           report.completedOptimiserSteps, report.caseCount);
    for (size_t i = 0U; i < caseCount; ++i) {
        printf("%-32s exact=%s, positional=%zu/%zu, prefix=%zu\n",
               activeCases[i].title, results[i].exactMatch ? "yes" : "no",
               results[i].matchingPositionBytes,
               results[i].expectedBytes,
               results[i].matchingPrefixBytes);
    }
    printf("Exact completion cases: %zu/%zu\n"
           "Matching byte positions: %zu/%zu\n",
           report.exactMatchCases, report.caseCount,
           report.matchingPositionBytes, report.expectedBytes);
    puts("LIMITATION: Matching expected bytes neither compiles nor tests C23 code.");
    puts("No generated code has been executed, saved or deemed trustworthy.");
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingBenchmarkSuiteDestroy(suite);
    return 0;
failed:
    UmiAiTrainingDecoderDestroy(decoder);
    UmiAiTrainingBenchmarkSuiteDestroy(suite);
    return 1;
}
