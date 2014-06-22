%rename(DiscreteVar) gum::DiscreteVariable;
%rename(LabelizedVar) gum::LabelizedVariable;
%rename(RangeVar) gum::RangeVariable;

%rename ( indexTick ) gum::DiscretizedVariable<double>::index(const double& tick) const;

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

%feature("shadow") gum::DiscretizedVariable<float>::addTick(const float& aTick) %{
def addTick(self,*args):
  $action(self,*args)
  return self
%}
