import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X2;X1->X2;C->X2;X0->X1"))
print(model.summaryGraph())  # one node per process, arcs labelled by their lag
# digraph KTBN {
#   rankdir=LR;
#   node [color="#000000", fillcolor=white, style=filled];
#
#   "C";
#   "X";
#
#   "C" -> "X";
#   "X" -> "X" [label="1"];
#   "X" -> "X" [label="2"];
# }
