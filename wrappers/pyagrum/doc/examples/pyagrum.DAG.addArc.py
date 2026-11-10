import pyagrum as gum

dag = gum.DAG()
dag.addNodes(2)
# {0, 1}
dag.addArc(0, 1)
try:
  dag.addArc(1, 0)  # would create a cycle
except gum.InvalidDirectedCycle:
  print("cycle refused")
dag.arcs()
# {(0, 1)}
