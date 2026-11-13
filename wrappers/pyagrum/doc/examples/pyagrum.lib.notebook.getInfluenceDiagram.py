# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

infdiag = gum.fastID("A->*D->$U<-A")
html = gnb.getInfluenceDiagram(infdiag)
html.startswith("<svg")
# True
