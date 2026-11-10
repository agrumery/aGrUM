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
tr = ct.Trajectory(data)
cim = tr.ctbn.CIM("B")
Bi, Bj, A = cim.findVar("B#i"), cim.findVar("B#j"), tr.ctbn.variable("A")
Tx = gum.Tensor().add(Bi).fillWith(0)
Txy = gum.Tensor().add(Bi).add(A).fillWith(0)
Mxy = gum.Tensor().add(A).add(Bi).add(Bj).fillWith(0)
tr.setStatsForTests("B", "A", {}, Tx, Txy, Mxy)
Tx.tolist()
# [2.0, 2.0]
Txy.tolist()
# [[1.0, 1.0], [1.0, 1.0]]
