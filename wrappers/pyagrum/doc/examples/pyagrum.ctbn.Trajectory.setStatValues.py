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
cim = tr.ctbn.CIM("A")
Tx = gum.Tensor().add(cim.findVar("A#i")).fillWith(0)
Mx = gum.Tensor().add(cim.findVar("A#i")).add(cim.findVar("A#j")).fillWith(0)
tr.setStatValues("A", {}, Tx, Mx)
Tx.tolist()
# [2.0, 2.0]
Mx.tolist()
# [[0.0, 1.0], [1.0, 0.0]]
