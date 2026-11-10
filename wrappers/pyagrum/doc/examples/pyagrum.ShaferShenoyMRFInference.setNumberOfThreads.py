import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setNumberOfThreads(2)
ie.getNumberOfThreads()
# 2
ie.isGumNumberOfThreadsOverriden()
# True
ie.setNumberOfThreads(0)  # back to aGrUM's default number of threads
ie.isGumNumberOfThreadsOverriden()
# False
