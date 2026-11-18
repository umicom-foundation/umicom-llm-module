#-----------------------------------------------------------------------------
# Umicom LLM Module
# File: CMakeLists.txt
#
# PURPOSE:
#   Build a thin C23 product composition over Framework-owned application
#   experience, runtime and workbench contracts.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

project(
    LlmModule
    VERSION 0.1.0
    DESCRIPTION "Thin Umicom LLM composition over Umicom Framework"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(UMICOM_LLM_BUILD_CONSOLE
       "Build the native Umicom LLM verification console"
       ON)

include(CTest)
include(GNUInstallDirs)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET Umicom::Framework)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET Umicom::application)
    message(FATAL_ERROR
        "Umicom LLM requires Framework target Umicom::application")
endif()

# Define the local fallback only when the parent build did not provide the shared helper.
if(NOT COMMAND umicom_apply_warnings)
    # Define the apply warnings build helper so parent and application projects apply one
    # consistent rule.
    function(umicom_apply_warnings target)
        # Select the warning options understood by the active compiler.
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        # Use this fallback path when the earlier condition does not apply.
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endfunction()
endif()

# Define the local fallback only when the parent build did not provide the shared helper.
if(NOT COMMAND umicom_apply_sanitizers)
    # Define the apply sanitizers build helper so parent and application projects apply one
    # consistent rule.
    function(umicom_apply_sanitizers target)
    endfunction()
endif()

add_library(umicom_llm_module STATIC
    src/application.c
    src/workspace.c
)
add_library(Umicom::llm_module ALIAS umicom_llm_module)

target_include_directories(umicom_llm_module PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_llm_module PUBLIC
    Umicom::Framework
    Umicom::application
)
umicom_apply_warnings(umicom_llm_module)
umicom_apply_sanitizers(umicom_llm_module)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomThinApplicationRuntime.cmake")
umicom_extend_thin_application_runtime(
    NAME "llm"
    TARGET "umicom_llm_module")

# Create this optional product surface only when its build option is enabled.
if(UMICOM_LLM_BUILD_CONSOLE)
    add_executable(umicom-llm-console
        src/console/main.c)
    target_link_libraries(umicom-llm-console PRIVATE Umicom::llm_module)
    umicom_apply_warnings(umicom-llm-console)
    umicom_apply_sanitizers(umicom-llm-console)
endif()

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    add_executable(llm-application-test
        tests/test_application.c)
    target_link_libraries(llm-application-test PRIVATE Umicom::llm_module)
    umicom_apply_warnings(llm-application-test)
    umicom_apply_sanitizers(llm-application-test)
    add_test(NAME llm.module.application
             COMMAND llm-application-test)

    add_executable(llm-workspace-test
        tests/test_workspace.c)
    target_link_libraries(llm-workspace-test PRIVATE Umicom::llm_module)
    umicom_apply_warnings(llm-workspace-test)
    umicom_apply_sanitizers(llm-workspace-test)
    add_test(NAME llm.module.workspace
             COMMAND llm-workspace-test)
endif()

install(TARGETS umicom_llm_module
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# Configure the optional target only when its feature has created it.
if(TARGET umicom-llm-console)
    install(TARGETS umicom-llm-console
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES application.umicom.yaml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/applications/llm)
# Use the optional file only when it is present in this checkout.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    install(FILES LICENSE
        DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/llm)
endif()

message(STATUS "Umicom LLM Module version: ${PROJECT_VERSION}")
message(STATUS "Umicom LLM uses Framework-owned application experience")

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomApplicationProductisationContribution.cmake")
umicom_add_application_productisation_contribution(
    NAME "llm"
    TARGET umicom_llm_productisation
    SOURCE "src/productisation_contribution.c"
    TEST_SOURCE "tests/test_productisation_contribution.c")

# Shared AI behaviour belongs to Framework. This composition adds an operations
# surface and example without removing the module's existing tests or layouts.
include("${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomAiWorkspace.cmake")
umicom_attach_ai_workspace(NAME llm TARGET umicom_llm_module)

# Optional native byte-completion evidence for reviewed decoder checkpoints.
# Keep the existing LLM product and console unchanged unless explicitly enabled.
# The shared maths, sampling and benchmark APIs come from Framework only.
option(UMICOM_LLM_BUILD_NATIVE_BENCHMARK
       "Build read-only native Umicom LLM completion benchmark" OFF)
if(UMICOM_LLM_BUILD_NATIVE_BENCHMARK)
    if(NOT TARGET Umicom::AiTraining)
        include("${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomNativeLlmTraining.cmake")
    endif()
    add_executable(umicom-llm-benchmark src/console/benchmark_main.c)
    # The former minimal link is preserved; desktop observations and UTF-8 entry
    # handling now use their shared Framework components.
    if(FALSE)
    target_link_libraries(umicom-llm-benchmark PRIVATE Umicom::AiTraining)
    endif()
    # Native observations are an explicitly requested diagnostic surface. Reuse
    # the installed/source desktop and UTF-8 platform services without app copies.
    if(NOT TARGET Umicom::desktop_system)
        message(FATAL_ERROR "Native LLM benchmark requires Framework desktop_system")
    endif()
    target_link_libraries(umicom-llm-benchmark PRIVATE Umicom::AiTraining Umicom::desktop_system Umicom::platform)
    target_compile_features(umicom-llm-benchmark PRIVATE c_std_23)
    umicom_apply_warnings(umicom-llm-benchmark)
    umicom_apply_sanitizers(umicom-llm-benchmark)
    include("${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-llm-benchmark
        PRODUCT_NAME "Umicom LLM Benchmark" INTERNAL_NAME "umicom-llm-benchmark"
        APPLICATION_ID "org.umicom.llm.benchmark" NO_RUNTIME_DEPLOYMENT)
    install(TARGETS umicom-llm-benchmark
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Product-specific switches have no model or platform side effects.
if(BUILD_TESTING)
    add_executable(llm-benchmark-options-test tests/test_benchmark_options.c)
    umicom_apply_warnings(llm-benchmark-options-test)
    umicom_apply_sanitizers(llm-benchmark-options-test)
    foreach(_case IN ITEMS all checkpoint suite json memory suite-json suite-memory both-options reordered dash-suite duplicate-memory duplicate-json two-suites empty-checkpoint empty-suite null-element null-vector null-output too-few too-many negative-count)
        add_test(NAME "llm.benchmark.options.${_case}" COMMAND llm-benchmark-options-test "${_case}")
        set_tests_properties("llm.benchmark.options.${_case}" PROPERTIES TIMEOUT 20 LABELS "llm;memory;regression")
    endforeach()
endif()

# Compile the real thin console against inert model services; no weights are loaded.
if(BUILD_TESTING)
    add_executable(llm-benchmark-memory-test tests/test_benchmark_memory.c)
    target_link_libraries(llm-benchmark-memory-test PRIVATE Umicom::platform)
    umicom_apply_warnings(llm-benchmark-memory-test)
    umicom_apply_sanitizers(llm-benchmark-memory-test)
    foreach(_case IN ITEMS default text json memory-unavailable suite-failure count-failure case-failure model-failure run-failure evidence-failure json-failure)
        add_test(NAME "llm.benchmark.memory.${_case}" COMMAND llm-benchmark-memory-test "${_case}")
        set_tests_properties("llm.benchmark.memory.${_case}" PROPERTIES TIMEOUT 20 LABELS "llm;memory;regression")
    endforeach()
endif()


# Product composition stays thin: Framework owns every geometry and parsing rule.
option(UMICOM_LLM_BUILD_CACHE_PLANNER "Build the offline dense KV-cache planning console" OFF)
if(UMICOM_LLM_BUILD_CACHE_PLANNER)
    if(NOT TARGET Umicom::local_models)
        message(FATAL_ERROR "Umicom LLM cache planning requires Framework local_models")
    endif()
    add_executable(umicom-llm-cache-plan src/console/cache_plan_main.c)
    target_link_libraries(umicom-llm-cache-plan PRIVATE Umicom::local_models Umicom::platform)
    target_compile_features(umicom-llm-cache-plan PRIVATE c_std_23)
    umicom_apply_warnings(umicom-llm-cache-plan)
    umicom_apply_sanitizers(umicom-llm-cache-plan)
    include("${CMAKE_CURRENT_SOURCE_DIR}/../../framework/cmake/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-llm-cache-plan
        PRODUCT_NAME "Umicom LLM Cache Planner" INTERNAL_NAME "umicom-llm-cache-plan"
        APPLICATION_ID "org.umicom.llm.cache-plan" NO_RUNTIME_DEPLOYMENT)
    install(TARGETS umicom-llm-cache-plan RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    if(BUILD_TESTING)
        add_test(NAME llm.cache_plan.command
            COMMAND "${CMAKE_COMMAND}" "-DTOOL=$<TARGET_FILE:umicom-llm-cache-plan>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/check_cache_plan.cmake")
        set_tests_properties(llm.cache_plan.command PROPERTIES TIMEOUT 20 LABELS "llm;memory;offline")
    endif()
endif()
