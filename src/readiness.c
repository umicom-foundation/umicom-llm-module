/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: src/readiness.c
 *
 * PURPOSE:
 *   Project the canonical Framework feature backlog without product-local roadmap duplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/llm/readiness.h"

#include "umicom/llm/runtime.h"
#include "umicom/application/experience_plan.h"

UmiStatus umi_llm_readiness_report(
    UmiApplicationReadinessReport *out_report)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_llm_runtime_experience();
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_application_readiness_report(experience, out_report);
}

const UmiExperienceFeatureDefinition *umi_llm_readiness_next_feature(void)
{
    return umi_application_experience_next_feature(
        umi_llm_runtime_experience());
}
