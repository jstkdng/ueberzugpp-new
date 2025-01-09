set(CLI11_PRECOMPILED ON)
add_subdirectory(CLI11-2.4.2)

set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_SYSTEM_INCLUDES ON)
add_subdirectory(spdlog-1.15.0)

add_subdirectory(glaze-4.3.0)
