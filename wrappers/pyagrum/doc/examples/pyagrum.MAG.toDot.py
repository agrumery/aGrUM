import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
dot = mag.toDot()
print("\n".join(line for line in dot.splitlines() if " -> " in line))
#   0 -> 1 [dir="both", arrowhead="normal", arrowtail="none"];
#   2 -> 3 [dir="both", arrowhead="none", arrowtail="normal"];
#   1 -> 2 [dir="both", arrowhead="normal", arrowtail="normal"];
