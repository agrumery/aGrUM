# Imports
from collections.abc import MutableMapping

class Explanation(MutableMapping) :
    def __init__(self, 
                 values, 
                 importances,
                 feature_names,
                 data, 
                 baseline, 
                 func
            ) -> None :
        
        self._values = values
        self.importances = importances
        self.feature_names = feature_names
        self.data = data
        self.baseline = baseline
        self.func = func

    def __getitem__(self, key):
        return self._values[key]

    def __setitem__(self, key, value):
        self._values[key] = value

    def __delitem__(self, key):
        del self._values[key]

    def __iter__(self):
        return iter(self._values)

    def __len__(self):
        return len(self._values)

    def __repr__(self):
        return f"Explanation(values={self._values})"