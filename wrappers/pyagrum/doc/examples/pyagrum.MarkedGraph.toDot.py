import pyagrum as gum

g = gum.fastMarkedGraph("A->B;B o-> C")
dot = g.toDot()
print("\n".join(line for line in dot.splitlines() if " -> " in line))
#   0 -> 1 [dir="both", arrowhead="normal", arrowtail="none"];
#   1 -> 2 [dir="both", arrowhead="normal", arrowtail="odot"];
