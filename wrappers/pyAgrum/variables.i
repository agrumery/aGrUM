%rename ( index ) gum::DiscretizedVariable<double>::index(const double& tick) const;

%newobject gum::LabelizedVariable::clone(); 

%extend gum::DiscreteVariable {
  %pythoncode %{
    def __getitem__(self,label):   # adding the y() function here
        return self.index(label)
  %}
}

%feature("shadow") gum::LabelizedVariable::addLabel(const std::string aLabel) %{
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

  Warnings
  --------
  DuplicateElement raised is raised if the variable already contains the label
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

  Warnings
  --------
  DefaultInLabel raised if the Tick is already defined
  """
  $action(self,*args)
  return self
%}