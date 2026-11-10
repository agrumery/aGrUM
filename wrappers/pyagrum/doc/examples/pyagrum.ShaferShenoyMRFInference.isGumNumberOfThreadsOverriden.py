import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.isGumNumberOfThreadsOverriden()
# False
ie.setNumberOfThreads(2)
ie.isGumNumberOfThreadsOverriden()
# True
