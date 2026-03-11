# TODO_CLAUDE.md

Actions à réaliser lors d'une prochaine session.

**Règle** : ne jamais traiter un item de cette liste sans l'avoir signalé à l'utilisateur et obtenu son accord explicite.

## C++

- **Namespaces anonymes** : parcourir le code C++ (`src/agrum/`) et utiliser systématiquement `namespace { ... }` pour toute fonction ou variable de portée interne à un `.cpp` (au lieu de `static` ou d'une exposition accidentelle dans le namespace global). Améliore l'encapsulation et le linkage.

- **Intégration `std::string_view`** : parcourir le code C++ (`src/agrum/`) et remplacer autant que possible les `const std::string&` (paramètres en entrée seulement, sans copie retournée) par `std::string_view`. Vérifier la compatibilité avec les appels existants, les bindings SWIG et les typemaps. Priorité aux interfaces publiques fréquemment utilisées.

## SWIG

- **Simplifier la structure des includes `.i`** : analyser l'ordre et la hiérarchie des `%include` dans `pyagrum.i` et les fichiers `aGrUM_wrap_*.i` / `swigsrc/*.i`. Vérifier qu'il n'y a pas de redondances, que la convention "swigsrc avant aGrUM_wrap" est appliquée partout, et proposer une réorganisation si nécessaire.

- **Audit des typemaps manquants** : parcourir les blocs `%extend` et le code inline dans `swigsrc/*.i` qui appellent manuellement des helpers de conversion (ex. `populateHashTableStrStrFromPyDict`, `populateStringSetFromPySequence`, etc.) alors qu'un typemap existe ou pourrait exister pour le type concerné. Remplacer ces appels manuels par le typemap correspondant pour simplifier le code.

- **Audit des `%ignore` redondants** : certains `%ignore` dans `swigsrc/*.i` ciblent peut-être des méthodes/classes que SWIG ignorerait de toute façon (paramètre sans typemap, classe jamais `%include`'d). Candidats à vérifier : `%ignore gum::MultiDimWithOffset/MultiDimImplementation/MultiDimInterface/MultiDimDecorator/MultiDimArray` dans `tensor.i` (sont-elles `%include`'d ?). Pour chaque `%ignore`, vérifier si le retrait produit un warning SWIG ou une API exposée non désirée.

## Python / pyAgrum

- **Généraliser `_types.py`** : actuellement localisé dans `pyLibs/causal/`, ce fichier contient des types (`NodeId`, `NodeSet`, `NameSet`, etc.) utiles à l'ensemble de `pyLibs/`. Le déplacer à un niveau commun (ex. `pyLibs/_types.py`) et mettre à jour tous les imports. Vérifier qu'aucun type n'est redéfini ailleurs.

- **Compléter les annotations de type dans `pyLibs/`** : parcourir tous les fichiers `wrappers/pyagrum/pyLibs/**/*.py` et ajouter les annotations manquantes sur les signatures (paramètres et retours). S'appuyer sur les types existants dans `causal/_types.py`. Ne pas toucher aux docstrings NumPy existantes.

- **Stubs `.pyi` pour les classes SWIG** : les classes exposées via SWIG ne sont pas visibles par mypy/pyright. Stratégie retenue : stubs comme artefact de build (jamais édités à la main), synchronisation garantie structurellement.
  1. **Génération** : lancer `stubgen -p pyagrum` à chaque `act install pyAgrum`, sortie dans `pyagrum-stubs/`.
  2. **Post-traitement** : script qui remplace les `Any` génériques par les vrais types, en lisant les typemaps (`gum_typemaps.i`) et les annotations existantes dans `pyLibs/`.
  3. **Marqueur** : ajouter `py.typed` dans le package et le déclarer dans la config de build une fois les stubs opérationnels.
  4. **Test CI** : vérifier via `inspect` qu'aucune méthode publique n'a disparu des stubs entre deux builds.
