# requires: IPython, graphviz
import pyagrum as gum
import pyagrum.lib.notebook as gnb

gnb.showMRF(gum.fastMRF("A--B--C;C--D"), view="graph")
