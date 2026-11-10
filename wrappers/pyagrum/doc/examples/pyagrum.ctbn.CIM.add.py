import pyagrum as gum
import pyagrum.ctbn as ct

cim = ct.CIM()
cim.add(gum.LabelizedVariable(ct.CIM.varI("A"), "", ["a0", "a1"]))  # the state before a transition
cim.add(gum.LabelizedVariable(ct.CIM.varJ("A"), "", ["a0", "a1"]))  # the state after
cim.add(gum.LabelizedVariable("U", "U", 2))  # a parent
cim.varNames
# ('A#i', 'A#j', 'U')
