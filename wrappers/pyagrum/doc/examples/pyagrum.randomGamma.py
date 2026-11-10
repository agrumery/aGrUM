import pyagrum as gum

gum.initRandom(42)
round(gum.randomGamma(2.5), 4)  # the same value on every platform for this seed
# 1.4801
values = [gum.randomGamma(2.5) for _ in range(10000)]
min(values) > 0
# True
abs(sum(values) / len(values) - 2.5) < 0.1  # the mean of Gamma(2.5, 1) is 2.5
# True
