cmake_minimum_required(VERSION 2.8)
project(skoobeditor)
set(skoobeditor_VERSION_MAJOR "0")
set(skoobeditor_VERSION_MINOR "4")
set(skoobeditor_VERSION_PATCH "0")

# ~~~~~~~~~~~~ LIBRARIES ~~~~~~~~~~~~ #

## On trouve la librairie Qt
find_package (Qt4 COMPONENTS QtCore QtGui REQUIRED)
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
file(GLOB_RECURSE SKOOB_INCLUDES *.h)
file(GLOB RESOURCES ../rsrc/*.qrc)
file(GLOB_RECURSE FORMS *.ui)
file(GLOB RC_FILE ../rsrc/*.rc)

# TODO : Ajouté les fichiers objects *.o de aGrUM/build/ plutôt que les sources
# et utiliser set_source_files_properties EXTERNAL_OBJECT pour l'indiquer.
#file(GLOB_RECURSE AGRUM_SOURCES ../../../src/agrum/*.cpp ../../../src/agrum/*.inl ../../../src/agrum/*.tcc)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  file(GLOB_RECURSE AGRUM_SOURCES ../../../build/linux/debug/*.o ../../../build/windows/debug/*.obj)
else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  file(GLOB_RECURSE AGRUM_SOURCES ../../../build/linux/release/*.o ../../../build/windows/release/*.obj)
endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
message(WARNING "${AGRUM_SOURCES}")

file(GLOB_RECURSE AGRUM_INCLUDES ../../../src/agrum/*.h)
set_source_files_properties( ${AGRUM_SOURCES} PROPERTIES EXTERNAL_OBJECT true )

# ~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~ #

## Pour les moc et ui_*.h
include_directories ( ../../../src/ )
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# ~~~~~~~~~~~~ PREPROCESSING ~~~~~~~~~~~~ #

qt4_wrap_cpp( MOCS ${SKOOB_INCLUDES} )
qt4_wrap_ui( UIS ${FORMS} )
qt4_add_resources( QRC ${RESOURCES} )

# Executable icon
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  enable_language(RC)
  #SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  set(GUI_TYPE WIN32)
endif(MINGW)

# ~~~~~~~~~~~ COMPILATION FLAGS ~~~~~~~~~~~~~~~~~~~~~~~ #
set(CMAKE_CXX_FLAGS_DEBUG " -O0 -pg -g -DGUM_NO_INLINE")
set(CMAKE_C_FLAGS_DEBUG "-O0 -pg -g -DGUM_NO_INLINE")

set(CMAKE_CXX_FLAGS_RELEASE "${AGRUM_OPTIMIZATION} -DNDEBUG ${AGRUM_INLINING_POLICY}")
set(CMAKE_C_FLAGS_RELEASE "${AGRUM_OPTIMIZATION} -DNDEBUG ${AGRUM_INLINING_POLICY}")

# ~~~~~~~~~~~~ PROCESSING ~~~~~~~~~~~~ #
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE} )
add_executable(skoobeditor ${GUI_TYPE} ${SKOOB_SOURCES} ${AGRUM_SOURCES} ${INCLUDES} ${MOCS} ${UIS} ${QRC} ${AGRUM_INCLUDES} ${RC_FILE})

#set_target_properties( skoobeditor PROPERTIES LINK_FLAGS "--enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc" )
# I do not know what is enable-stdcall-fixup (neither do G++ ???)
target_link_libraries( skoobeditor ${QT_LIBRARIES} ${QSCINTILLA2} )

