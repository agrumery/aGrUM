import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
# 0
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
# 1
ctbn.CIM("A").varNames
# ('A#j', 'A#i')
