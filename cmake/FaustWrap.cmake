include(CMakeParseArguments)

option(RECOMPILE_FAUST_SOURCES "Recompile faust sources" OFF)

if(RECOMPILE_FAUST_SOURCES)
    find_program(RDMD_PROGRAM "rdmd")
    if(NOT RDMD_PROGRAM)
        message(FATAL_ERROR "rdmd is missing, it is required for regenerating faust sources.")
    endif()
endif()

function(add_faust_command INPUT OUTPUT)
    set(_options ONE_SAMPLE DOUBLE IN_PLACE VECTORIZE MATH_APPROXIMATION)
    set(_one_args PROCESS_NAME CLASS_NAME SUPERCLASS_NAME)
    set(_multi_args IMPORT_DIRS)
    cmake_parse_arguments(_FAUST "${_options}" "${_one_args}" "${_multi_args}" ${ARGN})
    if(NOT RECOMPILE_FAUST_SOURCES)
        return()
    endif()
    if(NOT RDMD_PROGRAM)
        return()
    endif()
    if(NOT INPUT)
        message(FATAL_ERROR "No input file given.")
    endif()
    if(NOT OUTPUT)
        message(FATAL_ERROR "No output file given.")
    endif()
    set(_cmd "${RDMD_PROGRAM}" "${PROJECT_SOURCE_DIR}/scripts/faustwrap.d")
    if(NOT IS_ABSOLUTE "${INPUT}")
        set(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT}")
    endif()
    if(NOT IS_ABSOLUTE "${OUTPUT}")
        set(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT}")
    endif()
    get_filename_component(_output_dir "${OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_output_dir}")
    list(APPEND _cmd "-o" "${OUTPUT}" "${INPUT}")
    if(_FAUST_ONE_SAMPLE)
        list(APPEND _cmd "--os")
    endif()
    if(_FAUST_DOUBLE)
        list(APPEND _cmd "--double")
    endif()
    if(_FAUST_IN_PLACE)
        list(APPEND _cmd "--inpl")
    endif()
    if(_FAUST_VECTORIZE)
        list(APPEND _cmd "--vec")
    endif()
    if(_FAUST_MATH_APPROXIMATION)
        list(APPEND _cmd "--mapp")
    endif()
    if(_FAUST_PROCESS_NAME)
        list(APPEND _cmd "--pn" "${_FAUST_PROCESS_NAME}")
    endif()
    if(_FAUST_CLASS_NAME)
        list(APPEND _cmd "--cn" "${_FAUST_CLASS_NAME}")
    endif()
    if(_FAUST_SUPERCLASS_NAME)
        list(APPEND _cmd "--scn" "${_FAUST_SUPERCLASS_NAME}")
    endif()
    if (_FAUST_IMPORT_DIRS)
        foreach(_dir IN LISTS _FAUST_IMPORT_DIRS)
            if(NOT IS_ABSOLUTE "${_dir}")
                set(_dir "${CMAKE_CURRENT_SOURCE_DIR}/${_dir}")
            endif()
            list(APPEND _cmd "--import-dir" "${_dir}")
        endforeach()
    endif()
    add_custom_command(OUTPUT "${OUTPUT}" COMMAND ${_cmd} DEPENDS "${INPUT}")
endfunction()