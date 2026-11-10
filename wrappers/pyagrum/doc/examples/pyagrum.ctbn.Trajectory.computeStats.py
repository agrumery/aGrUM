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
T, M = tr.computeStats("A", [])
T.tolist()  # the time spent in a0 and a1
# [2.0, 2.0]
M.tolist()  # the number of transitions
# [[0.0, 1.0], [1.0, 0.0]]
