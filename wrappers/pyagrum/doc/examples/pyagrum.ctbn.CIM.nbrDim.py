import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
ctbn.CIM("A").nbrDim()  # A#i and A#j
# 2
ctbn.CIM("B").nbrDim()  # B#i, B#j and A
# 3
