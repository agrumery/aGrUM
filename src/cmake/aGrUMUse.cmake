include (CMakeImportBuildSettings)

cmake_import_build_settings(${aGrUM_BUILD_SETTINGS_FILE})
include_directories(${aGrUM_INCLUDE_DIRS})
link_directories(${aGrUM_LIBRARY_DIRS})
