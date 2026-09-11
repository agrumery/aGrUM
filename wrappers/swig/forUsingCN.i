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

// CN-owned %extend blocks, split out of forUsing.i -- see forUsingBN.i for why:
// forUsing.i's blanket inclusion by every module caused each module to redundantly
// reinstantiate every other module's %extend blocks. Only aGrUM_wrap_CN.i %include's
// this file.

ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::credal::CNMonteCarloSampling<double>)
ADD_APPROXIMATIONSCHEME_API(gum::ApproximationScheme,gum::credal::CNLoopyPropagation<double>)

%define ADD_CREDALINFERENCEENGINE_API(classname)
%extend classname  {
  void setRepetitiveInd(const bool flag) {
    self->gum::credal::InferenceEngine<double>::setRepetitiveInd(flag);
  }
 Tensor<double> marginalMax ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(id);
  }
  Tensor<double> marginalMin ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(id);
  }
  Tensor<double> marginalMax ( const std::string name ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(name);
  }
  Tensor<double> marginalMin ( const std::string name) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(name);
  }
  void insertModalsFile ( const std::string& path ) {
    self->gum::credal::InferenceEngine<double>::insertModalsFile(path);
  }
  const std::vector< double >& dynamicExpMax ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMax(varName);
  }
  const std::vector< double >& dynamicExpMin ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMin(varName);
  }
  void eraseAllEvidence() {
    self->gum::credal::InferenceEngine<double>::eraseAllEvidence();
  }
  //######## EVIDENCE ##########
  //############################
  void addEvidence( const NodeId id, const Idx val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,val);
  }
  void addEvidence( const std::string& nodeName, const Idx val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,val);
  }
  void addEvidence( const NodeId id, const std::string& val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,val);
  }

  void addEvidence( const std::string& nodeName, const std::string& val ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,val);
  }

  void addEvidence( const NodeId id,const std::vector<double>& vals ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(id,vals);
  }
  void addEvidence( const std::string& nodeName,
                   const std::vector<double>& vals ) {
    self->gum::credal::InferenceEngine<double>::addEvidence(nodeName,vals);
  }
  void addEvidence(const gum::Tensor<double>& p) {
    self->gum::credal::InferenceEngine<double>::addEvidence(p);
  }
}
%enddef
ADD_CREDALINFERENCEENGINE_API(gum::credal::CNMonteCarloSampling<double>)
ADD_CREDALINFERENCEENGINE_API(gum::credal::CNLoopyPropagation<double>)

%define ADD_CN_INFERENCE_API(classname...)
%extend classname {
  const CredalNet< double >& CN() const { return self->gum::credal::InferenceEngine<double>::credalNet(); }
}
%enddef
ADD_CN_INFERENCE_API(gum::credal::CNMonteCarloSampling<double>)
ADD_CN_INFERENCE_API(gum::credal::CNLoopyPropagation<double>)
