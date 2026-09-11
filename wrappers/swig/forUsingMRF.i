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

// MRF-owned %extend blocks, split out of forUsing.i -- see forUsingBN.i for why:
// forUsing.i's blanket inclusion by every module caused each module to redundantly
// reinstantiate every other module's %extend blocks. Only aGrUM_wrap_MRF.i
// %include's this file. ADD_PARALLELIZED_INFERENCE_API/ADD_MONOTARGET_INFERENCE_API
// themselves stay %define-d in forUsing.i since ID's own chain depends on them too.

ADD_PARALLELIZED_INFERENCE_API(gum::ShaferShenoyMRFInference<double>)

%define ADD_MRF_INFERENCE_API(classname...)
ADD_MONOTARGET_INFERENCE_API (gum::MarginalTargetedMRFInference<double>,classname)
%extend classname {
   const IMarkovRandomField<double>& MRF() const { return self->gum::MarginalTargetedMRFInference<double>::MRF(); }
}
%enddef

%define ADD_JOINT_MRF_INFERENCE_API(classname)
ADD_MRF_INFERENCE_API(classname)
%extend classname {
  const Tensor<double> posterior( const NodeId var ) {
    return self->JointTargetedMRFInference<double>::posterior(var);
  }
  const Tensor<double> posterior( const std::string nodeName ) {
    return self->JointTargetedMRFInference<double>::posterior(nodeName);
  }
  void eraseAllTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllTargets();
  }
  void eraseAllJointTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllJointTargets();
  }
  void eraseAllMarginalTargets() {
    self->gum::JointTargetedMRFInference<double>::eraseAllMarginalTargets();
  }

  gum::Size nbrJointTargets() {
    return self->gum::JointTargetedMRFInference<double>::nbrJointTargets();
  }

  Tensor<double> evidenceJointImpact(const NodeSet& targets,const NodeSet& evs){
    return self->gum::JointTargetedMRFInference<double>::evidenceJointImpact(targets,evs);
  }
  Tensor<double> evidenceJointImpact(const std::vector<std::string>& targets,const std::vector<std::string>& evs){
   return self->gum::JointTargetedMRFInference<double>::evidenceJointImpact(targets,evs);
  }
}
%enddef
ADD_JOINT_MRF_INFERENCE_API(gum::ShaferShenoyMRFInference<double>)
