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
tr.computeAllCIMs()  # with the arcs of tr.ctbn (none here)
tr.ctbn.CIM("A").toMatrix().tolist()
# [[-0.5, 0.5], [0.5, -0.5]]
