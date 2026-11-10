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
T, M = ct.Trajectory(data).computeStats("A", [])
cim = ct.computeCIMFromStats("A", M, T)
print(cim)
#
#       ║  A#j              │
# A#i   ║a0       │a1       │
# ──────║─────────│─────────│
# a0    ║ -0.5000 │ 0.5000  │
# a1    ║ 0.5000  │ -0.5000 │
