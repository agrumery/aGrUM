import pyagrum as gum

gum.saveMRF(gum.fastMRF("A--B;B--C"), "model.uai")
mrf = gum.loadMRF("model.uai")
mrf.size()
# 3
len(mrf.factors())
# 2

# a listener is called with the progress of the loading (in %), then with 200 once the MRF is loaded
progress = []
mrf = gum.loadMRF("model.uai", listeners=[progress.append])
progress[:3], progress[-2:]
# ([1, 2, 3], [100, 200])
