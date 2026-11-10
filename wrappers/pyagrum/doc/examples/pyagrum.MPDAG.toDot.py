import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
dot = mpdag.toDot()
print("\n".join(line for line in dot.splitlines() if " -> " in line))
#   0 -> 1 [dir="none"];
#   2 -> 3 [color="red"];
#   1 -> 2 [color="red"];
