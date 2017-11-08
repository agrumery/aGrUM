%rename ( index ) gum::DiscretizedVariable<double>::index(const double& tick) const;

%newobject gum::LabelizedVariable::clone(); 

%extend gum::DiscreteVariable {
  %pythoncode %{
    def __getitem__(self,label):   # adding the y() function here
        return self.index(label)
  %}
}