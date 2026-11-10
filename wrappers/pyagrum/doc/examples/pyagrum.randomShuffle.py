import pyagrum as gum

gum.initRandom(42)
names = ["A", "B", "C", "D", "E"]
gum.randomShuffle(names)  # in place
names
# ['B', 'D', 'E', 'C', 'A']
sorted(names)
# ['A', 'B', 'C', 'D', 'E']
gum.initRandom(42)  # same seed: same order, on every platform
other = ["A", "B", "C", "D", "E"]
gum.randomShuffle(other)
other == names
# True
