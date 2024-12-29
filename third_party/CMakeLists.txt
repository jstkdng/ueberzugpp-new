set(CLI11_PRECOMPILED ON)
add_subdirectory(CLI11-2.4.2)

set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_SYSTEM_INCLUDES ON)
add_subdirectory(spdlog-1.15.0)

set(JSON_BuildTests OFF)
set(JSON_Install OFF)
add_subdirectory(json-3.11.3)
