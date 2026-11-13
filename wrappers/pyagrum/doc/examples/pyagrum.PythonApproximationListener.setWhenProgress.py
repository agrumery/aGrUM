import pyagrum as gum

ie = gum.GibbsSampling(gum.fastBN("A->B->C;A->C"))
ie.disableEpsilon()
ie.disableMinEpsilonRate()
ie.setMaxIter(2000)
ie.setPeriodSize(500)
listener = gum.PythonApproximationListener(ie)
steps = []
listener.setWhenProgress(lambda step, error, duration: steps.append(step))
ie.makeInference()
steps  # a notification every 500 iterations (the burn-in included)
# [300, 800, 1300, 1800]
