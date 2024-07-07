/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GUM_BN_H
#define GUM_BN_H

#include <agrum/base.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFragment.h>

#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/algorithms/MarkovBlanket.h>
#include <agrum/BN/algorithms/structuralComparator.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>
#include <agrum/BN/io/XDSL/XDSLBNReader.h>
#include <agrum/BN/io/XDSL/XDSLBNWriter.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNWriter.h>
#include <agrum/BN/io/UAI/UAIBNReader.h>
#include <agrum/BN/io/UAI/UAIBNWriter.h>

#include <agrum/BN/inference/tools/BayesNetInference.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>

#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/weightedSampling.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/loopySamplingInference.h>

#include <agrum/BN/inference/loopyBeliefPropagation.h>

#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>

#include <agrum/BN/learning/BNLearner.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>

#endif   // GUM_BN_H
