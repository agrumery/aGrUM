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
    def posLabel(self,s:str)->int:
      """
      Returns the index of the label `s` in the variable's domain (if the variable is a pyagrum.LabelizedVariable)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.LabelizedVariable
        NotFoundError
          If the variable does not contain the label `s`

      Returns
      -------
        int
          the index of the label `s` in the variable's domain
      """
      try:
        return self.asLabelizedVar().posLabel(s)
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"posLabel not implemented for {self}")
    def isLabel(self,s:str)->bool:
      """
      returns whether s is a label of the variable

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.LabelizedVariable

      Returns
      -------
        bool
          True if s is a label of the variable, False otherwise
      """
      try:
        return self.asLabelizedVar().isLabel(s)
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"isLabel not implemented for {self}")
    ###########
    # Range
    def belongs(self,x:int)->bool:
      """
      returns whether x belongs to the variable domain (if the variable is a pyagrum.RangeVariable)

      Parameters
      ----------
      x : int
          the value for which we want to know if it belongs to the variable domain

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.RangeVariable

      Returns
      -------
        bool
          True if x belongs to the variable domain, False otherwise
      """
      try:
        return self.asRangeVar().belongs(x)
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"belongs not implemented for {self}")
    def minVal(self)->int:
      """
      returns the minimum value of the variable domain (if the variable is a pyagrum.RangeVariable)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.RangeVariable

      Returns
      -------
        int
          the minimum value of the variable domain
      """
      try:
        return self.asRangeVar().minVal()
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"minVal not implemented for {self}")
    def maxVal(self)->int:
      """
      returns the maximum value of the variable domain (if the variable is a pyagrum.RangeVariable)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.RangeVariable

      Returns
      -------
        int
          the maximum value of the variable domain
      """
      try:
        return self.asRangeVar().maxVal()
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"maxVal- not implemented for {self}")
    ###########
    # NumericalDiscreteVariable and/or IntegerVariable
    def numericalDomain(self)->tuple[float,...]:
      """
      Returns the numerical domain of the variable (if the variable is a pyagrum.NumericalDiscreteVariable)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.NumericalDiscreteVariable

      Returns
      -------
        tuple[float,...]
          the numerical domain of the variable
      """
      try:
        return self.asNumericalDiscreteVar().numericalDomain()
      except pyagrum.OperationNotAllowed:
         raise NotImplementedError(f"numericalDomain not implemented for {self}")

    def integerDomain(self)->tuple[int,...]:
      """
      Returns the integer domain of the variable (if the variable is a pyagrum.integerVariable)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.IntegerVariable

      Returns
      -------
        tuple[int,...]
          the integer domain of the variable
      """
      try:
        return self.asIntegerVar().integerDomain()
      except pyagrum.OperationNotAllowed:
        raise NotImplementedError(f"isValue not implemented for {self}")

    def isValue(self,x:float)->bool:
      """
      Returns whether x is a value for the variable

      Parameters
      ----------
      x : float
          the value for which we want to know if it is a value for the variable

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.NumericalDiscreteVariable or a pyagrum.IntegerVariable

      Returns
      -------
        bool
          whether x is a value for the variable, i.e. whether x belongs to the variable domain
      """
      try:
        return self.asNumericalDiscreteVar().isValue(x)
      except pyagrum.OperationNotAllowed:
        try:
          return self.asIntegerVar().isValue(x)
        except pyagrum.OperationNotAllowed:
          raise NotImplementedError(f"isValue not implemented for {self}")

    ###########
    # DiscretizedVariable
    def isTick(self,x:float):
      """
      Returns whether x is a tick for the variable

      Parameters
      ----------
      x : float
          the value for which we want to know if it is a tick for the variable

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.DiscretizedVariable

      Returns
      -------
        bool
          whether x is a tick for the variable, i.e. whether x belongs to the variable's domain
      """
      try:
        return self.asDiscretizedVar().isTick(x)
      except pyagrum.OperationNotAllowed:
        raise NotImplementedError(f"isTick not implemented for {self}")
    def ticks(self)->tuple[float,...]:
      """
      Returns the ticks of the variable

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.DiscretizedVariable

      Returns
      -------
        tuple[float,...]
          the ticks of the variable, i.e. the values that belong to the variable's domain
      """
      try:
        return self.asDiscretizedVar().ticks()
      except pyagrum.OperationNotAllowed:
        raise NotImplementedError(f"ticks not implemented for {self}")
    def isEmpirical(self)->bool:
      """
      Returns whether the variable is empirical (if the variable is a pyagrum.DiscretizedVariable). I.e. whether its limits (max and min) are defined by data and accept any values below or above these limits, contrary to non-empirical discretized variables for which the limits are fixed and values outside of these limits are not accepted)

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.DiscretizedVariable

      Returns
      -------
        bool
          whether the variable is empirical
      """
      try:
        return self.asDiscretizedVar().isEmpirical()
      except pyagrum.OperationNotAllowed:
        raise NotImplementedError(f"isEmpirical not implemented for {self}")
    def tick(self,x:int):
      """
      Returns the x-th tick of the variable (if the variable is a pyagrum.DiscretizedVariable)

      Parameters
      ----------
      x : int
          the index of the tick we want to get

      Raises
      ------
        NotImplementedError
          If the variable is not a pyagrum.DiscretizedVariable
        pyagrum.OutOfBounds
          If x is not a valid index for the ticks of the variable

      Returns
      -------
        float
          the x-th tick of the variable
      """
      try:
        return self.asDiscretizedVar().tick(x)
      except pyagrum.OperationNotAllowed:
        raise NotImplementedError(f"tick not implemented for {self}")
    def draw(self,x):
      try:
        return self.asDiscretizedVar().draw(x)
      except pyagrum.OperationNotAllowed :
        raise NotImplementedError(f"draw not implemented for {self}")
  %}
}

%feature("shadow") gum::LabelizedVariable::addLabel(std::string_view aLabel) %{
def addLabel(self,*args):
    """
    Add a label with a new index (we assume that we will NEVER remove a label).

    Parameters
    ----------
    aLabel : str
        the label to be added to the labelized variable

    Returns
    -------
    pyagrum.LabelizedVariable
        the labelized variable

    Raises
    ------
      pyagrum.DuplicateElement
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
    pyagrum.DiscretizedVariable
        the discretized variable

    Raises
    ------
      pyagrum.DefaultInLabel
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
    pyagrum.IntegerVariable
        the Integer variable

    Raises
    ------
      pyagrum.DuplicateElement
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
    pyagrum.IntegerVariable
        the Integer variable

    Raises
    ------
      pyagrum.DuplicateElement
        If the variable already contains the value
    """
    $action(self,*args)
    return self
%}
