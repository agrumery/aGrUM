INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(execinfo.h HAVE_EXECINFO_H)

include(CheckTypeSize)
CHECK_TYPE_SIZE(char GUM_CHAR_SIZE)
CHECK_TYPE_SIZE(int GUM_INT_SIZE)
CHECK_TYPE_SIZE(long GUM_LONG_SIZE)
CHECK_TYPE_SIZE(float GUM_FLOAT_SIZE)
CHECK_TYPE_SIZE(double GUM_DOUBLE_SIZE)

set(GUM_SRC_PATH "${AGRUM_SOURCE_DIR}")
set(GUM_VERSION "\"${AGRUM_VERSION}\"")
set(GUM_MAJOR_VERSION ${AGRUM_VERSION_MAJOR})
set(GUM_MINOR_VERSION ${AGRUM_VERSION_MINOR})
set(GUM_PATCH_VERSION ${AGRUM_VERSION_PATCH})
