%rename ( index ) gum::DiscretizedVariable<double>::index(const double& tick) const;

%extend gum::DiscreteVariable {
  %pythoncode %{
    def __getitem__(self,label):   # adding the y() function here
        return self.index(label)
  %}
}

%feature("shadow") gum::LabelizedVariable::addLabel(const std::string aLabel) %{
def addLabel(self,*args):
  $action(self,*args)
  return self
%}

%feature("shadow") gum::DiscretizedVariable<double>::addTick(const double& aTick) %{
def addTick(self,*args):
  $action(self,*args)
  return self
%}
