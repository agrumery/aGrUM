import pyagrum as gum
import pyagrum.graph as gg

gg.isAncestral(gum.fastMarkedGraph("A->B<->C"))
# True
gg.isAncestral(gum.fastMarkedGraph("A->B->C;A<->C"))
# False
