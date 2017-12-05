%extend gum::Instantiation {
  %pythoncode {
  def __setitem__(self,key,item):
    self.chgVal(self.variable(key),item)

  def __getitem__(self,key):
    return self.val(self.variable(key))
  }
};
