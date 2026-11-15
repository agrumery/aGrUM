import pyagrum as gum

gum.initRandom(42)
bn = gum.fastBN("A->B<-C<-D->E<-F->G->A")
ie = gum.LazyPropagation(bn)
it = gum.InformationTheory(ie, "A", ["B", "G"], ["C"])
print(f"Entropy(A)={it.entropyX():.4f}")
# Entropy(A)=0.9936
print(f"MutualInformation(A;B,G)={it.mutualInformationXY():.4f}")
# MutualInformation(A;B,G)=0.1334
print(f"MutualInformation(A;B,G|C)={it.mutualInformationXYgivenZ():.4f}")
# MutualInformation(A;B,G|C)=0.1463
print(f"VariationOfInformation(A;B,G)={it.variationOfInformationXY():.4f}")
# VariationOfInformation(A;B,G)=2.7072
