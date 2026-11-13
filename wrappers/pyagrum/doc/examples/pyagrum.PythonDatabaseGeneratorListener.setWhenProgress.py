import pyagrum as gum

generator = gum.BNDatabaseGenerator(gum.fastBN("A->B"))
listener = gum.PythonDatabaseGeneratorListener(generator)
progress = []
listener.setWhenProgress(lambda step, duration: progress.append(step))
ll = generator.drawSamples(1000)
len(progress), progress[:3], progress[-1]  # in percent
# (100, [0, 1, 2], 99)
