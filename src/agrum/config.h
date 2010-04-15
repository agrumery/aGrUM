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
    - \ref install_and_use "How to install and use aGrUM library ?"
    - \ref graph_page "How to use graph classes ?"
    - \ref multidim_page "How to use the MultiDim hierarchy ?"
    - \ref instantiation_page "How to use a Instantiation ?"
		- \ref signal_page "How to use signaler/listener in aGrUM ?"

    \defgroup basicstruct_group Basic data structures
    \defgroup graph_group Graph representation
    \defgroup multidim_group Multidimensional data
    \defgroup bn_group Bayesian Networks
    \defgroup learning_group Tools for learning
		\defgroup signal_group Signaler and Listener

  \page install_and_use Installing and using the aGrUM library
    \section Installation

    aGrUM is still in heavy developpement. Do not hesitate to have a look at http://agrum.lip6.fr to see which version you should use. Let's assume that you want install version v1.5 (which does not exist for now).

    To install aGrUm :
    @code
    svn co http://webia.lip6.fr/svn/agrum/tags/v1.5
    make
    sudo make install
    @endcode

    \section using_agrum Using aGrUM

    As a build system, aGrUM uses CMake (http://www.cmake.org). A minimal project with agrum should look like this (for a project foo):
    - in the project folder, a sub-folder <tt> src</tt>,
    - in <tt>src</tt> folder, your <tt>*.{cpp|h|etc.}</tt> files
    - in <tt>src</tt> folder, a file named <tt>CMakeLists.txt</tt> like this one :
@verbatim
project(FOO)
cmake_minimum_required(VERSION 2.8)

# do not forget to change this line if needed ("act install -d...")
set(AGRUM_INSTALLATION_DIRECTORY "/usr")
set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/aGrUM/")

find_package(aGrUM)

if (aGrUM_FOUND)
	include_directories(${AGRUM_INCLUDE_DIR})
	link_directories(${AGRUM_LIB_DIR})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

file(GLOB FOO_SOURCE ${FOO_SOURCE_DIR}/*.cpp)
file(GLOB FOO_INCLUDE ${FOO_SOURCE_DIR}/*.h)

add_executable (foo ${FOO_SOURCE})
target_link_libraries(foo agrum)
@endverbatim

    - to compile the project (from the project folder)
    @code
    <.../foo/> mkdir build
    <.../foo/> cd build
    <.../foo/build/> cmake ../src/
    <.../foo/build/> make
    @endcode

    - <tt>build/foo</tt> is the executable.
 */


#include <iostream>
#include <iomanip>
#include <string>
#include <limits.h>


#include <agrum/core/utils.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>

#include <agrum/core/refPtr.h>
#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>

#include <agrum/graphs/nodeGraphPart.h>
#include <agrum/graphs/mixedGraph.h>

#include <agrum/multidim/variable.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/discretizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>
#include <agrum/BN/io/BIFIO.h>

#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/Gibbs.h>

#endif // CONFIG_H
