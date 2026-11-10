import pyagrum as gum

bn = gum.fastBN("A->B->C")
loglikelihood = gum.generateSample(bn, 100, "sample.csv")  # the log2-likelihood of the sample
lines = open("sample.csv").read().splitlines()
len(lines)  # a header and 100 rows
# 101
sorted(lines[0].split(","))  # the columns are in random order
# ['A', 'B', 'C']
