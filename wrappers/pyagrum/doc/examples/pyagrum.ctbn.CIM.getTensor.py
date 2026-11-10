import numpy as np

import pyagrum as gum
import pyagrum.ctbn as ct

cim = ct.CIM()
cim.add(gum.LabelizedVariable(ct.CIM.varI("A"), "", ["a0", "a1"]))  # the state before a transition
cim.add(gum.LabelizedVariable(ct.CIM.varJ("A"), "", ["a0", "a1"]))  # the state after
cim.fromMatrix(np.array([[-2.0, 2.0], [1.0, -1.0]]))
p = cim.getTensor()  # a copy
p.names
# ('A#i', 'A#j')
