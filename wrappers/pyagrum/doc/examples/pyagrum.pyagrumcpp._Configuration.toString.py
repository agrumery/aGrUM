import pyagrum as gum

gum.config.push()
gum.config.reset()
print("\n".join(gum.config.toString().splitlines()[:3]))  # same as print(gum.config)
# [core]
#   maxNumberOfThreads = -1
#   randomSeed = 0
gum.config.pop()
