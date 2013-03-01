
%extend gum::DiscretizedVariable { //XXX hack, rename will be better
    gum::Idx index(const float& aTarget) const {
        return (*self)[aTarget];
    }
}
%rename(DiscreteVar) gum::DiscreteVariable;
%rename(LabelizedVar) gum::LabelizedVariable;
%rename(RangeVar) gum::RangeVariable;

/* remove __disown__() since it causes an error (str has no __disown__ method)...
%pythonprepend gum::LabelizedVariable::addLabel %{
        args = tuple([args[0].__disown__()]) + args[1:]
%}
%pythonprepend gum::DiscretizedVariable::addTick %{
        args = tuple([args[0].__disown__()]) + args[1:]
%}
*/
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
