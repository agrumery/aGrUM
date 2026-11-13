import pyagrum as gum

generator = gum.BNDatabaseGenerator(gum.fastBN("A->B"))
listener = gum.PythonDatabaseGeneratorListener(generator)
messages = []
listener.setWhenStop(lambda message: messages.append(message))
ll = generator.drawSamples(100)
messages[0].split(" generated")[0]  # followed by the duration and the log2-likelihood
# 'Database of size 100(100)'
