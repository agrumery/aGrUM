option(USE_NANODBC "Build with nanodbc support" ON)

set(NANODBC_USE_UNICODE OFF) #"build with unicode support turned on" OFF)
set(NANODBC_USE_BOOST_CONVERT OFF) #"build using Boost.Locale for string convert" OFF)
set(NANODBC_HANDLE_NODATA_BUG OFF) #"enable special handling for SQL_NO_DATA (required for vertica)" OFF)
set(NANODBC_STATIC OFF) # "build static instead of shared library" OFF)
set(NANODBC_EXAMPLES OFF) # "build examples" OFF)
set(NANODBC_TEST "build tests" OFF) # OFF)
set(NANODBC_INSTALL "generate install target" OFF) # OFF)
set(NANODBC_ENABLE_LIBCXX "Use libc++ if available." ON) # ON)


if (USE_NANODBC)
    CHECK_INCLUDE_FILE("sql.h" HAVE_SQLHEADER)
    if (NOT HAVE_SQLHEADER)
        set(USE_NANODBC OFF)
        message(STATUS "** aGrUM Notification: Can not find <sql.h>. Nanodbc support is OFF.")
    endif ()
endif ()

########################################
## find unixODBC or iODBC config binary
########################################
if (USE_NANODBC)
    if (UNIX)
        # Try to find unixODBC first via odbc_config program.
        find_program(ODBC_CONFIG odbc_config
                $ENV{ODBC_PATH}/bin /usr/bin /usr/local/bin PATHS)
        if (ODBC_CONFIG)
            if (APPLE)
                set(ODBCLIB iodbc)
            else (APPLE)
                set(ODBCLIB odbc)
            endif (APPLE)

            execute_process(COMMAND ${ODBC_CONFIG} --include-prefix
                    OUTPUT_VARIABLE ODBC_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
            set(ODBC_CFLAGS "-I${ODBC_INCLUDE_DIR}")
            set(CMAKE_FLAGS "${CMAKE_FLAGS} ${ODBC_CFLAGS}")
            execute_process(COMMAND ${ODBC_CONFIG} --libs
                    OUTPUT_VARIABLE ODBC_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif (ODBC_CONFIG)

        # Fallback to finding unixODBC via install paths
        if (NOT ODBC_CONFIG)
            find_path(UnixODBC_INCLUDE_DIR uodbc_stats.h
                    /usr/include
                    /usr/local/include
                    /usr/include/odbc
                    /usr/local/include/odbc
                    /usr/include/libodbc
                    /usr/local/include/libodbc)
            if (UnixODBC_INCLUDE_DIR)
                set(ODBC_CONFIG 1)
                set(ODBCLIB odbc)
                set(ODBC_CFLAGS "-I${UnixODBC_INCLUDE_DIR} -DHAVE_UNISTD_H -DHAVE_PWD_H -DHAVE_SYS_TYPES_H -DHAVE_LONG_LONG -DSIZEOF_LONG_INT=8")
            endif (UnixODBC_INCLUDE_DIR)
        endif (NOT ODBC_CONFIG)

        # Fallback to using iODBC
        if (NOT ODBC_CONFIG)
            find_program(ODBC_CONFIG iodbc-config
                    $ENV{ODBC_PATH}/bin /usr/bin /usr/local/bin PATHS)
            if (ODBC_CONFIG)
                set(ODBCLIB iodbc)
                execute_process(COMMAND ${ODBC_CONFIG} --cflags
                        OUTPUT_VARIABLE ODBC_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
                set(CMAKE_FLAGS "${CMAKE_FLAGS} ${ODBC_CFLAGS}")
                execute_process(COMMAND ${ODBC_CONFIG} --libs
                        OUTPUT_VARIABLE ODBC_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
            endif (ODBC_CONFIG)
        endif (NOT ODBC_CONFIG)

        if (NOT ODBC_CONFIG)
            set(USE_NANODBC OFF)
            message(STATUS "** aGrUM Notification: Can not find a suitable ODBC driver manager. Nanodbc support is OFF.")
        else ()
            message(STATUS "** aGrUM Notification: ODBC driver manager found")
        endif ()

    endif ()
endif ()

########################################
## build options
########################################
set(NANODBC_COMPILE_DEFS "")
set(NANODBC_BOOST_INCLUDE_DIRS "")
set(NANODBC_BOOST_LIBRARY_DIRS "")
if (USE_NANODBC)
    list(APPEND NANODBC_COMPILE_DEFS _ODBC)
    if (NANODBC_ODBC_VERSION)
        list(APPEND NANODBC_COMPILE_DEFS NANODBC_ODBC_VERSION=${NANODBC_ODBC_VERSION})
    endif ()

    if (NANODBC_USE_UNICODE)
        list(APPEND NANODBC_COMPILE_DEFS NANODBC_USE_UNICODE)
        if (MSVC)
            # Sets "Use Unicode Character Set" property in Visual Studio projects
            list(APPEND NANODBC_COMPILE_DEFS UNICODE _UNICODE)
        endif ()
    endif ()

    if (NANODBC_USE_BOOST_CONVERT)
        list(APPEND NANODBC_COMPILE_DEFS NANODBC_USE_BOOST_CONVERT)
    endif ()

    if (NANODBC_HANDLE_NODATA_BUG)
        list(APPEND NANODBC_COMPILE_DEFS NANODBC_HANDLE_NODATA_BUG)
    endif ()

    ########################################
    ## find Boost if necessary
    ########################################
    if (NANODBC_USE_BOOST_CONVERT)
        set(Boost_USE_STATIC_LIBS ON)
        set(Boost_USE_MULTITHREADED ON)
        find_package(Boost COMPONENTS unit_test_framework locale REQUIRED)
        if (Boost_FOUND)
            set(NANODBC_BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
            set(NANODBC_BOOST_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})
        endif ()
    endif ()

    ########################################
    ## setup ODBC link libraries
    ########################################
    if (UNIX)
        set(ODBC_LIBRARIES ${ODBCLIB})
        # ODBC_LINK_FLAGS applied to agrumBASE via target_link_options in src/CMakeLists.txt
    elseif (MSVC)
        set(ODBC_LIBRARIES odbc32.lib odbccp32.lib Ws2_32.lib)
    endif ()
endif ()
