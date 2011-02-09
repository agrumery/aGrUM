cmake_minimum_required(VERSION 2.8)
project(SKOOBEDITOR)
set(SKOOBEDITOR_VERSION_MAJOR "0")
set(SKOOBEDITOR_VERSION_MINOR "1")
set(SKOOBEDITOR_VERSION_PATCH "0")

# ~~~~~~~~~~~~ LIBRARIES ~~~~~~~~~~~~ #

## On trouve la librairie Qt
find_package (Qt4 REQUIRED)
include(${QT_USE_FILE})

## On trouve la librairie QScintilla qui doit être installée dans le répertoire de Qt
find_library(QSCINTILLA2 qscintilla2 PATHS ${QT_LIBRARY_DIR})
if(QSCINTILLA2)
	message(STATUS "Found QScintilla : ${QSCINTILLA2}")
else(QSCINTILLA2)
	message(FATAL_ERROR "---> QScintilla2 not found ! Visit http://www.riverbankcomputing.co.uk/")
endif(QSCINTILLA2)

# ~~~~~~~~~~~~ SOURCES ~~~~~~~~~~~~ #

## On trouve tous les fichiers sources
file(GLOB_RECURSE SKOOB_SOURCES *.cpp)
file(GLOB_RECURSE AGRUM_SOURCES ../../../src/agrum/*.cpp ../../../src/agrum/*.inl ../../../src/agrum/*.tcc)
file(GLOB_RECURSE SKOOB_INCLUDES *.h)
file(GLOB_RECURSE AGRUM_INCLUDES ../../../src/agrum/*.h)
file(GLOB_RECURSE RESOURCES ../rsrc/*.qrc)
file(GLOB_RECURSE FORMS *.ui)

# ~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~ #

## Pour les moc et ui_*.h
include_directories ( ../../../src/ )
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# ~~~~~~~~~~~~ PREPROCESSING ~~~~~~~~~~~~ #

qt4_wrap_cpp( MOCS ${SKOOB_INCLUDES} )
qt4_wrap_ui( UIS ${FORMS} )
qt4_add_resources( QRC ${RESOURCES} )

# ~~~~~~~~~~~~ PROCESSING ~~~~~~~~~~~~ #
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE} )
add_executable(skoobeditor WIN32 ${SKOOB_SOURCES} ${AGRUM_SOURCES} ${INCLUDES} ${MOCS} ${UIS} ${QRC} ${AGRUM_INCLUDES} )
set_target_properties( skoobeditor PROPERTIES LINK_FLAGS "-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc" )
target_link_libraries( skoobeditor ${QT_LIBRARIES} ${QSCINTILLA2} )
