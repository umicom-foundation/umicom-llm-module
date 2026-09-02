/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: src/runtime.c
 *
 * PURPOSE:
 *   Initialise the product through canonical Framework application runtime contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/llm/runtime.h"

#include "umicom/llm/application.h"

/*
 * Provide the llm runtime experience operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_llm_runtime_experience(void)
{
    return umi_llm_application_experience();
}

/*
 * Initialise llm runtime from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_llm_runtime_init(
    UmiApplicationWorkspaceRuntime *out_runtime)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_llm_runtime_experience();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_application_workspace_runtime_init(experience, out_runtime);
}

/*
 * Provide the llm runtime health operation used by this module and its client
 * applications.
 */
UmiStatus umi_llm_runtime_health(
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_llm_runtime_experience();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_application_runtime_health_evaluate(
        experience, probe, user_data, out_health);
}
