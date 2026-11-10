# requires: matplotlib
import pyagrum as gum
import pyagrum.ctbn as ct

data = {
  0: [
    (0.0, "A", "a0"),
    (0.0, "B", "b0"),
    (1.0, "A", "a0"),
    (2.0, "B", "b0"),
    (3.0, "A", "a1"),
    (4.0, "A", "a0"),
    (4.0, "B", "b1"),
  ]
}
ctbn = ct.CTBN()
ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
ctbn.addArc("A", "B")
ct.plotFollowVar(ctbn.variable("A"), data, timeHorizon=4.0, N=8)
