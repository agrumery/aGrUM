import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
joint = ctbn.CIM("A").amalgamate(ctbn.CIM("B"))  # or ctbn.CIM("A") * ctbn.CIM("B")
sorted(joint.varNames)
# ['A#i', 'A#j', 'B#i', 'B#j']
joint.isIM()  # A is no longer a parent
# True
