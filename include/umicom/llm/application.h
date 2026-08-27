/*-----------------------------------------------------------------------------
 * Umicom LLM Module
 * File: include/umicom/llm/application.h
 *
 * PURPOSE:
 *   Expose the thin application composition over Framework-owned experience metadata and services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LLM_APPLICATION_H
#define UMICOM_LLM_APPLICATION_H

#include "umicom/application/experience.h"
#include "umicom/application/experience_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LLM_MODULE_API_VERSION 1U

const char *umi_llm_application_id(void);

const UmiApplicationExperienceDefinition *
umi_llm_application_experience(void);

UmiStatus umi_llm_application_status(
    UmiApplicationExperienceStatus *out_status);

#ifdef __cplusplus
}
#endif

#endif
