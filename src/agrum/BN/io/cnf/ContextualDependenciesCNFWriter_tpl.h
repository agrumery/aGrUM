/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/BN/io/cnf/ContextualDependenciesCNFWriter.h>

namespace gum {
  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE
     ContextualDependenciesCNFWriter< GUM_SCALAR,
                                      IApproximationPolicy >::ContextualDependenciesCNFWriter() {
    GUM_CONSTRUCTOR(ContextualDependenciesCNFWriter)
  }

  // Default destructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE
     ContextualDependenciesCNFWriter< GUM_SCALAR,
                                      IApproximationPolicy >::~ContextualDependenciesCNFWriter() {
    GUM_DESTRUCTOR(ContextualDependenciesCNFWriter)
  }

  //
  // Writes a Bayesian network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE void ContextualDependenciesCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
     std::ostream&                  output,
     const IBayesNet< GUM_SCALAR >& bn) {
    Instantiation Order;

    for (auto node: bn.topologicalOrder())
      Order.add(bn.variable(node));

    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.")

    std::stringstream strfile;
    std::stringstream strfile2;

    Idx                                num    = 0;
    Idx                                numvar = 0;
    Idx                                clause = 0;
    std::stringstream                  clausstr;
    std::stringstream                  clausstr2;
    gum::HashTable< std::string, Idx > vartable;
    gum::HashTable< std::string, Idx > protable;
    gum::HashTable<
       const gum::DiscreteVariable*,
       gum::HashTable< std::string, gum::Sequence< gum::Sequence< gum::Instantiation* >* >* >* >
       cptparamval;

    for (auto node: bn.nodes()) {
      std::stringstream       str0;
      const DiscreteVariable* var = &bn.variable(node);

      for (Idx i = 0; i < var->domainSize(); i++) {
        std::stringstream stri;
        stri << var->name() << "_" << var->label(i);
        vartable.insert(stri.str(), ++num);
        strfile << num << "::" << stri.str() << "\n";
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n";
      clause++;
      numvar++;
      clausstr2 << str0.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);
      inst.forgetMaster();
      inst.reorder(Order);
      cptparamval.insert(
         var,
         new gum::HashTable< std::string,
                             gum::Sequence< gum::Sequence< gum::Instantiation* >* >* >());

      for (inst.setFirst(); !inst.end(); ++inst) {
        if (this->fromExact(cpt[inst]) != 1) {
          std::stringstream strk;
          strk << this->fromExact(cpt[inst]);
          std::string valp = strk.str();

          if (!(cptparamval[var])->exists(valp)) {
            (cptparamval[var])
               ->insert(valp, new gum::Sequence< gum::Sequence< gum::Instantiation* >* >());

            (*(cptparamval[var]))[valp]->insert(new gum::Sequence< gum::Instantiation* >);

            if (this->fromExact(cpt[inst])) {
              std::stringstream strinst;
              strinst << var->name();
              strinst << "_val=" << this->fromExact(cpt[inst]);

              if (!protable.exists(strinst.str())) {
                protable.insert(strinst.str(), ++num);
                strfile << num << "::" << strinst.str() << "\n";
              }
            }
          }

          (*(cptparamval[var]))[valp]->front()->insert(new gum::Instantiation(inst));
        }
      }
    }

    std::stringstream str2;

    while (!cptparamval.empty()) {
      auto itvar = cptparamval.begin();

      while (!(itvar.val())->empty()) {
        auto itpvall = (itvar.val())->begin();   // needed here

        for (auto pv: *itpvall.val()) {
          gum::Idx linecount = 0;
          gum::HashTable< std::string,
                          gum::HashTable< const gum::DiscreteVariable*,
                                          std::pair< gum::Set< Idx >*,
                                                     gum::Set< Idx >* >* >* >
             orderStruct;   // set sizeof Hashtable

          for (const auto* seqv: *pv) {
            if (seqv->nbrDim() > 1) {
              for (Idx iInst = 0; iInst < seqv->nbrDim(); iInst++) {
                gum::Instantiation instpro(*seqv, false);
                instpro.reorder(Order);
                const gum::DiscreteVariable* var = &(seqv->variable(iInst));
                instpro.erase(*var);   // reorder instance to optimize make sure
                                       // key unicity.

                if (!orderStruct.exists(instpro.toString())) {
                  orderStruct.insert(
                     instpro.toString(),
                     new gum::HashTable< const gum::DiscreteVariable*,
                                         std::pair< gum::Set< Idx >*, gum::Set< Idx >* >* >());
                }

                if (!orderStruct[instpro.toString()]->exists(var)) {
                  orderStruct[instpro.toString()]->insert(
                     var,
                     new std::pair< gum::Set< Idx >*, gum::Set< Idx >* >(
                        new gum::Set< Idx >,
                        new gum::Set< Idx >(
                           seqv->variable(iInst).domainSize())));   // set sizeof Hashtable
                }

                gum::HashTable< const gum::DiscreteVariable*,
                                std::pair< gum::Set< Idx >*, gum::Set< Idx >* >* >* orderStruct2
                   = orderStruct[instpro.toString()];
                (*orderStruct2)[var]->first->insert(linecount);
                (*orderStruct2)[var]->second->insert(seqv->val(iInst));
              }
            }

            linecount += 1;
          }

          gum::Set< gum::Idx >                  elimination;
          gum::Sequence< gum::Instantiation* >* newSeq = nullptr;

          for (const auto& [first, second]: orderStruct) {
            bool added = false;

            for (const auto& [first2, second2]: *second) {
              if (second2->second->size() == first2->domainSize()) {
                if (!newSeq) newSeq = new gum::Sequence< gum::Instantiation* >();

                if (!added) {
                  added = true;
                  newSeq->insert(
                     new gum::Instantiation(*((*pv)[(*(second2->first->begin()))]), false));
                  newSeq->back()->erase(*first2);
                }

                elimination = elimination + *(second2->first);
              }
            }
          }

          if (newSeq) {
            (itpvall.val())->insert(newSeq);

            for (Idx itelem = pv->size(); itelem > 0; itelem--) {
              if (elimination.exists(itelem - 1)) {
                delete ((*pv)[itelem - 1]);
                pv->erase((*pv)[itelem - 1]);
              }
            }
          }

          while (!orderStruct.empty()) {
            while (!(orderStruct.begin().val())->empty()) {
              delete orderStruct.begin().val()->begin().val()->first;
              delete orderStruct.begin().val()->begin().val()->second;
              (orderStruct.begin().val())
                 ->erase((orderStruct.begin().val())->beginSafe());   // safe iterator needed here
            }

            delete orderStruct.begin().val();
            orderStruct.erase(orderStruct.beginSafe());   // safe iterator needed here
          }
        }

        while (!(itpvall.val())->empty()) {
          auto itpv = (itpvall.val())->begin();

          while (!(*itpv)->empty()) {
            auto itseqv = (*itpv)->begin();

            for (Idx i = 0; i < (*itseqv)->nbrDim(); i++) {
              std::stringstream str;
              str << (*itseqv)->variable(i).name() << "_" << (*itseqv)->val((*itseqv)->variable(i));
              str2 << "-" << vartable[str.str()] << " ";
            }

            if (itpvall.key() != "0" && itpvall.key() != "0.0") {
              std::stringstream strinst;
              strinst << itvar.key()->name();
              strinst << "_val=" << itpvall.key();
              str2 << protable[strinst.str()];
            }

            str2 << " 0\n";
            clause++;
            delete (*itseqv);
            (*itpv)->erase(itseqv);
          }

          delete (*itpv);
          (itpvall.val())->erase(itpv);
        }

        delete (itpvall.val());
        (itvar.val())->erase(itvar.val()->beginSafe());   // safe iterator needed here
      }

      delete (itvar.val());
      cptparamval.erase(cptparamval.beginSafe());   // safe iterator needed here
    }

    clausstr << str2.str();

    output << "p cnf " << num << " " << clause << "\neclauses " << numvar << "\n"
           << clausstr.str() << clausstr2.str() << std::endl;
    output.flush();
  }

  // Writes a Bayesian network in the referenced file using the BN format.
  // If the file doesn't exist, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian network.
  // @param bn The Bayesian network writed in the file.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE void ContextualDependenciesCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
     const std::string&             filePath,
     const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);
    std::ofstream outputvar((filePath + ".var").c_str(), std::ios_base::trunc);

    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : " << filePath << " not writable.")

    std::stringstream strfile, strfile2;

    if (!outputvar.good())
      GUM_ERROR(IOError, "Input/Output error : " << (filePath + ".var") << " not writable.")

    Idx                                num    = 0;
    Idx                                numvar = 0;
    Idx                                clause = 0;
    std::stringstream                  clausstr, clausstr2;
    gum::HashTable< std::string, Idx > vartable;
    gum::HashTable< std::string, Idx > protable;
    gum::HashTable<
       const gum::DiscreteVariable*,
       gum::HashTable< std::string, gum::Sequence< gum::Sequence< gum::Instantiation* >* >* >* >
       cptparamval;

    Instantiation Order;

    for (auto node: bn.topologicalOrder())
      Order.add(bn.variable(node));

    for (auto node: bn.nodes()) {
      std::stringstream       str0;
      const DiscreteVariable* var = &bn.variable(node);

      for (Idx i = 0; i < bn.variable(node).domainSize(); i++) {
        std::stringstream stri;
        stri << var->name() << "_" << var->label(i);
        vartable.insert(stri.str(), ++num);
        strfile << num << "::" << stri.str() << "\n";
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n";
      clause++;
      numvar++;
      clausstr2 << str0.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);
      inst.forgetMaster();
      inst.reorder(Order);
      cptparamval.insert(
         var,
         new gum::HashTable< std::string,
                             gum::Sequence< gum::Sequence< gum::Instantiation* >* >* >());

      for (inst.setFirst(); !inst.end(); ++inst) {
        if (this->fromExact(cpt[inst]) != 1) {
          std::stringstream strk;
          strk << this->fromExact(cpt[inst]);
          std::string valp = strk.str();

          if (!(cptparamval[var])->exists(valp)) {
            (cptparamval[var])
               ->insert(valp,
                        new gum::Sequence< gum::Sequence< gum::Instantiation* >* >());   // remember
                                                                                         // to
            // verify
            // protocole for
            // param = to 1

            (*(cptparamval[var]))[valp]->insert(new gum::Sequence< gum::Instantiation* >);

            if (this->fromExact(cpt[inst])) {
              std::stringstream strinst;
              strinst << var->name();
              strinst << "_val=" << this->fromExact(cpt[inst]);

              if (!protable.exists(strinst.str())) {
                protable.insert(strinst.str(), ++num);
                strfile << num << "::" << strinst.str() << "\n";
              }
            }
          }

          (*(cptparamval[var]))[valp]->front()->insert(new gum::Instantiation(inst));
        }
      }
    }

    std::stringstream str2;

    while (!cptparamval.empty()) {
      auto itvar = cptparamval.begin();

      while (!(itvar.val())->empty()) {
        auto itpvall = (itvar.val())->begin();

        for (auto pv: *itpvall.val()) {
          gum::Idx linecount = 0;
          gum::HashTable< std::string,
                          gum::HashTable< const gum::DiscreteVariable*,
                                          std::pair< gum::Set< Idx >*,
                                                     gum::Set< Idx >* >* >* >
             orderStruct;   // set sizeof Hashtable

          gum::Set< gum::Idx >                               elimination;
          gum::HashTable< std::string, gum::Instantiation* > newSeqpre;

          for (const auto* seqv: *pv) {
            if (seqv->nbrDim() > 1) {
              for (Idx iInst = 0; iInst < seqv->nbrDim(); iInst++) {
                auto                         instpro = new gum::Instantiation(*seqv, false);
                const gum::DiscreteVariable* var     = &(seqv->variable(iInst));
                instpro->erase(*var);
                instpro->reorder(Order);

                if (!orderStruct.exists(instpro->toString())
                    && !newSeqpre.exists(instpro->toString())) {
                  orderStruct.insert(
                     instpro->toString(),
                     new gum::HashTable< const gum::DiscreteVariable*,
                                         std::pair< gum::Set< Idx >*, gum::Set< Idx >* >* >());
                }

                if (orderStruct.exists(instpro->toString())
                    && !orderStruct[instpro->toString()]->exists(var)) {
                  orderStruct[instpro->toString()]->insert(
                     var,
                     new std::pair< gum::Set< Idx >*, gum::Set< Idx >* >(
                        new gum::Set< Idx >,
                        new gum::Set< Idx >(
                           seqv->variable(iInst).domainSize())));   // set sizeof Hashtable
                }

                if (orderStruct.exists(instpro->toString())
                    && !newSeqpre.exists(instpro->toString())) {
                  gum::HashTable< const gum::DiscreteVariable*,
                                  std::pair< gum::Set< Idx >*, gum::Set< Idx >* >* >* orderStruct2
                     = orderStruct[instpro->toString()];
                  (*orderStruct2)[var]->first->insert(linecount);
                  (*orderStruct2)[var]->second->insert(seqv->val(iInst));

                  if ((*orderStruct2)[var]->second->size() == var->domainSize()) {
                    newSeqpre.insert(instpro->toString(), instpro);

                    for (const auto& elt: *orderStruct2) {
                      elimination = elimination + *(elt.second->first);
                      delete (elt.second->first);
                      delete (elt.second->second);
                      delete (elt.second);
                    }

                    orderStruct2->clear();

                    delete orderStruct2;
                    orderStruct.erase(instpro->toString());
                  } else delete instpro;
                } else if (newSeqpre.exists(instpro->toString())) {
                  elimination.insert(linecount);
                  delete instpro;
                }
              }
            }

            linecount += 1;
          }

          gum::Sequence< gum::Instantiation* >* newSeq = nullptr;

          if (!newSeqpre.empty()) {
            newSeq = new gum::Sequence< gum::Instantiation* >();

            for (auto& elt: newSeqpre)
              newSeq->insert(elt.second);

            (itpvall.val())->insert(newSeq);

            for (Idx itelem = pv->size(); itelem > 0; itelem--) {
              if (elimination.exists(itelem - 1)) {
                delete ((*pv)[itelem - 1]);
                pv->erase((*pv)[itelem - 1]);
              }
            }
          }

          while (!orderStruct.empty()) {
            while (!(orderStruct.begin().val())->empty()) {
              delete orderStruct.begin().val()->begin().val()->first;
              delete orderStruct.begin().val()->begin().val()->second;
              (orderStruct.begin().val())->erase(orderStruct.begin().val()->beginSafe());   // safe
              // iterator
              // needed here
            }

            delete orderStruct.begin().val();
            orderStruct.erase(orderStruct.beginSafe());   // safe iterator needed here
          }
        }

        while (!(itpvall.val())->empty()) {
          gum::Sequence< gum::Sequence< gum::Instantiation* >* >::iterator_safe itpv
             = (itpvall.val())->begin();

          while (!(*itpv)->empty()) {
            gum::Sequence< gum::Instantiation* >::iterator_safe itseqv = (*itpv)->begin();

            for (Idx i = 0; i < (*itseqv)->nbrDim(); i++) {
              std::stringstream str;
              str << (*itseqv)->variable(i).name() << "_" << (*itseqv)->val((*itseqv)->variable(i));
              str2 << "-" << vartable[str.str()] << " ";
            }

            /*if (itpvall.key().compare("0") != 0 &&
                itpvall.key().compare("0.0") != 0) {  */
            if (itpvall.key() != "0" && itpvall.key() != "0.0") {
              std::stringstream strinst;
              strinst << itvar.key()->name();
              strinst << "_val=" << itpvall.key();
              str2 << protable[strinst.str()];
            }

            str2 << " 0\n";
            clause++;
            delete (*itseqv);
            (*itpv)->erase((*itpv)->beginSafe());   // safe iterator needed here
          }

          delete (*itpv);
          (itpvall.val())->erase(itpvall.val()->beginSafe());   // safe iterator needed here
        }

        delete (itpvall.val());
        (itvar.val())->erase(itvar.val()->beginSafe());   // safe iterator needed here
      }

      delete (itvar.val());
      cptparamval.erase(cptparamval.beginSafe());   // safe iterator needed here
    }

    clausstr << str2.str();

    output << "p cnf " << num << " " << clause << "\neclauses " << numvar << "\n"
           << clausstr.str() << clausstr2.str() << std::endl;
    output.flush();
    outputvar << strfile.str();
    outputvar.flush();

    outputvar.close();
    output.close();

    if (outputvar.fail()) GUM_ERROR(IOError, "Writing in the ostream failed.")

    if (output.fail()) GUM_ERROR(IOError, "Writing in the ostream failed.")
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
