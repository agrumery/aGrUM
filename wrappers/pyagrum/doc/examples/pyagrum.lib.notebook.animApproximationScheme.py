import matplotlib.pyplot as plt

import pyagrum as gum
import pyagrum.lib.notebook as gnb

gum.initRandom(42)
bn = gum.fastBN("A->B->C;A->D->C")
ie = gum.GibbsSampling(bn)
ie.setEpsilon(1e-2)
plt.figure()
gnb.animApproximationScheme(ie)  # in a notebook, the figure is updated at each step
ie.makeInference()
len(plt.gca().get_lines()) > 0  # the history of the approximation is drawn
# True
plt.gca().get_title().startswith(ie.messageApproximationScheme())
# True
