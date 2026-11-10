import pyagrum as gum
import pyagrum.ctbn as ct

cim = ct.CIM()
cim.add(gum.LabelizedVariable(ct.CIM.varI("A"), "", ["a0", "a1"]))  # the state before a transition
cim.add(gum.LabelizedVariable(ct.CIM.varJ("A"), "", ["a0", "a1"]))  # the state after
cim.add(gum.LabelizedVariable("U", "U", 2))
cim.remove(cim.findVar("U"))  # only a parent can be removed
cim.varNames
# ('A#i', 'A#j')
