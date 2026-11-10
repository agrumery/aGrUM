import pyagrum as gum

section, option, meta, default = next(gum.config.schema())
(section, option, meta.kind, default)
# ('core', 'maxNumberOfThreads', 'int', '-1')
meta.doc
# 'default maximum number of threads used by aGrUM (-1 = adapted to the machine)'
