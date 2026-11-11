import pyagrum as gum

gum.saveBN(gum.fastBN("A->B<-C"), "model.bif")
bn = gum.loadBN("model.bif")
sorted(bn.names())
# ['A', 'B', 'C']
bn.sizeArcs()
# 2

# a listener is called with the progress of the loading (in %), then with 200 once the BN is loaded
progress = []
bn = gum.loadBN("model.bif", listeners=[progress.append])
progress[:3], progress[-2:]
# ([1, 2, 3], [100, 200])
