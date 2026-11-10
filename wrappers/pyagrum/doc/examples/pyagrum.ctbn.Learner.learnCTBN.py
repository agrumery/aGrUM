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
learner = ct.Learner(data)  # or the name of a csv file of trajectories
learned = learner.learnCTBN()
sorted(learned.names())
# ['A', 'B']
learned.labels("B")
# ('b0', 'b1')
