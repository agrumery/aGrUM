# requires: IPython
import pyagrum as gum
import pyagrum.ctbn as ct

ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
dot = ctbn.toDot()  # a Graphviz string
'"A"->"B"' in dot
# True
