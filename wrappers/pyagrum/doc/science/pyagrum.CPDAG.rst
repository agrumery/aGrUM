A CPDAG (completed partially directed acyclic graph, or essential graph) represents a Markov equivalence class of
DAGs: two DAGs are Markov equivalent iff they have the same skeleton and the same v-structures
:cite:`verma1990equivalence`. The CPDAG has an arc for each compelled arc and an undirected edge for each reversible
one; it is a chain graph whose chain components are chordal and whose arcs are strongly protected
:cite:`andersson1997characterization`. It is built from a DAG by the algorithm Find-Compelled
:cite:`chickering1995transformational`; a PDAG is checked to be a CPDAG through one of its consistent extensions
:cite:`wienobst2021extendability`. Its separation is the d-separation of any DAG of the class; it is also the
c-separation of its chain graph, which is complete :cite:`studeny1998chain`, and on such a chain graph the LWF and AMP
interpretations coincide :cite:`andersson1996alternative`.
