#include <agrum/CN/credalNet.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/utils_string.h>

namespace gum {
  namespace credal {

    template <typename GUM_SCALAR> CredalNet<GUM_SCALAR>::CredalNet() {
      __initParams();

      __src_bn = BayesNet<GUM_SCALAR>();
      __src_bn_min = BayesNet<GUM_SCALAR>();
      __src_bn_max = BayesNet<GUM_SCALAR>();

      GUM_CONSTRUCTOR(CredalNet);
    }

    template <typename GUM_SCALAR>
    NodeId CredalNet<GUM_SCALAR>::addNode(const std::string &name,
                                          const unsigned long &card) {
      LabelizedVariable var(name, "node " + name, card);

      NodeId a = __src_bn.add(var);
      NodeId b = __src_bn_min.add(var);
      NodeId c = __src_bn_max.add(var);

      if (a != b || a != c /*|| b != c*/)
        GUM_ERROR(OperationNotAllowed,
                  "addNode : not the same id over all networks : " << a << ", " << b
                                                                   << ", " << c);

      return a;
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::addArc(const NodeId &tail, const NodeId &head) {
      __src_bn.addArc(tail, head);
      __src_bn_min.addArc(tail, head);
      __src_bn_max.addArc(tail, head);
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::setCPTs(
        const NodeId &id,
        const std::vector<std::vector<std::vector<GUM_SCALAR>>> &cpt) {
      const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(id));

      auto var_dSize = __src_bn.variable(id).domainSize();
      auto entry_size = potential->domainSize() / var_dSize;

      if (cpt.size() != entry_size)
        GUM_ERROR(SizeError,
                  "setCPTs : entry sizes of cpts does not match for node id : "
                      << id << " : " << cpt.size() << " != " << entry_size);

      for (const auto &cset : cpt) {
        if (cset.size() == 0)
          GUM_ERROR(SizeError,
                    "setCPTs : vertices in credal set does not match for node id : "
                        << id << " with 0 vertices");

        for (const auto &vertex : cset) {
          if (vertex.size() != var_dSize)
            GUM_ERROR(
                SizeError,
                "setCPTs : variable modalities in cpts does not match for node id : "
                    << id << " with vertex " << vertex << " : " << vertex.size()
                    << " != " << var_dSize);

          GUM_SCALAR sum = 0;

          for (const auto &prob : vertex) {
            sum += prob;
          }

          if (std::fabs(sum - 1) > 1e-6)
            GUM_ERROR(
                CPTNoSumTo1,
                "setCPTs : a vertex coordinates does not sum to one for node id : "
                    << id << " with vertex " << vertex);
        }
      }

      __credalNet_src_cpt.insert(id, cpt);
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::setCPT(const NodeId &id, unsigned long int &entry,
                                  const std::vector<std::vector<GUM_SCALAR>> &cpt) {
      const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(id));

      auto var_dSize = __src_bn.variable(id).domainSize();
      auto entry_size = potential->domainSize() / var_dSize;

      if (entry >= entry_size)
        GUM_ERROR(SizeError, "setCPT : entry is greater or equal than entry size "
                             "(entries start at 0 up to entry_size - 1) : "
                                 << entry << " >= " << entry_size);

      if (cpt.size() == 0)
        GUM_ERROR(SizeError, "setCPT : empty credal set for entry : " << entry);

      for (const auto &vertex : cpt) {
        if (vertex.size() != var_dSize)
          GUM_ERROR(
              SizeError,
              "setCPT : variable modalities in cpts does not match for node id : "
                  << id << " with vertex " << vertex << " at entry " << entry
                  << " : " << vertex.size() << " != " << var_dSize);

        GUM_SCALAR sum = 0;

        for (const auto &prob : vertex) {
          sum += prob;
        }

        if (std::fabs(sum - 1) > 1e-6)
          GUM_ERROR(
              CPTNoSumTo1,
              "setCPT : a vertex coordinates does not sum to one for node id : "
                  << id << " at entry " << entry << " with vertex " << vertex);
      }

      // !! auto does NOT use adress (if available) unless explicitly asked !!
      auto &node_cpt = __credalNet_src_cpt.getWithDefault(
          id, std::vector<std::vector<std::vector<GUM_SCALAR>>>(entry_size));

      if (node_cpt[entry].size() != 0)
        GUM_ERROR(DuplicateElement, "setCPT : vertices of entry id "
                                        << entry
                                        << " already set to : " << node_cpt[entry]
                                        << ", cannot insert : " << cpt);

      node_cpt[entry] = cpt;

      ///__credalNet_src_cpt.set ( id, node_cpt );
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::setCPT(const NodeId &id, Instantiation ins,
                                  const std::vector<std::vector<GUM_SCALAR>> &cpt) {
      const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(id));

      auto var_dSize = __src_bn.variable(id).domainSize();
      auto entry_size = potential->domainSize() / var_dSize;

      // to be sure of entry index reorder ins according to the bayes net potentials
      // ( of the credal net )
      // it WONT throw an error if the sequences are not equal not because of order
      // but content, so we double check (before & after order correction)
      // beware of slaves & master
      Instantiation ref(potential);
      ref.forgetMaster();

      ins.forgetMaster();

      const auto &vseq = ref.variablesSequence();

      if (ins.variablesSequence() != vseq) {
        ins.reorder(ref);

        if (ins.variablesSequence() != vseq)
          GUM_ERROR(OperationNotAllowed,
                    "setCPT : instantiation : "
                        << ins << " is not valid for node id " << id
                        << " which accepts instantiations such as (order is not "
                           "important) : " << ref);
      }

      unsigned long int entry = 0, jump = 1;

      for (unsigned int i = 0, end = ins.nbrDim(); i < end; i++) {
        if (__src_bn.nodeId(ins.variable(i)) == id)
          continue;

        entry += ins.val(i) * jump;

        jump *= ins.variable(i).domainSize();
      }

      if (entry >= entry_size)
        GUM_ERROR(SizeError, "setCPT : entry is greater or equal than entry size "
                             "(entries start at 0 up to entry_size - 1) : "
                                 << entry << " >= " << entry_size);

      if (cpt.size() == 0)
        GUM_ERROR(SizeError, "setCPT : empty credal set for entry : " << entry);

      for (const auto &vertex : cpt) {
        if (vertex.size() != var_dSize)
          GUM_ERROR(
              SizeError,
              "setCPT : variable modalities in cpts does not match for node id : "
                  << id << " with vertex " << vertex << " at entry " << entry
                  << " : " << vertex.size() << " != " << var_dSize);

        GUM_SCALAR sum = 0;

        for (const auto &prob : vertex) {
          sum += prob;
        }

        if (std::fabs(sum - 1) > 1e-6)
          GUM_ERROR(
              CPTNoSumTo1,
              "setCPT : a vertex coordinates does not sum to one for node id : "
                  << id << " at entry " << entry << " with vertex " << vertex);
      }

      auto &node_cpt = __credalNet_src_cpt.getWithDefault(
          id, std::vector<std::vector<std::vector<GUM_SCALAR>>>(entry_size));

      if (node_cpt[entry].size() != 0)
        GUM_ERROR(DuplicateElement, "setCPT : vertices of entry : "
                                        << ins << " id " << entry
                                        << " already set to : " << node_cpt[entry]
                                        << ", cannot insert : " << cpt);

      node_cpt[entry] = cpt;

      ///__credalNet_src_cpt.set ( id, node_cpt );
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::fillConstraints(const NodeId &id,
                                           const std::vector<GUM_SCALAR> &lower,
                                           const std::vector<GUM_SCALAR> &upper) {
      try {
        __src_bn_min.cpt(id).fillWith(lower);
        __src_bn_max.cpt(id).fillWith(upper);
      } catch (const SizeError &err) {
        GUM_ERROR(SizeError,
                  "fillConstraints : sizes does not match in fillWith for node id : "
                      << id);
      }
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::fillConstraint(const NodeId &id, unsigned long int &entry,
                                          const std::vector<GUM_SCALAR> &lower,
                                          const std::vector<GUM_SCALAR> &upper) {
      Potential<GUM_SCALAR> *const potential_min(
          const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_min.cpt(id)));
      Potential<GUM_SCALAR> *const potential_max(
          const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_max.cpt(id)));

      auto var_dSize = __src_bn.variable(id).domainSize();

      if (lower.size() != var_dSize || upper.size() != var_dSize)
        GUM_ERROR(
            SizeError,
            "setCPT : variable modalities in cpts does not match for node id : "
                << id << " with sizes of constraints : ( " << lower.size() << " || "
                << upper.size() << " ) != " << var_dSize);

      auto entry_size = potential_min->domainSize() / var_dSize;

      if (entry >= entry_size)
        GUM_ERROR(SizeError, "setCPT : entry is greater or equal than entry size "
                             "(entries start at 0 up to entry_size - 1) : "
                                 << entry << " >= " << entry_size);

      Instantiation min(potential_min);
      Instantiation max(potential_max);
      min.setFirst();
      max.setFirst();

      unsigned long int pos = 0;

      while (pos != entry) {
        ++min;
        ++max;
        ++pos;
      }

      for (Size i = 0; i < var_dSize; i++) {
        potential_min->set(min, lower[i]);
        potential_max->set(max, upper[i]);
        ++min;
        ++max;
      }
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::fillConstraint(const NodeId &id, Instantiation ins,
                                          const std::vector<GUM_SCALAR> &lower,
                                          const std::vector<GUM_SCALAR> &upper) {
      const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(id));
      /*
      auto var_dSize = __src_bn.variable ( id ).domainSize();
      auto entry_size = potential->domainSize() / var_dSize;
      */
      // to be sure of entry index reorder ins according to the bayes net potentials
      // ( of the credal net )
      // it WONT throw an error if the sequences are not equal not because of order
      // but content, so we double check (before & after order correction)
      // beware of slaves & master
      Instantiation ref(potential);
      ref.forgetMaster();

      ins.forgetMaster();

      const auto &vseq = ref.variablesSequence();

      if (ins.variablesSequence() != vseq) {
        ins.reorder(ref);

        if (ins.variablesSequence() != vseq)
          GUM_ERROR(OperationNotAllowed,
                    "setCPT : instantiation : "
                        << ins << " is not valid for node id " << id
                        << " which accepts instantiations such as (order is not "
                           "important) : " << ref);
      }

      unsigned long int entry = 0, jump = 1;

      for (unsigned int i = 0, end = ins.nbrDim(); i < end; i++) {
        if (__src_bn.nodeId(ins.variable(i)) == id)
          continue;

        entry += ins.val(i) * jump;

        jump *= ins.variable(i).domainSize();
      }

      /*
      if ( entry >= entry_size )
        GUM_ERROR ( SizeError, "setCPT : entry is greater or equal than entry size
      (entries start at 0 up to entry_size - 1) : " << entry << " >= " << entry_size
      );

      if ( lower.size() != var_dSize || upper.size() != var_dSize )
        GUM_ERROR ( SizeError, "setCPT : variable modalities in cpts does not match
      for node id : " << id << " with sizes of constraints : ( "<< lower.size() << "
      || " << upper.size() << " ) != " << var_dSize );
      */
      fillConstraint(id, entry, lower, upper);
    }

    ////////////////////////////////////////////////
    /// bnet accessors / shortcuts

    template <typename GUM_SCALAR>
    INLINE Instantiation CredalNet<GUM_SCALAR>::instantiation(const NodeId &id) {
      return Instantiation(__src_bn.cpt(id));
    }

    template <typename GUM_SCALAR>
    INLINE Size CredalNet<GUM_SCALAR>::domainSize(const NodeId &id) {
      return __src_bn.variable(id).domainSize();
    }

    ///////////////////////////////////////////////

    template <typename GUM_SCALAR>
    CredalNet<GUM_SCALAR>::CredalNet(const std::string &src_min_num,
                                     const std::string &src_max_den) {
      try {
        __initParams();
        __initCNNets(src_min_num, src_max_den);
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        throw(err);
      }

      GUM_CONSTRUCTOR(CredalNet);
    }

    template <typename GUM_SCALAR>
    CredalNet<GUM_SCALAR>::CredalNet(const BayesNet<GUM_SCALAR> &src_min_num,
                                     const BayesNet<GUM_SCALAR> &src_max_den) {
      try {
        __initParams();
        __initCNNets(src_min_num, src_max_den);
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        throw(err);
      }

      GUM_CONSTRUCTOR(CredalNet);
    }

    template <typename GUM_SCALAR> CredalNet<GUM_SCALAR>::~CredalNet() {
      if (__current_bn != nullptr)
        delete __current_bn;

      if (__credalNet_current_cpt != nullptr)
        delete __credalNet_current_cpt;

      if (__current_nodeType != nullptr)
        delete __current_nodeType;

      GUM_DESTRUCTOR(CredalNet);
    }

    // from BNs with numerators & denominators or cpts & denominators to credal
    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::bnToCredal(const GUM_SCALAR beta, const bool oneNet,
                                           const bool keepZeroes) {
      double epsi_min = 1;
      double epsi_max = 0;
      double epsi_moy = 0;
      double epsi_den = 0;

      for (auto node : src_bn().nodes()) {
        const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(node));

        Potential<GUM_SCALAR> *const potential_min(
            const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_min.cpt(node)));
        Potential<GUM_SCALAR> *const potential_max(
            const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_max.cpt(node)));

        Size var_dSize = __src_bn.variable(node).domainSize();
        Size entry_size = potential->domainSize() / var_dSize;

        Instantiation ins(potential);
        Instantiation ins_min(potential_min);
        Instantiation ins_max(potential_max);

        ins.setFirst();
        ins_min.setFirst();
        ins_max.setFirst();

        std::vector<GUM_SCALAR> vertex(var_dSize);

        for (Size entry = 0; entry < entry_size; entry++) {
          GUM_SCALAR den;

          if (oneNet)
            den = 0;
          else
            den = potential_max->get(ins_max);

          unsigned int nbm = 0;

          for (Size modality = 0; modality < var_dSize; modality++) {
            vertex[modality] = potential->get(ins);

            if (oneNet) {
              den += vertex[modality];

              if (vertex[modality] < 1 && vertex[modality] > 0)
                GUM_ERROR(OperationNotAllowed, "bnToCredal : the BayesNet contains "
                                               "probabilities and not event counts "
                                               "although user precised oneNet = "
                                                   << oneNet);
            }

            if (vertex[modality] > 0)
              nbm++;

            ++ins;
          }

          /// check sum is 1 if not oneNet (we are not using counts)
          if (!oneNet) {
            GUM_SCALAR sum = 0;

            for (auto modality = vertex.cbegin(), theEnd = vertex.cend();
                 modality != theEnd; ++modality) {
              sum += *modality;
            }

            if (std::fabs(1. - sum) > __epsRedund) {
              GUM_ERROR(CPTNoSumTo1, __src_bn.variable(node).name()
                                         << "(" << __epsRedund << ")"
                                         << " " << entry << std::endl << vertex
                                         << std::endl << ins << std::endl);
            }
          }

          /// end check sum is 1

          GUM_SCALAR epsilon;

          if (beta == 0)
            epsilon = 0;
          else if (den == 0 || beta == 1)
            epsilon = 1;
          else
            epsilon = std::pow(beta, std::log(den + 1));

          epsi_moy += epsilon;
          epsi_den += 1;

          if (epsilon > epsi_max)
            epsi_max = epsilon;

          if (epsilon < epsi_min)
            epsi_min = epsilon;

          GUM_SCALAR min, max;

          for (Size modality = 0; modality < var_dSize; modality++) {
            if ((vertex[modality] > 0 && nbm > 1) || !keepZeroes) {
              min = (1. - epsilon) * vertex[modality];

              if (oneNet)
                min = min * 1.0 / den;

              max = min + epsilon;
            } else { // if ( ( vertex[modality] == 0 && keepZeroes ) || (
                     // vertex[modality] > 0 && nbm <= 1 ) || ( vertex[modality] == 0
                     // && nbm <= 1 ) ) {
              min = vertex[modality];

              if (oneNet)
                min = min * 1.0 / den;

              max = min;
            }

            potential_min->set(ins_min, min);
            potential_max->set(ins_max, max);

            ++ins_min;
            ++ins_max;
          } // end of : for each modality

        } // end of : for each entry

      } // end of : for each variable

      __epsilonMin = epsi_min;
      __epsilonMax = epsi_max;
      __epsilonMoy = (double)epsi_moy / (double)epsi_den;

      __intervalToCredal();
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::lagrangeNormalization() {
      for (auto node : __src_bn.nodes()) {
        const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(node));

        auto var_dSize = __src_bn.variable(node).domainSize();
        auto entry_size = potential->domainSize() / var_dSize;

        Instantiation ins(potential);

        ins.setFirst();

        std::vector<GUM_SCALAR> vertex(var_dSize);

        for (Size entry = 0; entry < entry_size; entry++) {

          GUM_SCALAR den = 0;
          bool zeroes = false;
          Instantiation ins_prev = ins;

          for (Size modality = 0; modality < var_dSize; modality++) {
            vertex[modality] = potential->get(ins);

            if (vertex[modality] < 1 && vertex[modality] > 0)
              GUM_ERROR(OperationNotAllowed, "lagrangeNormalization : the BayesNet "
                                             "contains probabilities and not event "
                                             "counts.");

            den += vertex[modality];

            if (!zeroes && vertex[modality] == 0) {
              zeroes = true;
            }

            ++ins;
          }

          if (zeroes) {
            ins = ins_prev;

            for (Size modality = 0; modality < var_dSize; modality++) {
              potential->set(ins, potential->get(ins) + 1);
              ++ins;
            }
          }

        } // end of : for each entry

      } // end of : for each variable
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::idmLearning(const unsigned int s,
                                            const bool keepZeroes) {
      for (auto node : __src_bn.nodes()) {
        const Potential<GUM_SCALAR> *const potential(&__src_bn.cpt(node));

        Potential<GUM_SCALAR> *const potential_min(
            const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_min.cpt(node)));
        Potential<GUM_SCALAR> *const potential_max(
            const_cast<Potential<GUM_SCALAR> *const>(&__src_bn_max.cpt(node)));

        Size var_dSize = __src_bn.variable(node).domainSize();
        Size entry_size = potential->domainSize() / var_dSize;

        Instantiation ins(potential);
        Instantiation ins_min(potential_min);
        Instantiation ins_max(potential_max);

        ins.setFirst();
        ins_min.setFirst();
        ins_max.setFirst();

        std::vector<GUM_SCALAR> vertex(var_dSize);

        for (Size entry = 0; entry < entry_size; entry++) {
          GUM_SCALAR den = 0;
          unsigned int nbm = 0;

          for (Size modality = 0; modality < var_dSize; modality++) {
            vertex[modality] = potential->get(ins);

            if (vertex[modality] < 1 && vertex[modality] > 0)
              GUM_ERROR(OperationNotAllowed, "idmLearning : the BayesNet contains "
                                             "probabilities and not event counts.");

            den += vertex[modality];

            if (vertex[modality] > 0)
              nbm++;

            ++ins;
          }

          if (nbm > 1 || !keepZeroes)
            den += s;

          GUM_SCALAR min, max;

          for (Size modality = 0; modality < var_dSize; modality++) {
            min = vertex[modality];
            max = min;

            if ((vertex[modality] > 0 && nbm > 1) || !keepZeroes) {
              max += s;
            }

            min = min * 1.0 / den;
            max = max * 1.0 / den;

            potential_min->set(ins_min, min);
            potential_max->set(ins_max, max);

            ++ins_min;
            ++ins_max;
          } // end of : for each modality

        } // end of : for each entry

      } // end of : for each variable

      __epsilonMin = s;
      __epsilonMax = s;
      __epsilonMoy = s;
      __intervalToCredal();
    }

    /* no need for lrs : (max ... min ... max) vertices from bnToCredal() */
    template <typename GUM_SCALAR> void CredalNet<GUM_SCALAR>::__intervalToCredal() {
      if (!__credalNet_src_cpt.empty())
        __credalNet_src_cpt.clear();

      __credalNet_src_cpt.resize(__src_bn.size());

      for (auto node : __src_bn.nodes()) {
        const Potential<GUM_SCALAR> *const potential_min(&__src_bn_min.cpt(node));
        const Potential<GUM_SCALAR> *const potential_max(&__src_bn_max.cpt(node));

        Size var_dSize = __src_bn.variable(node).domainSize();
        Size entry_size = potential_min->domainSize() / var_dSize;

        std::vector<std::vector<std::vector<GUM_SCALAR>>> var_cpt(entry_size);

        Instantiation ins_min(potential_min);
        Instantiation ins_max(potential_max);

        ins_min.setFirst();
        ins_max.setFirst();

        std::vector<GUM_SCALAR> lower(var_dSize);
        std::vector<GUM_SCALAR> upper(var_dSize);

        for (Size entry = 0; entry < entry_size; entry++) {
          for (Size modality = 0; modality < var_dSize;
               modality++, ++ins_min, ++ins_max) {
            lower[modality] = potential_min->get(ins_min);
            upper[modality] = potential_max->get(ins_max);
          }

          bool all_equals = true;
          std::vector<std::vector<GUM_SCALAR>> vertices;

          for (Size modality = 0; modality < var_dSize; modality++) {
            if (std::fabs(upper[modality] - lower[modality]) < 1e-6)
              continue;

            all_equals = false;
            std::vector<GUM_SCALAR> vertex(var_dSize);
            vertex[modality] = upper[modality];

            for (Size mod = 0; mod < var_dSize; mod++) {
              if (modality != mod)
                vertex[mod] = lower[mod];
            }

            double total = 0;

            auto vsize = vertex.size();

            for (Size i = 0; i < vsize; i++)
              total += vertex[i];

            if (std::fabs(total - 1.) > 1e-6)
              GUM_ERROR(CPTNoSumTo1, __src_bn.variable(node).name()
                                         << " " << entry << std::endl << vertex
                                         << std::endl);

            vertices.push_back(vertex);
          }

          if (all_equals) {
            std::vector<GUM_SCALAR> vertex(var_dSize);

            for (Size modality = 0; modality < var_dSize; modality++)
              vertex[modality] = lower[modality];

            double total = 0.;

            auto vsize = vertex.size();

            for (Size i = 0; i < vsize; i++)
              total += vertex[i];

            if (std::fabs(total - 1.) > 1e-6)
              GUM_ERROR(CPTNoSumTo1, __src_bn.variable(node).name()
                                         << " " << entry << std::endl << vertex
                                         << std::endl);

            vertices.push_back(vertex);
          }

          var_cpt[entry] = vertices;
        }

        __credalNet_src_cpt.insert(node, var_cpt);

      } // end of : for each variable (node)

      // get precise/credal/vacuous status of each variable
      __sort_varType();
      __separatelySpecified = true;
    }

    /* uses lrsWrapper */
    template <typename GUM_SCALAR> void CredalNet<GUM_SCALAR>::intervalToCredal() {
      if (!__credalNet_src_cpt.empty())
        __credalNet_src_cpt.clear();

      __credalNet_src_cpt.resize(__src_bn.size());

      LRSWrapper<GUM_SCALAR> lrsWrapper;

      for (auto node : __src_bn.nodes()) {
        const Potential<GUM_SCALAR> *const potential_min(&__src_bn_min.cpt(node));
        const Potential<GUM_SCALAR> *const potential_max(&__src_bn_max.cpt(node));

        Size var_dSize = __src_bn.variable(node).domainSize();
        Size entry_size = potential_min->domainSize() / var_dSize;

        std::vector<std::vector<std::vector<GUM_SCALAR>>> var_cpt(entry_size);

        Instantiation ins_min(potential_min);
        Instantiation ins_max(potential_max);

        ins_min.setFirst();
        ins_max.setFirst();

        lrsWrapper.setUpH(var_dSize);

        for (Size entry = 0; entry < entry_size; entry++) {
          for (Size modality = 0; modality < var_dSize; modality++) {
            lrsWrapper.fillH(potential_min->get(ins_min),
                             potential_max->get(ins_max), modality);
            ++ins_min;
            ++ins_max;
          }

          lrsWrapper.H2V();
          var_cpt[entry] = lrsWrapper.getOutput();
          lrsWrapper.nextHInput();
        }

        __credalNet_src_cpt.insert(node, var_cpt);

      } // end of : for each variable (node)

      // get precise/credal/vacuous status of each variable
      __sort_varType();
      __separatelySpecified = true;
    }

    /* call lrs */
    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::intervalToCredalWithFiles() {
      if (!__credalNet_src_cpt.empty())
        __credalNet_src_cpt.clear();

      __credalNet_src_cpt.resize(__src_bn.size());

      for (auto node : __src_bn.nodes()) {
        const Potential<GUM_SCALAR> *const potential_min(&__src_bn_min.cpt(node));
        const Potential<GUM_SCALAR> *const potential_max(&__src_bn_max.cpt(node));

        auto var_dSize = __src_bn.variable(node).domainSize();
        auto entry_size = potential_min->domainSize() / var_dSize;

        std::vector<std::vector<std::vector<GUM_SCALAR>>> var_cpt(entry_size);

        Instantiation ins_min(potential_min);
        Instantiation ins_max(potential_max);

        ins_min.setFirst();
        ins_max.setFirst();

        // use iterator
        for (Size entry = 0; entry < entry_size; entry++) {
          std::vector<std::vector<GUM_SCALAR>> vertices;
          std::vector<GUM_SCALAR> vertex(var_dSize); // if not interval

          std::vector<std::vector<GUM_SCALAR>> inequalities(
              var_dSize * 2, std::vector<GUM_SCALAR>(var_dSize + 1, 0));

          std::vector<GUM_SCALAR> sum_ineq1(var_dSize + 1, -1);
          std::vector<GUM_SCALAR> sum_ineq2(var_dSize + 1, 1);
          sum_ineq1[0] = 1;
          sum_ineq2[0] = -1;

          bool isInterval = false;

          for (Size modality = 0; modality < var_dSize; modality++) {
            inequalities[modality * 2][0] = -potential_min->get(ins_min);
            inequalities[modality * 2 + 1][0] = potential_max->get(ins_max);
            inequalities[modality * 2][modality + 1] = 1;
            inequalities[modality * 2 + 1][modality + 1] = -1;

            vertex[modality] = inequalities[modality * 2 + 1][0];

            if (!isInterval && (-inequalities[modality * 2][0] !=
                                inequalities[modality * 2 + 1][0]))
              isInterval = true;

            ++ins_min;
            ++ins_max;
          }

          inequalities.push_back(sum_ineq1);
          inequalities.push_back(sum_ineq2);

          if (!isInterval) {
            vertices.push_back(vertex);
          } else {
            try {
              __H2Vlrs(inequalities, vertices);
              //__H2Vcdd ( inequalities, vertices );
            } catch (const std::exception &err) {
              std::cout << err.what() << std::endl;
              throw;
            }

          } // end of : is interval

          if (entry == 0 && vertices.size() >= 2) {
            auto tmp = vertices[0];
            vertices[0] = vertices[1];
            vertices[1] = tmp;
          }

          var_cpt[entry] = vertices;

        } // end of : for each entry

        __credalNet_src_cpt.insert(node, var_cpt);
        // std::cout << __src_bn.variable(node_idIt).name() << std::endl;
        // std::cout << var_cpt << std::endl;

      } // end of : for each variable (node)

      // get precise/credal/vacuous status of each variable
      __sort_varType();
      __separatelySpecified = true;
    }

    /**
     * to call after bnToCredal( GUM_SCALAR beta )
     * save a BN with lower probabilities and a BN with upper ones
     */
    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::saveBNsMinMax(const std::string &min_path,
                                              const std::string &max_path) const {
      BIFWriter<GUM_SCALAR> writer;

      std::string minfilename = min_path; //"min.bif";
      std::string maxfilename = max_path; //"max.bif";
      std::ofstream min_file(minfilename.c_str(), std::ios::out | std::ios::trunc);
      std::ofstream max_file(maxfilename.c_str(), std::ios::out | std::ios::trunc);

      if (!min_file.good())
        GUM_ERROR(IOError, "bnToCredal() : could not open stream : min_file : "
                               << minfilename);

      if (!max_file.good()) {
        min_file.close();
        GUM_ERROR(IOError, "bnToCredal() : could not open stream : min_file : "
                               << maxfilename);
      }

      try {
        writer.write(min_file, __src_bn_min);
        writer.write(max_file, __src_bn_max);
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        min_file.close();
        max_file.close();
        throw(err);
      }

      min_file.close();
      max_file.close();
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::approximatedBinarization() {
      // don't forget to delete the old one (__current), if necessary at the end
      BayesNet<GUM_SCALAR> *__bin_bn = new BayesNet<GUM_SCALAR>();

      //__bnCopy ( *__bin_bn );
      // delete old one too
      typename Property<std::vector<std::vector<std::vector<GUM_SCALAR>>>>::onNodes *
          __credalNet_bin_cpt = new typename Property<
              std::vector<std::vector<std::vector<GUM_SCALAR>>>>::onNodes();

      // delete old one too
      NodeProperty<NodeType> *__bin_nodeType = new NodeProperty<NodeType>();

      const BayesNet<GUM_SCALAR> *__current_bn;
      // const NodeProperty< nodeType > *__current_nodeType;
      const typename Property<std::vector<
          std::vector<std::vector<GUM_SCALAR>>>>::onNodes *__credalNet_current_cpt;

      if (this->__current_bn == nullptr)
        __current_bn = &this->__src_bn;
      else
        __current_bn = this->__current_bn;

      if (this->__credalNet_current_cpt == nullptr)
        __credalNet_current_cpt = &this->__credalNet_src_cpt;
      else
        __credalNet_current_cpt = this->__credalNet_current_cpt;

      /*if ( this->__current_nodeType == nullptr )
        __current_nodeType = & this->__nodeType;
      else
        __current_nodeType = this->__current_nodeType;*/

      if (!__var_bits.empty())
        __var_bits.clear();

      __bin_bn->beginTopologyTransformation();

      for (auto node : __current_bn->nodes()) {
        Size nb_bits, new_card;
        auto var_dSize = __current_bn->variable(node).domainSize();

        if (var_dSize != 2) {
          superiorPow(var_dSize, nb_bits, new_card);

          std::string bit_name;
          std::vector<NodeId> bits(nb_bits);

          for (Size bit = 0; bit < nb_bits; bit++) {
            bit_name = __current_bn->variable(node).name() + " - bit - ";
            std::stringstream ss;
            ss << bit;
            bit_name += ss.str();

            LabelizedVariable var_bit(bit_name, "node " + bit_name, 2);
            NodeId iD = __bin_bn->add(var_bit);

            bits[bit] = iD;
          } // end of : for each bit

          __var_bits.insert(node, bits);

        } // end of : if variable is not binary
        else {
          std::string bit_name = __current_bn->variable(node).name();
          LabelizedVariable var_bit(bit_name, "node " + bit_name, 2);
          NodeId iD = __bin_bn->add(var_bit);

          __var_bits.insert(node, std::vector<NodeId>(1, iD));
        }

      } // end of : for each original variable

      for (auto node : __current_bn->nodes()) {
        NodeSet parents = __current_bn->dag().parents(node);

        if (!parents.empty()) {
          for (auto par : __current_bn->dag().parents(node)) {
            for (Size parent_bit = 0, spbits = __var_bits[par].size();
                 parent_bit < spbits; parent_bit++)
              for (Size var_bit = 0, mbits = __var_bits[node].size();
                   var_bit < mbits; var_bit++)
                __bin_bn->addArc(__var_bits[par][parent_bit],
                                 __var_bits[node][var_bit]);
          }
        }

        // arcs with one's bits
        auto bitsize = __var_bits[node].size();

        for (Size bit_c = 1; bit_c < bitsize; bit_c++)
          for (Size bit_p = 0; bit_p < bit_c; bit_p++)
            __bin_bn->addArc(__var_bits[node][bit_p], __var_bits[node][bit_c]);

      } // end of : for each original variable

      __bin_bn->endTopologyTransformation();

      // binarization of cpts

      auto varsize = __current_bn->size();

      for (Size var = 0; var < varsize; var++) {
        auto bitsize = __var_bits[var].size();

        for (Size i = 0; i < bitsize; i++) {
          Potential<GUM_SCALAR> const *potential(&__bin_bn->cpt(__var_bits[var][i]));
          Instantiation ins(potential);
          ins.setFirst();

          auto entry_size = potential->domainSize() / 2;
          std::vector<std::vector<std::vector<GUM_SCALAR>>> var_cpt(entry_size);

          unsigned int old_conf = 0;

          for (Size conf = 0; conf < entry_size; conf++) {
            std::vector<std::vector<GUM_SCALAR>> pvar_cpt;
            auto verticessize = (*__credalNet_current_cpt)[var][old_conf].size();

            for (Size old_distri = 0; old_distri < verticessize; old_distri++) {
              const std::vector<GUM_SCALAR> &vertex =
                  (*__credalNet_current_cpt)[var][old_conf][old_distri];
              auto vertexsize = vertex.size();

              std::vector<unsigned int> incc(vertexsize, 0);

              for (Size preced = 0; preced < i; preced++) {
                auto bit_pos = ins.pos(__bin_bn->variable(__var_bits[var][preced]));
                auto val = ins.val(bit_pos);

                Size pas = preced;
                int2Pow(pas);

                Size elem;

                if (val == 0)
                  elem = 0;
                else
                  elem = pas;

                while (elem < vertexsize) {
                  incc[elem]++;
                  elem++;

                  if (elem % pas == 0)
                    elem += pas;
                }
              }

              Size pas = i;
              int2Pow(pas);

              std::vector<GUM_SCALAR> distri(2, 0);
              int pos = 1;

              for (Size elem = 0; elem < vertexsize; elem++) {
                if (elem % pas == 0)
                  pos = -pos;

                if (incc[elem] == i)
                  (pos < 0) ? (distri[0] += vertex[elem])
                            : (distri[1] += vertex[elem]);
              }

              if (i > 0) {
                GUM_SCALAR den = distri[0] + distri[1];

                if (den == 0) {
                  distri[0] = 0;
                  distri[1] = 0;
                } else {
                  distri[0] /= den;
                  distri[1] /= den;
                }
              }

              pvar_cpt.push_back(distri);

            } // end of old distris

            // get min/max approx, 2 vertices
            std::vector<std::vector<GUM_SCALAR>> vertices(
                2, std::vector<GUM_SCALAR>(2, 1));
            vertices[1][1] = 0;

            auto new_verticessize = pvar_cpt.size();

            for (Size v = 0; v < new_verticessize; v++) {
              if (pvar_cpt[v][1] < vertices[0][1])
                vertices[0][1] = pvar_cpt[v][1];

              if (pvar_cpt[v][1] > vertices[1][1])
                vertices[1][1] = pvar_cpt[v][1];
            }

            vertices[0][0] = 1 - vertices[0][1];
            vertices[1][0] = 1 - vertices[1][1];

            pvar_cpt = vertices;

            var_cpt[conf] = pvar_cpt;

            ++ins;
            ++ins;

            old_conf++;

            if (old_conf == (*__credalNet_current_cpt)[var].size())
              old_conf = 0;

          } // end of new parent conf

          __credalNet_bin_cpt->insert(__var_bits[var][i], var_cpt);

        } // end of bit i

      } // end of old variable

      __bin_bn->beginTopologyTransformation();

      /* indicatrices variables */
      auto old_varsize = __var_bits.size();

      for (Size i = 0; i < old_varsize; i++) {
        auto bitsize = __var_bits[i].size();

        // binary variable
        if (bitsize == 1)
          continue;

        auto old_card = __src_bn.variable(i).domainSize();

        for (Size mod = 0; mod < old_card; mod++) {
          std::string s;
          s = "I-";
          std::stringstream ss;
          ss << __src_bn.variable(i).name();
          ss << "-";
          ss << mod;
          s += ss.str();

          LabelizedVariable var(s, "node " + s, 2);
          const NodeId indic = __bin_bn->add(var);

          // arcs from one's bits
          for (Size bit = 0; bit < bitsize; bit++)
            __bin_bn->addArc(__var_bits[i][bit], indic);

          // cpt
          Size num = bitsize;
          int2Pow(num);

          std::vector<std::vector<std::vector<GUM_SCALAR>>> icpt(num);

          for (Size entry = 0; entry < num; entry++) {
            std::vector<std::vector<GUM_SCALAR>> vertices(
                1, std::vector<GUM_SCALAR>(2, 0));

            if (i == entry)
              vertices[0][1] = 1;
            else
              vertices[0][0] = 1;

            icpt[entry] = vertices;
          }

          __credalNet_bin_cpt->insert(indic, icpt);

          __bin_nodeType->insert(indic, NodeType::Indic);
        } // end of each modality, i.e. as many indicatrice
      }

      __bin_bn->endTopologyTransformation();

      if (this->__current_bn != nullptr)
        delete this->__current_bn;

      this->__current_bn = __bin_bn;

      if (this->__credalNet_current_cpt != nullptr)
        delete this->__credalNet_current_cpt;

      this->__credalNet_current_cpt = __credalNet_bin_cpt;

      if (this->__current_nodeType != nullptr)
        delete this->__current_nodeType;

      this->__current_nodeType = __bin_nodeType;

      __sort_varType(); // will fill __bin_nodeType except for NodeType::Indic
                        // variables
    }

    template <typename GUM_SCALAR>
    const NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>> &
    CredalNet<GUM_SCALAR>::credalNet_currentCpt() const {
      if (__credalNet_current_cpt != nullptr)
        return *__credalNet_current_cpt;

      return __credalNet_src_cpt;
    }

    template <typename GUM_SCALAR>
    const NodeProperty<std::vector<std::vector<std::vector<GUM_SCALAR>>>> &
    CredalNet<GUM_SCALAR>::credalNet_srcCpt() const {
      return __credalNet_src_cpt;
    }

    template <typename GUM_SCALAR>
    typename CredalNet<GUM_SCALAR>::NodeType
    CredalNet<GUM_SCALAR>::currentNodeType(const NodeId &id) const {
      if (__current_nodeType != nullptr)
        return (*(__current_nodeType))[id];

      return __original_nodeType[id];
    }

    template <typename GUM_SCALAR>
    typename CredalNet<GUM_SCALAR>::NodeType
    CredalNet<GUM_SCALAR>::nodeType(const NodeId &id) const {
      return __original_nodeType[id];
    }

    template <typename GUM_SCALAR>
    const bool CredalNet<GUM_SCALAR>::isSeparatelySpecified() const {
      return __separatelySpecified;
    }

    template <typename GUM_SCALAR>
    const bool CredalNet<GUM_SCALAR>::hasComputedCPTMinMax() const {
      return __hasComputedCPTMinMax;
    }

    // only if CN is binary !!
    template <typename GUM_SCALAR> void CredalNet<GUM_SCALAR>::computeCPTMinMax() {
      __binCptMin.resize(current_bn().size());
      __binCptMax.resize(current_bn().size());

      for (auto node : current_bn().nodes()) {
        auto pConf = credalNet_currentCpt()[node].size();
        std::vector<GUM_SCALAR> min(pConf);
        std::vector<GUM_SCALAR> max(pConf);

        for (Size pconf = 0; pconf < pConf; pconf++) {
          GUM_SCALAR v1, v2;
          v1 = credalNet_currentCpt()[node][pconf][0][1];

          if (credalNet_currentCpt()[node][pconf].size() > 1)
            v2 = credalNet_currentCpt()[node][pconf][1][1];
          else
            v2 = v1;

          GUM_SCALAR delta = v1 - v2;
          min[pconf] = (delta >= 0) ? v2 : v1;
          max[pconf] = (delta >= 0) ? v1 : v2;
        }

        __binCptMin[node] = min;
        __binCptMax[node] = max;
      }

      __hasComputedCPTMinMax = true;
    }

    template <typename GUM_SCALAR>
    const std::vector<std::vector<GUM_SCALAR>> &
    CredalNet<GUM_SCALAR>::get_CPT_min() const {
      return __binCptMin;
    }

    template <typename GUM_SCALAR>
    const std::vector<std::vector<GUM_SCALAR>> &
    CredalNet<GUM_SCALAR>::get_CPT_max() const {
      return __binCptMax;
    }

    template <typename GUM_SCALAR>
    const double &CredalNet<GUM_SCALAR>::epsilonMin() const {
      return __epsilonMin;
    }

    template <typename GUM_SCALAR>
    const double &CredalNet<GUM_SCALAR>::epsilonMax() const {
      return __epsilonMax;
    }

    template <typename GUM_SCALAR>
    const double &CredalNet<GUM_SCALAR>::epsilonMean() const {
      return __epsilonMoy;
    }

    template <typename GUM_SCALAR>
    std::string CredalNet<GUM_SCALAR>::toString() const {
      std::stringstream output;
      const BayesNet<GUM_SCALAR> *__current_bn;
      const typename Property<std::vector<
          std::vector<std::vector<GUM_SCALAR>>>>::onNodes *__credalNet_current_cpt;

      if (this->__current_bn == nullptr)
        __current_bn = &this->__src_bn;
      else
        __current_bn = this->__current_bn;

      if (this->__credalNet_current_cpt == nullptr)
        __credalNet_current_cpt = &this->__credalNet_src_cpt;
      else
        __credalNet_current_cpt = this->__credalNet_current_cpt;

      for (auto node : __current_bn->nodes()) {
        const Potential<GUM_SCALAR> *potential(&__current_bn->cpt(node));
        auto pconfs =
            potential->domainSize() / __current_bn->variable(node).domainSize();

        output << "\n" << __current_bn->variable(node) << "\n";

        Instantiation ins(potential);
        ins.forgetMaster();
        ins.erase(__current_bn->variable(node));
        ins.setFirst();

        for (Size pconf = 0; pconf < pconfs; pconf++) {
          output << ins << " : ";
          output << (*__credalNet_current_cpt)[node][pconf] << "\n";

          if (pconf < pconfs - 1)
            ++ins;
        }
      }

      output << "\n";

      return output.str();
    }

    template <typename GUM_SCALAR>
    const BayesNet<GUM_SCALAR> &CredalNet<GUM_SCALAR>::current_bn() const {
      if (__current_bn != nullptr)
        return *__current_bn;

      return __src_bn;
    }

    template <typename GUM_SCALAR>
    const BayesNet<GUM_SCALAR> &CredalNet<GUM_SCALAR>::src_bn() const {
      return __src_bn;
    }

    /////////// protected stuff //////////

    /////////// private stuff ////////////

    template <typename GUM_SCALAR> void CredalNet<GUM_SCALAR>::__initParams() {
      __epsilonMin = 0;
      __epsilonMax = 0;
      __epsilonMoy = 0;

      __epsRedund = 1e-6;

      // farey algorithm
      __epsF = 1e-6;
      __denMax = 1e6; // beware LRSWrapper

      // continued fractions, beware LRSWrapper
      // decimal paces (__epsC * __precisionC == 1)
      __precisionC = 1e6;
      __deltaC = 5;

      // old custom algorithm
      __precision = 1e6; // beware LRSWrapper

      __current_bn = nullptr;
      __credalNet_current_cpt = nullptr;
      __current_nodeType = nullptr;

      __hasComputedCPTMinMax = false;
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::__initCNNets(const std::string &src_min_num,
                                             const std::string &src_max_den) {
      BIFReader<GUM_SCALAR> reader(&__src_bn, src_min_num);
      std::string other;

      if (src_max_den.compare("") != 0)
        other = src_max_den;
      else
        other = src_min_num;

      BIFReader<GUM_SCALAR> reader_min(&__src_bn_min, src_min_num);
      BIFReader<GUM_SCALAR> reader_max(&__src_bn_max, other);

      try {
        reader.proceed();
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        throw(err);
      }

      try {
        reader_min.proceed();
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        throw(err);
      }

      try {
        reader_max.proceed();
      } catch (Exception &err) {
        GUM_SHOWERROR(err);
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    void
    CredalNet<GUM_SCALAR>::__initCNNets(const BayesNet<GUM_SCALAR> &src_min_num,
                                        const BayesNet<GUM_SCALAR> &src_max_den) {
      __src_bn = src_min_num;
      __src_bn_min = src_min_num;

      if (src_max_den.size() > 0)
        __src_bn_max = src_max_den;
      else
        __src_bn_max = src_min_num;
    }

    template <typename GUM_SCALAR>
    int CredalNet<GUM_SCALAR>::__find_dNode_card(
        const std::vector<std::vector<std::vector<GUM_SCALAR>>> &var_cpt) const {
      Size vertices_size = 0;

      for (auto entry = var_cpt.cbegin(), theEnd = var_cpt.cend(); entry != theEnd;
           ++entry) {
        if (entry->size() > vertices_size)
          vertices_size = entry->size();
      }

      return vertices_size;
    }

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::__bnCopy(BayesNet<GUM_SCALAR> &dest) {
      const BayesNet<GUM_SCALAR> *__current_bn;

      if (this->__current_bn == nullptr)
        __current_bn = &this->__src_bn;
      else
        __current_bn = this->__current_bn;

      for (auto node : __current_bn->nodes())
        dest.add(__current_bn->variable(node));

      dest.beginTopologyTransformation();

      for (auto node : __current_bn->nodes()) {
        for (auto parent_idIt = __current_bn->cpt(node).begin(),
                  theEnd2 = __current_bn->cpt(node).end();
             parent_idIt != theEnd2; ++parent_idIt) {
          if (__current_bn->nodeId(**parent_idIt) != node)
            dest.addArc(__current_bn->nodeId(**parent_idIt), node);
        } // end of : for each parent in order of appearence
      }   // end of : for each variable

      dest.endTopologyTransformation();
    }

    /*
      // cdd can use real values, not just rationals / integers
      template< typename GUM_SCALAR >
      void CredalNet< GUM_SCALAR >::__H2Vcdd ( const std::vector< std::vector<
      GUM_SCALAR > > & h_rep, std::vector< std::vector< GUM_SCALAR > > & v_rep )
      const {
        dd_set_global_constants();

        dd_MatrixPtr M, G;
        dd_PolyhedraPtr poly;
        dd_ErrorType err;

        unsigned int rows = h_rep.size();
        unsigned int cols = 0;
        if( h_rep.size() > 0 )
          cols = h_rep[0].size();

        M = dd_CreateMatrix( rows, cols);

        for ( unsigned int row = 0; row < rows; row++ )
          for ( unsigned int col = 0; col < cols; col++ )
            dd_set_d( M->matrix[row][col], h_rep[row][col] );

        M->representation = dd_Inequality;

        poly = dd_DDMatrix2Poly(M, &err);
        G = dd_CopyGenerators(poly);

        rows = G->rowsize;
        cols = G->colsize;

        v_rep.clear();
        for ( unsigned int row = 0; row < rows; row++ ) {
          std::vector< GUM_SCALAR > aRow(cols - 1);

          if ( *G->matrix[row][0] != 1 )
            GUM_ERROR(OperationNotAllowed, "__H2Vcdd : not reading a vertex");

          for ( unsigned int col = 0; col < cols - 1; col++ )
            aRow[col] = *G->matrix[row][ col + 1 ];

          v_rep.push_back(aRow);
        }

        dd_FreeMatrix(M);
        dd_FreeMatrix(G);
        dd_FreePolyhedra(poly);

        dd_free_global_constants();
      }
    */

    template <typename GUM_SCALAR>
    void CredalNet<GUM_SCALAR>::__H2Vlrs(
        const std::vector<std::vector<GUM_SCALAR>> &h_rep,
        std::vector<std::vector<GUM_SCALAR>> &v_rep) const {
      // write H rep file
      long int num, den;

      std::string sinefile = getUniqueFileName(); // generate unique file name, we
                                                  // need to add .ine or .ext for lrs
                                                  // to know which input it is (Hrep
                                                  // to Vrep or Vrep to Hrep)
      sinefile += ".ine";

      std::ofstream h_file(sinefile.c_str(), std::ios::out | std::ios::trunc);

      if (!h_file.good())
        GUM_ERROR(IOError,
                  "__H2Vlrs : could not open lrs input file : " << sinefile);

      h_file << "H - representation\n";
      h_file << "begin\n";
      h_file << h_rep.size() << ' ' << h_rep[0].size() << " rational\n";

      for (auto it = h_rep.cbegin(), theEnd = h_rep.cend(); it != theEnd; ++it) {
        for (auto it2 = it->cbegin(), theEnd2 = it->cend(); it2 != theEnd2; ++it2) {
          // get integer fraction from decimal value
          // smallest numerator & denominator is farley, also
          // best precision
          Rational<GUM_SCALAR>::farey(num, den, ((*it2 > 0) ? *it2 : -*it2),
                                      __denMax, __epsF);

          h_file << ((*it2 > 0) ? num : -num) << '/' << den << ' ';
        }

        h_file << '\n';
      }

      h_file << "end\n";
      h_file.close();

      // call lrs
      // lrs arguments
      char *args[3];

      std::string soft_name = "lrs";
      std::string extfile(sinefile);
      extfile += ".ext";

      args[0] = new char[soft_name.size()];
      args[1] = new char[sinefile.size()];
      args[2] = new char[extfile.size()];

      strcpy(args[0], soft_name.c_str());
      strcpy(args[1], sinefile.c_str());
      strcpy(args[2], extfile.c_str());

      // cout to null not working in agrum, why ?
      //
      // doesn't matter, use temporary file (not working with TestSuite either)
      //
      /*   char * lrs_outputs = tmpnam(nullptr);
         std::string lrslog(lrs_outputs);
         lrslog += ".lrslog";

         std::ofstream l_file ( lrslog.c_str(), std::ios::out | std::ios::trunc );

         if ( ! l_file.good() )
           GUM_ERROR ( IOError, "__H2V : could not open lrs log file : " << lrslog );

         std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
         std::cout.rdbuf(l_file.rdbuf()); //redirect cout to tmp file

         lrs_main ( 3, args );

         std::cout.rdbuf(coutbuf); //restore standard output again

         // delete file
         if( std::remove(lrslog.c_str()) != 0)
           GUM_ERROR(IOError, "error removing : " + lrslog);
      */
      /////
      /////
      /////

      // standard cout to null (avoid lrs flooding)
      int old_cout, new_cout;
      fflush(stdout);
      old_cout = dup(1);

      new_cout = open("/dev/null", O_WRONLY);
      dup2(new_cout, 1);
      close(new_cout);

      lrs_main(3, args);

      // restore standard cout
      fflush(stdout);
      dup2(old_cout, 1);
      close(old_cout);

      delete[] args[2];
      delete[] args[1];
      delete[] args[0];

      // read V rep file
      std::ifstream v_file(extfile.c_str() /*extfilename.c_str()*/, std::ios::in);

      if (!v_file.good())
        GUM_ERROR(IOError, "__H2Vlrs : could not open lrs ouput file : ");

      std::string line, tmp;
      char *cstr, *p;
      GUM_SCALAR probability;

      std::string::size_type pos, end_pos;
      bool keep_going = true;
      // int vertices;

      std::vector<GUM_SCALAR> vertex;

      v_file.ignore(256, 'l');

      while (v_file.good() && keep_going) {
        getline(v_file, line);

        if (line.size() == 0)
          continue;
        else if (line.compare("end") == 0) {
          keep_going = false;
          // this is to get vertices number :
          /*getline ( v_file, line );
          pos = line.find ( "vertices = " );
          end_pos = line.find ( "rays", pos + 9 );
          vertices = atoi ( line.substr ( pos + 9, end_pos - pos - 9 ).c_str() );*/
          break;
        } else if (line[1] != '1') {
          GUM_ERROR(IOError, "__H2Vlrs : reading something other than a vertex from "
                             "lrs output file : ");
        }

        line = line.substr(2);
        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p = strtok(cstr, " ");

        while (p != nullptr) {
          tmp = p;

          if (tmp.compare("1") == 0 || tmp.compare("0") == 0)
            probability = atof(tmp.c_str());
          else {
            pos = tmp.find("/");
            probability = atof(tmp.substr(0, pos).c_str()) /
                          atof(tmp.substr(pos + 1, tmp.size()).c_str());
          }

          vertex.push_back(probability);
          p = strtok(nullptr, " ");
        } // end of : for all tokens

        delete[] p;
        delete[] cstr;

        bool is_redund = false;

#pragma omp parallel
        {
          int this_thread = getThreadNumber();
          int num_threads = getNumberOfRunningThreads();

          auto begin_pos = (this_thread + 0) * v_rep.size() / num_threads;
          auto end_pos = (this_thread + 1) * v_rep.size() / num_threads;

          for (auto p = begin_pos; p < end_pos; p++) {
#pragma omp flush(is_redund)

            if (is_redund)
              break;

            bool thread_redund = true;

            auto vsize = vertex.size();

            for (Size modality = 0; modality < vsize; modality++) {
              if (std::fabs(vertex[modality] - v_rep[p][modality]) > __epsRedund) {
                thread_redund = false;
                break;
              }
            }

            if (thread_redund) {
              is_redund = true;
#pragma omp flush(is_redund)
            }
          } // end of : each thread for
        }   // end of : parallel

        if (!is_redund)
          v_rep.push_back(vertex);

        vertex.clear();

      } // end of : file

      v_file.close();

      if (std::remove(sinefile.c_str()) != 0)
        GUM_ERROR(IOError, "error removing : " + sinefile);

      if (std::remove(extfile.c_str()) != 0)
        GUM_ERROR(IOError, "error removing : " + extfile);
    }

    template <typename GUM_SCALAR> void CredalNet<GUM_SCALAR>::__sort_varType() {
      NodeProperty<NodeType> *__current_nodeType;
      const typename Property<std::vector<
          std::vector<std::vector<GUM_SCALAR>>>>::onNodes *__credalNet_current_cpt;

      const BayesNet<GUM_SCALAR> *__current_bn;

      if (this->__current_bn == nullptr)
        __current_bn = &__src_bn;
      else
        __current_bn = this->__current_bn;

      if (this->__credalNet_current_cpt == nullptr)
        __credalNet_current_cpt = &__credalNet_src_cpt;
      else
        __credalNet_current_cpt = this->__credalNet_current_cpt;

      if (this->__current_nodeType == nullptr)
        __current_nodeType = &__original_nodeType;
      else
        __current_nodeType = this->__current_nodeType;

      /*if ( ! __current_nodeType->empty() )
        __current_nodeType->clear();*/

      for (auto node : __current_bn->nodes()) {
        // indicatrices are already present
        if (__current_nodeType->exists(node))
          continue;

        bool precise = true, vacuous = true;

        for (auto entry = (*__credalNet_current_cpt)[node].cbegin(),
                  theEnd2 = (*__credalNet_current_cpt)[node].cend();
             entry != theEnd2; ++entry) {
          auto vertices = entry->size();
          auto var_dSize = (*entry)[0].size();

          if (precise && vertices > 1)
            precise = false;

          if (vacuous && vertices == var_dSize) {
            std::vector<bool> elem(var_dSize, false);

            for (auto vertex = entry->cbegin(), vEnd = entry->cend(); vertex != vEnd;
                 ++vertex) {
              for (auto probability = vertex->cbegin(), pEnd = vertex->cend();
                   probability != pEnd; ++probability) {
                if (*probability == 1) {
                  elem[probability - vertex->begin()] = true;
                  break;
                }
              } // end of : for each modality

              break; // not vacuous
            }        // end of : for each vertex

            for (auto /*std::vector< bool >::const_iterator*/ probability =
                     elem.cbegin();
                 probability != elem.cend(); ++probability)
              if (*probability == false)
                vacuous = false;

          } // end of : if vertices == dSize
          else
            vacuous = false;

          if (vacuous == false && precise == false) {
            __current_nodeType->insert(node, NodeType::Credal);
            break;
          }

        } // end of : for each parents entry

        if (vacuous)
          __current_nodeType->insert(node, NodeType::Vacuous);
        else if (precise)
          __current_nodeType->insert(node, NodeType::Precise);

      } // end of : for each variable
    }

  } // namespace cn
}
