/***************************************************************************
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/**
 * @file
 * @brief This file gives access to (too complex) gum::InformationTheory
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/agrum.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>
#include <agrum/tools/graphicalModels/algorithms/informationTheory.h>
#include "helpers.h"

#ifndef AGRUM_INFORMATIONTHEORY_H
#  define AGRUM_INFORMATIONTHEORY_H

class InformationTheory {
  public:
  InformationTheory(gum::LazyPropagation< double >& engine,
                    PyObject*                       Xnames,
                    PyObject*                       Ynames,
                    PyObject*                       Znames = nullptr) {
    gum::NodeSet X;
    gum::NodeSet Y;
    gum::NodeSet Z;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(X,
                                                              Xnames,
                                                              engine.model().variableNodeMap());
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Y,
                                                              Ynames,
                                                              engine.model().variableNodeMap());
    if (Znames)
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Z,
                                                                Znames,
                                                                engine.model().variableNodeMap());
    _bnIT_  = new gum::InformationTheory(engine, X, Y, Z);
    _mrfIT_ = nullptr;
  }


  InformationTheory(gum::ShaferShenoyMRFInference< double >& engine,
                    PyObject*                                Xnames,
                    PyObject*                                Ynames,
                    PyObject*                                Znames = nullptr) {
    gum::NodeSet X;
    gum::NodeSet Y;
    gum::NodeSet Z;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(X,
                                                              Xnames,
                                                              engine.model().variableNodeMap());
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Y,
                                                              Ynames,
                                                              engine.model().variableNodeMap());
    if (Znames)
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(Z,
                                                                Znames,
                                                                engine.model().variableNodeMap());
    _bnIT_  = nullptr;
    _mrfIT_ = new gum::InformationTheory(engine, X, Y, Z);
  }

#  define DEFINE_IT_FUNC(func) \
    double func() { return (_bnIT_ != nullptr) ? _bnIT_->func() : _mrfIT_->func(); }

  DEFINE_IT_FUNC(entropyXY);
  DEFINE_IT_FUNC(entropyX);
  DEFINE_IT_FUNC(entropyY);
  DEFINE_IT_FUNC(entropyXgivenY);
  DEFINE_IT_FUNC(entropyYgivenX);
  DEFINE_IT_FUNC(mutualInformationXY);
  DEFINE_IT_FUNC(variationOfInformationXY);
  DEFINE_IT_FUNC(entropyXYgivenZ);
  DEFINE_IT_FUNC(mutualInformationXYgivenZ);

#  undef DEFINT_IT_FUNC

  ~InformationTheory() {
    delete _bnIT_;
    delete _mrfIT_;
  }

  private:
  gum::InformationTheory< gum::LazyPropagation, double >*          _bnIT_;
  gum::InformationTheory< gum::ShaferShenoyMRFInference, double >* _mrfIT_;
};
#endif   // AGRUM_INFORMATIONTHEORY_H
