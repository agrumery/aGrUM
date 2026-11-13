# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

gnb.showInfluenceDiagram(gum.fastID("A->*D->$U<-A"))
