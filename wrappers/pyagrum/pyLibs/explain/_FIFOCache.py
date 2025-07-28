from collections import OrderedDict
class FIFOCache(OrderedDict):
    def __init__(self, maxlen, *args, **kwargs):
        self.maxlen = maxlen
        super().__init__(*args, **kwargs)

    def __setitem__(self, key, value):
        if key in self:
            del self[key]  # pour mettre à jour l'ordre
        elif len(self) >= self.maxlen:
            self.popitem(last=False)  # retire le plus ancien
        super().__setitem__(key, value)