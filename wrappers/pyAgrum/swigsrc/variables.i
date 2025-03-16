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




%rename ( index ) gum::DiscretizedVariable<double>::index(const double& tick) const;
%ignore **::ticksAsDoubles() const;

%newobject gum::LabelizedVariable::clone();

%extend gum::DiscreteVariable {
  %pythoncode %{
    def __iter__(self):
      """
      Iterate over the labels of the variable

      Yield
      -----
      Tuple[int,str]
        The index of the label and its value
      """
      for i in range(self.domainSize()):
        yield i,self.label(i)

    def __hash__(self):
        return hash(self.name()+self.domain())

    def __getitem__(self,label):   # adding the y() function here
        return self.index(label)

    ###########
    # shortcuts for readonly API from derived classes
    ###########
    # Labelized
    def posLabel(self,s):
      try:
        return self.asLabelizedVar().posLabel(s)
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"posLabel not implemented for {self}")
    def isLabel(self,s):
      try:
        return self.asLabelizedVar().isLabel(s)
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"isLabel not implemented for {self}")
    ###########
    # Range
    def belongs(self,x):
      try:
        return self.asRangeVar().belongs(x)
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"belongs not implemented for {self}")
    def minVal(self):
      try:
        return self.asRangeVar().minVal()
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"minVal not implemented for {self}")
    def maxVal(self):
      try:
        return self.asRangeVar().maxVal()
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"maxVal- not implemented for {self}")
    ###########
    # NumericalDiscrete / Integer
    def numericalDomain(self):
      try:
        return self.asNumericalDiscreteVar().numericalDomain()
      except pyAgrum.base.OperationNotAllowed:
         raise NotImplementedError(f"numericalDomain not implemented for {self}")
    def isValue(self,x):
      try:
        return self.asNumericalDiscreteVar().isValue(x)
      except pyAgrum.base.OperationNotAllowed:
        try:
          return self.asIntegerVar().isValue(x)
        except pyAgrum.base.OperationNotAllowed:
          raise NotImplementedError(f"isValue not implemented for {self}")
    def integerDomain(self):
      try:
        return self.asIntegerVar().integerDomain()
      except pyAgrum.base.OperationNotAllowed:
        raise NotImplementedError(f"isValue not implemented for {self}")
    ###########
    # DiscretizedVariable
    def isTick(self,x):
      try:
        return self.asDiscretizedVar().isTick(x)
      except pyAgrum.base.OperationNotAllowed:
        raise NotImplementedError(f"isTick not implemented for {self}")
    def ticks(self):
      try:
        return self.asDiscretizedVar().ticks()
      except pyAgrum.base.OperationNotAllowed:
        raise NotImplementedError(f"ticks not implemented for {self}")
    def isEmpirical(self):
      try:
        return self.asDiscretizedVar().isEmpirical()
      except pyAgrum.base.OperationNotAllowed:
        raise NotImplementedError(f"isEmpirical not implemented for {self}")
    def tick(self,x):
      try:
        return self.asDiscretizedVar().tick(x)
      except pyAgrum.base.OperationNotAllowed:
        raise NotImplementedError(f"tick not implemented for {self}")
    def draw(self,x):
      try:
        return self.asDiscretizedVar().draw(x)
      except pyAgrum.OperationNotAllowed :
        raise NotImplementedError(f"draw not implemented for {self}")
  %}
}

%feature("shadow") gum::LabelizedVariable::addLabel(const std::string& aLabel) %{
def addLabel(self,*args):
    """
    Add a label with a new index (we assume that we will NEVER remove a label).

    Parameters
    ----------
    aLabel : str
        the label to be added to the labelized variable

    Returns
    -------
    pyAgrum.LabelizedVariable
        the labelized variable

    Raises
    ------
      pyAgrum.DuplicateElement
        If the variable already contains the label
    """
    $action(self,*args)
    return self
%}

%feature("shadow") gum::DiscretizedVariable<double>::addTick(const double& aTick) %{
def addTick(self,*args):
    """
    Parameters
    ----------
    aTick : double
        the Tick to be added

    Returns
    -------
    pyAgrum.DiscretizedVariable
        the discretized variable

    Raises
    ------
      pyAgrum.DefaultInLabel
        If the tick is already defined
    """
    $action(self,*args)
    return self
%}

%feature("shadow") gum::IntegerVariable::addValue(int value) %{
def addValue(self,*args):
    """
    Add a value to the list of values for the variable.

    Parameters
    ----------
    value : int
        the new value

    Returns
    -------
    pyAgrum.IntegerVariable
        the Integer variable

    Raises
    ------
      pyAgrum.DuplicateElement
        If the variable already contains the value
    """
    $action(self,*args)
    return self
%}


%feature("shadow") gum::NumericalDiscreteVariable::addValue(double value) %{
def addValue(self,*args):
    """
    Add a value to the list of values for the variable.

    Parameters
    ----------
    value : double
        the new value

    Returns
    -------
    pyAgrum.IntegerVariable
        the Integer variable

    Raises
    ------
      pyAgrum.DuplicateElement
        If the variable already contains the value
    """
    $action(self,*args)
    return self
%}
