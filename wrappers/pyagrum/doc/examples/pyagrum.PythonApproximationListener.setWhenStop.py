import pyagrum as gum

ie = gum.GibbsSampling(gum.fastBN("A->B->C;A->C"))
ie.disableEpsilon()
ie.disableMinEpsilonRate()
ie.setMaxIter(2000)
listener = gum.PythonApproximationListener(ie)
messages = []
listener.setWhenStop(lambda message: messages.append(message))
ie.makeInference()
messages
# ['stopped with max iteration=2000']
