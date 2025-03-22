/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef GUM_BASE_H
#define GUM_BASE_H

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/base/core/bijection.h>
#include <agrum/base/core/debug.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/hashFunc.h>
#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/list.h>
#include <agrum/base/core/refPtr.h>
#include <agrum/base/core/sequence.h>
#include <agrum/base/core/set.h>
#include <agrum/base/core/staticInitializer.h>
#include <agrum/base/core/threads/threadExecutor.h>
#include <agrum/base/core/threads/threadExecutorOMP.h>
#include <agrum/base/core/threads/threadExecutorSTL.h>
#include <agrum/base/core/threads/threads.h>
#include <agrum/base/core/threads/threadsOMP.h>
#include <agrum/base/core/threads/threadsSTL.h>
#include <agrum/base/core/types.h>
#include <agrum/base/graphicalModels/DAGmodel.h>
#include <agrum/base/graphicalModels/graphicalModel.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduledInference.h>
#include <agrum/base/graphicalModels/UGmodel.h>
#include <agrum/base/graphicalModels/variableNodeMap.h>
#include <agrum/base/graphs/algorithms/MeekRules.h>
#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/cliqueGraph.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/parts/arcGraphPart.h>
#include <agrum/base/graphs/parts/edgeGraphPart.h>
#include <agrum/base/graphs/parts/nodeGraphPart.h>
#include <agrum/base/graphs/PDAG.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/base/multidim/implementations/multiDimAdressable.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/implementations/multiDimContainer.h>
#include <agrum/base/multidim/implementations/multiDimDecorator.h>
#include <agrum/base/multidim/implementations/multiDimImplementation.h>
#include <agrum/base/multidim/implementations/multiDimInterface.h>
#include <agrum/base/multidim/implementations/multiDimReadOnly.h>
#include <agrum/base/multidim/implementations/multiDimWithOffset.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/allDiscreteVariables.h>
#include <agrum/base/variables/discreteVariable.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/IDiscretizedVariable.h>
#include <agrum/base/variables/integerVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/numericalDiscreteVariable.h>
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/base/variables/variable.h>

#include <agrum/base/core/utils_misc.h>

#endif   // GUM_BASE_H
