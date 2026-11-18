#-----------------------------------------------------------------------------
# Umicom LLM
# File: tests/check_cache_plan.cmake
#
# PURPOSE:
#   Qualify the thin native planner console without loading a model.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED TOOL OR NOT EXISTS "${TOOL}")
    message(FATAL_ERROR "Build the selected cache planner before qualification")
endif()
foreach(_budget IN ITEMS 1920 1919)
    execute_process(COMMAND "${TOOL}" ${_budget} 10 2 3 2 2 4 4 2 2
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 10)
    if(NOT "${_result}" STREQUAL "0")
        message(FATAL_ERROR "Valid cache plan refused: ${_err}")
    endif()
    if(_budget EQUAL 1920)
        set(_expected "affordable_tokens=10")
    else()
        set(_expected "affordable_tokens=9")
    endif()
    if(NOT _out MATCHES "${_expected}" OR NOT _out MATCHES "KV payload only")
        message(FATAL_ERROR "Cache report lacks the expected capacity or limit: ${_out}")
    endif()
endforeach()
foreach(_bad IN ITEMS -1 1x 18446744073709551616)
    execute_process(COMMAND "${TOOL}" "${_bad}" 10 2 3 2 2 4 4 2 2
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 10)
    if(NOT "${_result}" STREQUAL "2" OR _out MATCHES "affordable_tokens")
        message(FATAL_ERROR "Malformed argument was not refused cleanly: ${_bad}")
    endif()
endforeach()
execute_process(COMMAND "${TOOL}" 1920 0 2 3 2 2 4 4 2 2
    RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 10)
if(NOT "${_result}" STREQUAL "2" OR _out MATCHES "affordable_tokens")
    message(FATAL_ERROR "Zero requested geometry was not refused")
endif()
