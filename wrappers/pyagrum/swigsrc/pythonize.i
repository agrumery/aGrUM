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







/* MAKE OPERATIONS MORE PYTHONIC */
%ignore gum::operator<<;
%ignore* ::operator=;
%ignore* ::operator++;
%ignore* ::operator--;
%ignore* ::operator[];

%ignore* ::operator<<;
%ignore* ::operator>>;
%ignore* ::operator!=;



%define ADD_REPR_STR_API(ns,classname)
%extend ns::classname {
  // adding __repr__ and __str__ calling toString
  std::string __repr__() const {
   std::stringstream s;
   s<<"(pyagrum.classname@"<<std::hex<<self<<") "<<self->toString();
   return s.str();
  }
  std::string __str__() const { return self->toString();};
}
%ignore ns::classname::toString;
%enddef

ADD_REPR_STR_API(gum,Tensor<double>);
ADD_REPR_STR_API(gum,Instantiation);

ADD_REPR_STR_API(gum,DiscreteVariable);
ADD_REPR_STR_API(gum,RangeVariable);
ADD_REPR_STR_API(gum,LabelizedVariable);
ADD_REPR_STR_API(gum,IntegerVariable);
ADD_REPR_STR_API(gum,NumericalDiscreteVariable);
ADD_REPR_STR_API(gum,DiscretizedVariable<double>);

ADD_REPR_STR_API(gum,UndiGraph);
ADD_REPR_STR_API(gum,DiGraph);
ADD_REPR_STR_API(gum,DAG);
ADD_REPR_STR_API(gum,MixedGraph);
ADD_REPR_STR_API(gum,PDAG);

ADD_REPR_STR_API(gum,IBayesNet<double>);
ADD_REPR_STR_API(gum,BayesNet<double>);
ADD_REPR_STR_API(gum,InfluenceDiagram<double>);

ADD_REPR_STR_API(gum::credal,CredalNet<double>);

ADD_REPR_STR_API(gum,IMarkovRandomField<double>);
ADD_REPR_STR_API(gum,MarkovRandomField<double>);

ADD_REPR_STR_API(gum::learning,BNLearner<double>);


%ignore *::diffSet;
%ignore gum::Instantiation::setValsFrom;
%ignore gum::randomGenerator;
%ignore gum::_rand_namespace_::generator;

