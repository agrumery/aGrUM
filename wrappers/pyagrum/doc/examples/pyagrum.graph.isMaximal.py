import pyagrum as gum
import pyagrum.graph as gg

gg.isMaximal(gum.fastMarkedGraph("A->B<->C"))
# True
gg.isMaximal(gum.fastMarkedGraph("A<->B<->C<->D;B->D;C->A"))  # inducing path A<->B<->C<->D
# False
