# requires: IPython, matplotlib, pydot
import pyagrum as gum
import pyagrum.lib.notebook as gnb

mrf = gum.fastMRF("A--B--C;C--D")
html = gnb.getMRF(mrf, view="factorgraph")
html.startswith("<svg")
# True
