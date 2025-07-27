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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







/* INCLUDES */
%{
#include <agrum/bn.h>
%}


%include "typemaps.i"
%include "std_vector.i"
%include "std_string.i"

%include "forUsing.i"

%include <agrum/BN/IBayesNet.h>
%include <agrum/BN/BayesNet.h>
%include <agrum/BN/BayesNetFragment.h>

%include <agrum/BN/algorithms/essentialGraph.h>
%include <agrum/BN/algorithms/MarkovBlanket.h>
%include <agrum/BN/algorithms/structuralComparator.h>

%import <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>
%include <agrum/base/core/approximations/approximationScheme.h>

%import <agrum/BN/inference/tools/relevantTensorsFinderType.h>

%ignore gum::BayesNetInference;
%import <agrum/base/graphicalModels/inference/scheduler/scheduledInference.h>
%include <agrum/BN/inference/tools/BayesNetInference.h>
%include <agrum/BN/inference/lazyPropagation.h>
%include <agrum/BN/inference/ShaferShenoyInference.h>
%include <agrum/BN/inference/variableElimination.h>

%include <agrum/BN/inference/GibbsSampling.h>
%include <agrum/BN/inference/importanceSampling.h>
%include <agrum/BN/inference/weightedSampling.h>
%include <agrum/BN/inference/MonteCarloSampling.h>
%include <agrum/BN/inference/loopySamplingInference.h>

%include <agrum/BN/inference/loopyBeliefPropagation.h>

%import <agrum/BN/algorithms/divergence/BNdistance.h>
%include <agrum/BN/algorithms/divergence/exactBNdistance.h>
%include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>

%import <agrum/base/core/signal/listener.h>
%import <agrum/base/graphs/parts/listeners/diGraphListener.h>
%import <agrum/BN/io/BIF/BIFReader.h>

%import <agrum/base/database/databaseTable.h>
%include <agrum/BN/database/BNDatabaseGenerator.h>

%template (IBayesNet ) gum::IBayesNet<double>;
%template ( BayesNet ) gum::BayesNet<double>;
%template ( BayesNetFragment ) gum::BayesNetFragment<double>;

//%template ( BayesNetInference ) gum::BayesNetInference<double>;
%template ( LazyPropagation ) gum::LazyPropagation<double>;
%template ( ShaferShenoyInference ) gum::ShaferShenoyInference<double>;
%template ( VariableElimination ) gum::VariableElimination<double>;

%template ( GibbsSampling ) gum::GibbsSampling<double>;
%template ( ImportanceSampling ) gum::ImportanceSampling<double>;
%template ( WeightedSampling ) gum::WeightedSampling<double>;
%template ( MonteCarloSampling ) gum::MonteCarloSampling<double>;
%template ( LoopyImportanceSampling ) gum::LoopySamplingInference<double,gum::ImportanceSampling>;
%template ( LoopyWeightedSampling ) gum::LoopySamplingInference<double,gum::WeightedSampling>;
%template ( LoopyGibbsSampling ) gum::LoopySamplingInference<double,gum::GibbsSampling>;
%template ( LoopyMonteCarloSampling ) gum::LoopySamplingInference<double,gum::MonteCarloSampling>;

%template ( LoopyBeliefPropagation ) gum::LoopyBeliefPropagation<double>;

%template ( ExactBNdistance ) gum::ExactBNdistance<double>;
%template ( GibbsBNdistance ) gum::GibbsBNdistance<double>;

%template ( BNDatabaseGenerator) gum::learning::BNDatabaseGenerator<double>;

// learning
%import <agrum/BN/learning/BNLearnUtils/IBNLearner.h>
%include <agrum/BN/learning/BNLearner.h>
%template (BNLearner) gum::learning::BNLearner<double>;
