option(NANODBC_USE_UNICODE "build with unicode support turned on" OFF)
option(NANODBC_USE_BOOST_CONVERT "build using Boost.Locale for string convert" OFF)
option(NANODBC_HANDLE_NODATA_BUG "enable special handling for SQL_NO_DATA (required for vertica)" OFF)
option(NANODBC_STATIC "build static instead of shared library" OFF)
option(NANODBC_EXAMPLES "build examples" ON)
option(NANODBC_TEST "build tests" ON)
option(NANODBC_INSTALL "generate install target" ON)
option(NANODBC_ENABLE_LIBCXX "Use libc++ if available." ON)

if(APPLE)
	message(STATUS "OS X rpath: Turned on")
	set(CMAKE_MACOSX_RPATH ON)
endif()

########################################
## find unixODBC or iODBC config binary
########################################
if(UNIX)
	# Try to find unixODBC first via odbc_config program.
	find_program(ODBC_CONFIG odbc_config
		$ENV{ODBC_PATH}/bin /usr/bin /usr/local/bin PATHS)
	if(ODBC_CONFIG)
		message(STATUS "ODBC: unixODBC")
		set(ODBCLIB odbc)
		execute_process(COMMAND ${ODBC_CONFIG} --include-prefix
			OUTPUT_VARIABLE ODBC_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
		set(ODBC_CFLAGS "-I${ODBC_INCLUDE_DIR}")
		set(CMAKE_FLAGS "${CMAKE_FLAGS} ${ODBC_CFLAGS}")
		execute_process(COMMAND ${ODBC_CONFIG} --libs
			OUTPUT_VARIABLE ODBC_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	endif()

	# Fallback to finding unixODBC via install paths
	if(NOT ODBC_CONFIG)
		find_path(UnixODBC_INCLUDE_DIR uodbc_stats.h
			/usr/include
			/usr/local/include
			/usr/include/odbc
			/usr/local/include/odbc
			/usr/include/libodbc
			/usr/local/include/libodbc)
		if(UnixODBC_INCLUDE_DIR)
			set(ODBC_CONFIG 1)
			message(STATUS "ODBC: unixODBC")
			set(ODBCLIB odbc)
			set(ODBC_CFLAGS "-I${UnixODBC_INCLUDE_DIR} -DHAVE_UNISTD_H -DHAVE_PWD_H -DHAVE_SYS_TYPES_H -DHAVE_LONG_LONG -DSIZEOF_LONG_INT=8")
		endif()
	endif()

	# Fallback to using iODBC
	if(NOT ODBC_CONFIG)
		find_program(ODBC_CONFIG iodbc-config
			$ENV{ODBC_PATH}/bin /usr/bin /usr/local/bin PATHS)
		if(ODBC_CONFIG)
			message(STATUS "ODBC: iODBC")
			set(ODBCLIB iodbc)
			execute_process(COMMAND ${ODBC_CONFIG} --cflags
				OUTPUT_VARIABLE ODBC_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
			set(CMAKE_FLAGS "${CMAKE_FLAGS} ${ODBC_CFLAGS}")
			execute_process(COMMAND ${ODBC_CONFIG} --libs
				OUTPUT_VARIABLE ODBC_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		endif()
	endif()

	if(NOT ODBC_CONFIG)
		message(WARNING "can not find a suitable odbc driver manager")
  else()
    add_definitions(-D_ODBC)
	endif()

	message(STATUS "ODBC compile flags: ${ODBC_CFLAGS}")
	message(STATUS "ODBC link flags: ${ODBC_LINK_FLAGS}")
endif()

########################################
## build options
########################################
IF(NOT DEFINED NANODBC_ODBC_VERSION)
	message(STATUS "ODBC Version Override: Turned off")
else()
	message(STATUS "ODBC Version Override: ${NANODBC_ODBC_VERSION}")
	add_definitions(-DNANODBC_ODBC_VERSION=${NANODBC_ODBC_VERSION})
endif()

if(NANODBC_USE_UNICODE)
	message(STATUS "Unicode support: Turned on")
	add_definitions(-DNANODBC_USE_UNICODE)
	if(MSVC)
		# Sets "Use Unicode Character Set" property in Visual Studio projects
		add_definitions(-DUNICODE -D_UNICODE)
	endif()
else()
	message(STATUS "Unicode support: Turned off")
endif()

if(NANODBC_USE_BOOST_CONVERT)
	message(STATUS "Boost string convert: Turned on")
	add_definitions(-DNANODBC_USE_BOOST_CONVERT)
else()
	message(STATUS "Boost string convert: Turned off")
endif()

if(NANODBC_HANDLE_NODATA_BUG)
	message(STATUS "Handle SQL_NO_DATA bug: Turned on")
	add_definitions(-DNANODBC_HANDLE_NODATA_BUG)
else()
	message(STATUS "Handle SQL_NO_DATA bug: Turned off")
endif()

########################################
## find Boost if necessary
########################################
if(NANODBC_USE_BOOST_CONVERT)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	find_package(Boost COMPONENTS unit_test_framework locale REQUIRED)
	if(Boost_FOUND)
		include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})
		link_directories(${CMAKE_BINARY_DIR}/lib ${Boost_LIBRARY_DIRS})
	else()
		message(FATAL_ERROR "can not find boost")
	endif()
endif()

########################################
## setup ODBC link libraries
########################################
if(UNIX)
	set(ODBC_LIBRARIES ${ODBCLIB})
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ODBC_LINK_FLAGS}")
elseif(MSVC)
	set(ODBC_LIBRARIES odbc32.lib odbccp32.lib Ws2_32.lib)
endif()
