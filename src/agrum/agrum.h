/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/*! \mainpage aGrUM a GRaphical Universal Model

\section intro_sec Presentation
\b aGrUM is a C++ library designed for easily building applications using
graphical models such as Bayesian networks, influence diagrams, decision trees,
GAI networks or Markov decision processes. It is written to provide the basic
building blocks to perform the following tasks :
  - graphical model learning / elicitation,
  - inference within the graphical model,
  - planification.

\section howto_sec How to?
Here is a list of howtos about some classes of aGrUM :
  - \ref installing_agrum "How to install aGrUM library ?"
  - \ref using_agrum "How to create a project using aGrUM library ?"
  - \ref graph_page "How to use graph classes ?"
  - \ref multidim_page "How to use the MultiDim hierarchy ?"
  - \ref instantiation_page "How to use a Instantiation ?"
  - \ref signal_page "How to use signaler/listener in aGrUM ?"
  - \ref prm_page "How to use Probabilistic Relational Models ?"

  \defgroup basicstruct_group Basic data structures
  \defgroup graph_group Graph representation
  \defgroup multidim_group Multidimensional data
  \defgroup bn_group Bayesian Networks
  \defgroup learning_group Tools for learning
  \defgroup signal_group Signaler and Listener
  \defgroup prm_group Probabilistic Relational Models
  \defgroup fmdp_group Factored Markov Decision Process
  \defgroup cn_group Credal Networks
*/

/*! \page installing_agrum Installing the aGrUM library
\section Installation

aGrUM is in heavy developpement. Do not hesitate to have a look at
http://agrum.lip6.fr to see which version you should use.

To install aGrUm :

    git clone git://forge.lip6.fr/aGrUM


To test aGrUM, you could run (quite long)

    act test release -t all

To install aGruM, you classically have the choice between :

- system-wide installation : using root privilege
\verbatim
sudo act install release
\endverbatim

- user-wide installation : if you want to install it in ~/usr
\verbatim
act install release -d ~/usr
\endverbatim

\page using_agrum Using aGrUM

As a build system, aGrUM uses CMake (http://www.cmake.org). A minimal project with
agrum should look like this (for a project foo):
- in the project folder, a sub-folder src,
- in src folder, your *.{cpp|h|etc.} files
- in src folder, a file named CMakeLists.txt like this one :

\verbatim
project(FOO)
cmake_minimum_required(VERSION 2.8)

# do not forget to change this line if needed ("act install -d...")
set(AGRUM_INSTALLATION_DIRECTORY "installation_path")
set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")

find_package(aGrUM)

if (aGrUM_FOUND)
  include_directories(${AGRUM_INCLUDE_DIR})
  link_directories(${AGRUM_LIB_DIR})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

# cmake -DCMAKE_BUILD_TYPE=DEBUG
# or
# cmake -DCMAKE_BUILD_TYPE=RELEASE
#     RELEASE is the default option (thanks to the next 3 lines)
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE FOO_SOURCE ${FOO_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE FOO_INCLUDE ${FOO_SOURCE_DIR}/*.h)

add_executable (foo ${FOO_SOURCE})

if ($CMAKE_BUILD_TYPE  STREQUAL "RELEASE") # release : act install release
  target_link_libraries(foo agrum)
else() # debug : act install debug
  target_link_libraries(foo agrum-dbg)
endif()
\endverbatim

- a small minimum src/example.cpp :
\code
#include <iostream>

#include <agrum/core/hashTable.h>

int main(void) {
  gum::HashTable<std::string,int> h;

  h.insert("Hello",1);
  h.insert("World",2);

  std::cout<<h<<std::endl;
}
\endcode

- to compile the project (from the project folder)

\verbatim
    mkdir build
    cd build
    cmake ../src/
    make
\endverbatim

- build/foo is the executable.

*/

/// gum is the global namespace for all aGrUM entities
namespace gum {

  /// Internal namespace for aGrUM signaler/listener components
  namespace __sig__ {}

  /// Internal namespace for aGrUM debugging tools
  namespace __debug__ {}

  /// Aggregators are functional description of CPTs
  namespace aggregator {}

  /// Internal namespace for complexity tools (quite empty for now)
  namespace complexity {}

  /// namespace for all credal networks entities
  namespace credal {

    /// namespace for constraint-based description of credal sets
    namespace lp {}
  }

  /// namespace for all particles for approximation inference based on simulation
  namespace particle {}

  /// namespace for all probabilistic relational models entities
  namespace prm {}
}
#endif // CONFIG_H
