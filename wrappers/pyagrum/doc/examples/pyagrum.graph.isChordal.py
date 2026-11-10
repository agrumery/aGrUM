import pyagrum as gum
import pyagrum.graph as gg

gg.isChordal(gum.fastUndiGraph("A-B-C-D-A"))
# False
gg.isChordal(gum.fastUndiGraph("A-B-C-D-A;A-C"))
# True
