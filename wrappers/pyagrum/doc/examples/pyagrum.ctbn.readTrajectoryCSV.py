import pyagrum.ctbn as ct

with open("traj.csv", "w") as f:
  f.write("IdSample,time,var,state\n0,0,A,a0\n0,1.5,A,a0\n0,3.0,A,a1\n")
ct.readTrajectoryCSV("traj.csv")
# {0: [(0.0, 'A', 'a0'), (1.5, 'A', 'a0'), (3.0, 'A', 'a1')]}
