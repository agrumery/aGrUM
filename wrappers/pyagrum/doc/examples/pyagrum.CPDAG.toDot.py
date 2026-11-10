import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
dot = cpdag.toDot()
"cluster" in dot  # the chain component {A, B}
# True
print("\n".join(line for line in dot.splitlines() if " -> " in line))
#   1 -> 2 [color="red"];
#   3 -> 2 [color="red"];
#   0 -> 1 [dir="none"];
