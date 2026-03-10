# TODO_CLAUDE.md

Actions à réaliser lors d'une prochaine session.

**Règle** : ne jamais traiter un item de cette liste sans l'avoir signalé à l'utilisateur et obtenu son accord explicite.

## C++

- **Intégration `std::string_view`** : parcourir le code C++ (`src/agrum/`) et remplacer autant que possible les `const std::string&` (paramètres en entrée seulement, sans copie retournée) par `std::string_view`. Vérifier la compatibilité avec les appels existants, les bindings SWIG et les typemaps. Priorité aux interfaces publiques fréquemment utilisées.

## SWIG

- **Simplifier la structure des includes `.i`** : analyser l'ordre et la hiérarchie des `%include` dans `pyagrum.i` et les fichiers `aGrUM_wrap_*.i` / `swigsrc/*.i`. Vérifier qu'il n'y a pas de redondances, que la convention "swigsrc avant aGrUM_wrap" est appliquée partout, et proposer une réorganisation si nécessaire.

- **Audit des typemaps manquants** : parcourir les blocs `%extend` et le code inline dans `swigsrc/*.i` qui appellent manuellement des helpers de conversion (ex. `populateHashTableStrStrFromPyDict`, `populateStringSetFromPySequence`, etc.) alors qu'un typemap existe ou pourrait exister pour le type concerné. Remplacer ces appels manuels par le typemap correspondant pour simplifier le code.

- **Audit des `%ignore` redondants** : certains `%ignore` dans `swigsrc/*.i` ciblent peut-être des méthodes/classes que SWIG ignorerait de toute façon (paramètre sans typemap, classe jamais `%include`'d). Candidats à vérifier : `%ignore gum::MultiDimWithOffset/MultiDimImplementation/MultiDimInterface/MultiDimDecorator/MultiDimArray` dans `tensor.i` (sont-elles `%include`'d ?). Pour chaque `%ignore`, vérifier si le retrait produit un warning SWIG ou une API exposée non désirée.
