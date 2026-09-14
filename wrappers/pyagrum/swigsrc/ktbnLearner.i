/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

// Pre-template directives for gum::learning::KTBNLearner and
// gum::learning::KTBNAdaptiveLearner. Must be %include'd BEFORE
// aGrUM_wrap_KTBN.i's %include of KTBNLearner.h/KTBNAdaptiveLearner.h, since
// SWIG only applies a %typemap to declarations parsed after it.

// KTBNLearner::latentVariables() returns by value, KTBNAdaptiveLearner::
// latentVariables() by const reference -- both need their own typemap
// (SWIG typemaps do not implicitly cover a by-value type from a
// reference-to-that-type declaration, or vice versa).
%typemap(out) std::vector<std::pair<std::string,std::string>> {
  $result = PyAgrumHelper::PyTupleFromStrStrVector($1);
}
%typemap(out) const std::vector<std::pair<std::string,std::string>>& {
  $result = PyAgrumHelper::PyTupleFromStrStrVector(*$1);
}

// KTBNAdaptiveLearner::scorePerCandidateK() returns by const reference.
%typemap(out) const std::vector<std::pair<gum::Size,double>>& {
  $result = PyAgrumHelper::PyTupleFromSizeDoubleVector(*$1);
}

// KTBNLearner::state()/KTBNAdaptiveLearner::state() return by value.
%typemap(out) std::vector<std::tuple<std::string,std::string,std::string>> {
  $result = PyAgrumHelper::PyTupleFromStrStrStrVector($1);
}

// KTBNLearner::nbRows() returns by value.
%typemap(out) std::vector<gum::Size> {
  $result = PyAgrumHelper::PyTupleFromSizeVector($1);
}

// Every setter below returns `*this` by reference (C++ method chaining), but
// SWIG's default out-typemap for a reference-to-self wraps the returned
// pointer in a brand-new Python proxy: functionally the same object, but a
// *different* Python identity (`is` fails). %pythonappend forces the Python
// wrapper to return the original `self` instead, mirroring the
// SETPROP_THEN_RETURN_SELF idiom used for BNLearner (swigsrc/learning.i) and
// CHANGE_THEN_RETURN_SELF for Tensor (swigsrc/tensor.i).
%define KTBN_LEARNER_RETURN_SELF(classname, methodname)
%pythonappend classname::methodname %{
        return self
%}
%enddef

%define KTBN_LEARNER_RETURN_SELF_COMMON(classname)
KTBN_LEARNER_RETURN_SELF(classname, useScoreAIC);
KTBN_LEARNER_RETURN_SELF(classname, useScoreBD);
KTBN_LEARNER_RETURN_SELF(classname, useScoreBDeu);
KTBN_LEARNER_RETURN_SELF(classname, useScoreBIC);
KTBN_LEARNER_RETURN_SELF(classname, useScoreLog2Likelihood);
KTBN_LEARNER_RETURN_SELF(classname, useScoreMDL);
KTBN_LEARNER_RETURN_SELF(classname, useGreedyHillClimbing);
KTBN_LEARNER_RETURN_SELF(classname, useExtendedGreedyHillClimbing);
KTBN_LEARNER_RETURN_SELF(classname, useLocalSearchWithTabuList);
KTBN_LEARNER_RETURN_SELF(classname, useMIIC);
KTBN_LEARNER_RETURN_SELF(classname, useNMLCorrection);
KTBN_LEARNER_RETURN_SELF(classname, useMDLCorrection);
KTBN_LEARNER_RETURN_SELF(classname, useNoCorrection);
KTBN_LEARNER_RETURN_SELF(classname, useSmoothingPrior);
KTBN_LEARNER_RETURN_SELF(classname, addForbiddenArc);
KTBN_LEARNER_RETURN_SELF(classname, eraseForbiddenArc);
KTBN_LEARNER_RETURN_SELF(classname, addMandatoryArc);
KTBN_LEARNER_RETURN_SELF(classname, eraseMandatoryArc);
KTBN_LEARNER_RETURN_SELF(classname, addForbiddenIntraSliceArc);
KTBN_LEARNER_RETURN_SELF(classname, eraseForbiddenIntraSliceArc);
KTBN_LEARNER_RETURN_SELF(classname, addForbiddenArcAllSlices);
KTBN_LEARNER_RETURN_SELF(classname, eraseForbiddenArcAllSlices);
KTBN_LEARNER_RETURN_SELF(classname, addNoParentNode);
KTBN_LEARNER_RETURN_SELF(classname, eraseNoParentNode);
KTBN_LEARNER_RETURN_SELF(classname, addNoChildrenNode);
KTBN_LEARNER_RETURN_SELF(classname, eraseNoChildrenNode);
KTBN_LEARNER_RETURN_SELF(classname, addPossibleEdge);
KTBN_LEARNER_RETURN_SELF(classname, erasePossibleEdge);
KTBN_LEARNER_RETURN_SELF(classname, allowArcAdditions);
KTBN_LEARNER_RETURN_SELF(classname, allowArcDeletions);
KTBN_LEARNER_RETURN_SELF(classname, allowArcReversals);
KTBN_LEARNER_RETURN_SELF(classname, setMaxIndegree);
%enddef

KTBN_LEARNER_RETURN_SELF_COMMON(gum::learning::KTBNLearner<double>);
KTBN_LEARNER_RETURN_SELF_COMMON(gum::learning::KTBNAdaptiveLearner<double>);

// KTBNAdaptiveLearner-only chaining setters.
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, useOrderScoreBIC);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, useOrderScoreAIC);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, useOrderScorefNML);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, ignoreMissingSymbols);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, addForbiddenKernelArc);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, eraseForbiddenKernelArc);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, addMandatoryKernelArc);
KTBN_LEARNER_RETURN_SELF(gum::learning::KTBNAdaptiveLearner<double>, eraseMandatoryKernelArc);
