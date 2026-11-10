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
Tx, Txy, Mxy = tr.computeStatsForTests("B", "A", [])
Tx.tolist()
# [2.0, 2.0]
Txy.names
# ('B#i', 'A')
Txy.tolist()  # time spent in each state of B, for each value of A
# [[1.0, 1.0], [1.0, 1.0]]
