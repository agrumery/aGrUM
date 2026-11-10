import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
inst = ctbn.CIM("B").instantiation()
inst.todict(withLabels=True)
# {'B#j': 'b0', 'B#i': 'b0', 'A': 'a0'}
