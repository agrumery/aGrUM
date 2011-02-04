cmake_minimum_required(VERSION 2.8)
project(SKOOBEDITOR)


## On trouve la librairie Qt
find_package (Qt4 REQUIRED)
set( LIBS ${LIBS} ${QT_LIBRARIES} )

find_library(QSCINTILLA2 qscintilla2)
if(QSCINTILLA2-NOTFOUND)
	message(FATAL_ERROR "---> QScintilla2 not found ! Visit http://http://www.riverbankcomputing.co.uk/")
endif(QSCINTILLA2-NOTFOUND)

include(${QT_USE_FILE})

## On trouve tous les fichiers sources
file(GLOB_RECURSE SOURCES *.cpp ../../../src/agrum/*.cpp ../../../src/agrum/*.inl ../../../src/agrum/*.tcc)
file(GLOB_RECURSE SKOOB_INCLUDES *.h)
file(GLOB_RECURSE AGRUM_INCLUDES ../../../src/agrum/*.h)
file(GLOB_RECURSE RESOURCES ../rsrc/*.qrc)
file(GLOB_RECURSE FORMS *.ui)

## Pour les moc et ui_*.h
include_directories ( ../../../src/ )
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

qt4_wrap_cpp( MOCS ${SKOOB_INCLUDES} )
qt4_wrap_ui( UIS ${FORMS} )
qt4_add_resources( QRC ${RESOURCES} )

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE} )

add_executable(skoobeditor ${SOURCES} ${INCLUDES} ${MOCS} ${UIS} ${QRC} ${AGRUM_INCLUDES})
target_link_libraries( skoobeditor ${QT_LIBRARIES} ${QSCINTILLA2} )
