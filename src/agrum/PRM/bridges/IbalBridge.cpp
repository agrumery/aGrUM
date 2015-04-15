// /***************************************************************************
//  *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
//  *   {prenom.nom}_at_lip6.fr                                               *
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   This program is distributed in the hope that it will be useful,       *
//  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//  *   GNU General Public License for more details.                          *
//  *                                                                         *
//  *   You should have received a copy of the GNU General Public License     *
//  *   along with this program; if not, write to the                         *
//  *   Free Software Foundation, Inc.,                                       *
//  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
//  ***************************************************************************/
// /**
//  * @file
//  * @brief Implementation of IBALBridge.
//  *
//  * @author Lionel TORTI and Pierre-Henri WUILLEMIN
//  */
//
// #include <agrum/PRM/bridges/IbalBridge.h>
//
// #ifdef GUM_NO_INLINE
// #include <agrum/PRM/bridges/IbalBridge.inl>
// #endif // GUM_NO_INLINE
//
// namespace gum {
// namespace prm {
//
//   // Default constructor.
//   IBALBridge::IBALBridge()
//   {
//     GUM_CONSTRUCTOR( IBALBridge );
//   }
//
//   // Destructor.
//   IBALBridge::~IBALBridge()
//   {
//     GUM_DESTRUCTOR( IBALBridge );
//     typedef HashTable<const ClassElementContainer*,
//     IBALBridge::ParamData*>::iterator
//             ParamIter;
//     for (ParamIter iter = __paramData.begin(); iter != __paramData.end(); ++iter)
//     {
//       delete *iter;
//     }
//     __paramData.clear();
//   }
//
//   // Copy constructor is illegal.
//   IBALBridge::IBALBridge(const IBALBridge& source)
//   {
//     GUM_CONS_CPY( IBALBridge );
//     GUM_ERROR(FatalError, "Illegal copy constructor.");
//   }
//
//   // Copy operator is illegal.
//   IBALBridge&
//   IBALBridge::operator=(const IBALBridge& source)
//   {
//     GUM_ERROR(FatalError, "Illegal copy operator.");
//   }
//
//
//   // Prints the given PRM's classes and models in the stream in the IBAL
//   // formalism.
//   // @param output The stream in which the result is printed.
//   // @param prm A constant reference over the printed PRM.
//   void
//   IBALBridge::print(std::ostream& output, const PRM& prm)
//   {
//
//     output << "data List [a] =  Nil | Cons (a, List [a])" << std::endl;
//
//     for (Sequence<Class*>::const_iterator iter = prm.classes().begin();
//          iter != prm.classes().end(); ++iter) {
//       __print_class(output, prm, **iter);
//     }
//     for (Sequence<Model*>::const_iterator iter = prm.models().begin();
//          iter != prm.models().end(); ++iter) {
//       __print_model(output, prm, **iter);
//     }
//     output << std::endl << std::flush;
//   }
//
//   // Prints the given class in the stream in the IBAL formalism.
//   void
//   IBALBridge::__print_class(std::ostream& output,
//                             const PRM& prm, const Class& c)
//   {
//     __paramData.insert(&c, new IBALBridge::ParamData());
//     std::stringstream buffer;
//     buffer << __removeDots(c.name()) << " (";
//     Size counter = 1;
//     List<NodeId> nodes_list; // Used to parse the graph in width
//     NodeProperty<bool> visit_map;
//     bool first = true;
//     for (DAG::NodeIterator iter = c.dag().beginNodes(); iter !=
//     c.dag().endNodes(); ++iter) {
//       __print_param(buffer, prm, c, c.get(*iter), counter, first);
//       if (c.dag().parents(*iter).size() == 0) {
//         nodes_list.push_back(*iter);
//       }
//       visit_map.insert(*iter, false);
//     }
//     buffer << ") = {" << std::endl;
//
//     typedef HashTable<const ClassElement*, std::string>::const_iterator ParamIter;
//     for (ParamIter iter = __paramData[&c]->paramName.begin();
//          iter != __paramData[&c]->paramName.end(); ++iter) {
//       if (not (static_cast<const ReferenceSlot*>(iter.key()))->isArray() ) {
//         buffer << "  " << iter.key()->name() << " = " << *iter << ";" <<
//         std::endl;
//       }
//     }
//
//     while (not nodes_list.empty()) {
//       visit_map[nodes_list.front()] = true;
//       for (DAG::ArcIterator iter = c.dag().children(nodes_list.front()).begin();
//            iter != c.dag().children(nodes_list.front()).end(); ++iter) {
//         if (not visit_map[iter->head()]) {
//           visit_map[iter->head()] = true;
//           nodes_list.push_back(iter->head());
//         }
//       }
//       __print_c_elt(buffer, prm, c, c.get(nodes_list.front()));
//       nodes_list.popFront();
//     }
//     buffer << "}" << std::endl;
//
//     std::string tmp = toLowerCase(buffer.str());
//     // Need to restore nil to Nil...
//     std::string nil = " nil ";
//     size_t pos = 0;
//     while (true) {
//       pos = tmp.find(nil, pos);
//       if (pos != std::string::npos) {
//         tmp.replace(pos, 5, " Nil ");
//       } else {
//         break;
//       }
//     }
//     output << tmp << std::endl << std::flush;
//   }
//
//   // Print a parameter if necessary and add it to the parameter map for future
//   use.
//   void
//   IBALBridge::__print_param(std::ostream& output, const PRM& prm,
//                             const Class& c, const ClassElement& elt,
//                             Size& counter, bool& first)
//   {
//     std::stringstream param;
//     switch (elt.elt_type()) {
//       case ClassElement::prm_refslot: {
//                                         if (not static_cast<const
//                                         ReferenceSlot&>(elt).isArray()) {
//                                           if (not first) { param << ", "; }
//                                           else           { first = false; }
//                                           std::stringstream tmp;
//                                           tmp << "ref" << counter++;
//                                           param << tmp.str();
//                                           __paramData[&c]->paramName.insert(&elt,
//                                           tmp.str());
//                                           __paramData[&c]->paramSequence.insert(&elt);
//                                         }
//                                         break;
//                                       }
//       case ClassElement::prm_aggregate: {
//                                           if (not first) { param << ", "; }
//                                           else           { first = false; }
//                                           std::stringstream tmp;
//                                           tmp << "ref" << counter++;
//                                           param << tmp.str();
//                                           __paramData[&c]->paramName.insert(&elt,
//                                           tmp.str());
//                                           __paramData[&c]->paramSequence.insert(&elt);
//                                           break;
//                                         }
//       default: { /* Do nothing */ break; }
//     }
//     output << param.str();
//   }
//
//   // Call the good sub method for the given ClassElement
//   void
//   IBALBridge::__print_c_elt(std::ostream& output, const PRM& prm,
//                             const Class& c, const ClassElement& elt)
//   {
//     switch (elt.elt_type()) {
//       case ClassElement::prm_aggregate: {
//                             __print_agg(output, prm, c, static_cast<const
//                             Aggregate&>(elt));
//                             break;
//                           }
//       case ClassElement::prm_attribute: {
//                             __print_att(output, prm, c, static_cast<const
//                             Attribute&>(elt));
//                             break;
//                           }
//       default: { /* Do nothing */ break; }
//     }
//   }
//
//   // Prints the given aggregate in the stream in the IBAL formalism.
//   void
//   IBALBridge::__print_agg(std::ostream& output, const PRM& prm,
//                           const Class& c, const Aggregate& agg)
//   {
//     std::stringstream buffer;
//     buffer << "  " << agg.name() << "(l) = case l of" << std::endl;
//     buffer << "    " << "# Nil : ";
//
//     const DiscreteVariable* apply_on = 0;
//     if (c.dag().parents(agg.id()).size() > 0) {
//       const ClassElement& parent =
//       c.get(c.dag().parents(agg.id()).begin()->tail());
//       apply_on = &(parent.type().variable());
//     } else {
//       GUM_ERROR(NotFound, "aggregator without any parents");
//     }
//
//     switch (agg.agg_type()) {
//       case Aggregate::agg_exists: {
//                                     buffer << "false" << std::endl;
//                                     buffer << "    # ";
//                                     buffer << __print_label(*apply_on,
//                                     agg.label());
//                                     buffer << " :: _ : true" << std::endl;
//                                     buffer << "    # _ :: xs : ";
//                                     buffer << agg.name() << "(xs)" << std::endl <<
//                                     std::endl;
//                                     break;
//                                   }
//       case Aggregate::agg_forall: {
//                                     buffer << "true" << std::endl;
//                                     buffer << "    # ";
//                                     buffer << __print_label(*apply_on,
//                                     agg.label());
//                                     buffer << " :: xs : " << agg.name() << "(xs)"
//                                     << std::endl;
//                                     buffer << "    # _ :: _ : false" << std::endl
//                                     << std::endl;
//                                     break;
//                                   }
//       default: {
//                  GUM_ERROR(FatalError, "Aggregate not implemented yet!");
//                }
//     }
//     output << buffer.str();
//   }
//
//   // Prints the given attribute in the stream in the IBAL formalism.
//   void
//   IBALBridge::__print_att(std::ostream& output, const PRM& prm,
//                           const Class& c, const Attribute& attr)
//   {
//     if (c.dag().parents(attr.id()).size() == 0) {
//       __print_att_no_parents(output, prm, c, attr);
//     } else if (c.dag().parents(attr.id()).size() == 1) {
//       __print_att_one_parents(output, prm, c, attr);
//     } else {
//       __print_att_lots_of_parents(output, prm, c, attr);
//     }
//   }
//
//   // Prints the given attribute with no parent.
//   void
//   IBALBridge::__print_att_no_parents(std::ostream& output, const PRM& prm,
//                                      const Class& c, const Attribute& attr)
//   {
//     std::stringstream buffer;
//     //std::string priv_flag = (attr.innerNode())?"private ":"";
//     std::string priv_flag = (c.isInnerNode(attr.id()))?"":"";
//     buffer << "  " << priv_flag << attr.name() << " = dist [ ";
//     Instantiation inst(attr.cpf());
//     bool first = true;
//     for (inst.setFirst(); not inst.end(); inst.inc()) {
//       if (not first) { buffer << ", "; }
//       else           { first = false; }
//       buffer << __cleanFloatValue(attr.cpf().get(inst)) << ": ";
//       buffer << __print_label(attr.type().variable(),
//       inst.val(attr.type().variable()));
//     }
//     buffer << " ];" << std::endl << std::endl;
//     output << buffer.str();
//   }
//
//   // Prints the given attribute with one parents.
//   void
//   IBALBridge::__print_att_one_parents(std::ostream& output, const PRM& prm,
//                                       const Class& c, const Attribute& attr)
//   {
//     std::stringstream buffer;
//     Sequence<const DiscreteVariable*> parents;
//     for (DAG::ArcIterator iter = c.dag().parents(attr.id()).begin();
//          iter != c.dag().parents(attr.id()).end(); ++iter) {
//       parents.insert(&(c.get(iter->tail()).type().variable()));
//     }
//     Instantiation inst(attr.cpf());
//     Instantiation var;
//     var.add(attr.type().variable());
//
//     //std::string priv_flag = (attr.innerNode())?"private ":"";
//     std::string priv_flag = (c.isInnerNode(attr.id()))?"":"";
//     buffer << "  " << priv_flag << attr.name() << " = case " << __print_parents(c,
//     attr);
//     buffer << " of" << std::endl;
//
//     inst.setFirstOut(var);
//     while (true) {
//       buffer << "    # " << __print_label(*(parents.atPos(0)),
//       inst.val(*(parents.atPos(0))));
//       buffer << " : dist [ ";
//       bool first = true;
//       for (inst.setFirstIn(var); not inst.end(); inst.incIn(var)) {
//         if (not first) { buffer << ", "; }
//         else           { first = false; }
//         buffer << __cleanFloatValue(attr.cpf().get(inst)) << ": ";
//         buffer << __print_label(attr.type().variable(),
//         inst.val(attr.type().variable()));
//       }
//       inst.unsetOverflow();
//       buffer << " ]";
//
//       inst.incOut(var);
//       if (inst.end()) { buffer << ";" << std::endl << std::endl; break; }
//       else            { buffer << std::endl; }
//     }
//     output << buffer.str();
//   }
//
//   // Prints the given attribute with several parents.
//   void
//   IBALBridge::__print_att_lots_of_parents(std::ostream& output, const PRM& prm,
//                                           const Class& c, const Attribute& attr)
//   {
//     std::stringstream buffer;
//     Sequence<const DiscreteVariable*> parents;
//     for (DAG::ArcIterator iter = c.dag().parents(attr.id()).begin();
//          iter != c.dag().parents(attr.id()).end(); ++iter) {
//       parents.insert(&(c.get(iter->tail()).type().variable()));
//     }
//     Instantiation inst(attr.cpf());
//     Instantiation var;
//     var.add(attr.type().variable());
//
//     buffer << "  " << attr.name() << " = case <" << __print_parents(c, attr);
//     buffer << "> of" << std::endl;
//
//     inst.setFirstOut(var);
//     while (true) {
//       buffer << "    # <";
//       typedef Sequence<const DiscreteVariable*>::iterator var_iter;
//       bool first = true;
//       for (var_iter iter = parents.begin(); iter != parents.end(); ++iter) {
//         if (not first) { buffer << ", "; }
//         else           { first = false; }
//         buffer << __print_label(**iter, inst.val(**iter));
//       }
//       buffer << "> : dist [ ";
//       first = true;
//       for (inst.setFirstIn(var); not inst.end(); inst.incIn(var)) {
//         if (not first) { buffer << ", "; }
//         else           { first = false; }
//         buffer << __cleanFloatValue(attr.cpf().get(inst)) << ": ";
//         buffer << __print_label(attr.type().variable(),
//         inst.val(attr.type().variable()));
//       }
//       inst.unsetOverflow();
//       buffer << " ]";
//
//       inst.incOut(var);
//       if (inst.end()) { buffer << ";" << std::endl << std::endl; break; }
//       else            { buffer << std::endl; }
//     }
//     output << buffer.str();
//   }
//
//   // In IBAL dots in names are not welcome, this method replace them with "_".
//   std::string
//   IBALBridge::__removeDots(const std::string& str, char replace, char with)
//   {
//     std::string retVal = str;
//     size_t pos = retVal.find(replace, (size_t) 0);
//     while (pos != std::string::npos) {
//       retVal[pos] = with;
//       pos = retVal.find(replace, pos);
//     }
//     return retVal;
//   }
//
//   // Print the value of label prefix by a quote if it's not a boolean
//   std::string
//   IBALBridge::__print_label(const DiscreteVariable& var, Idx pos)
//   {
//     if ((var.label(pos) == "false") or (var.label(pos) == "true")) {
//       return var.label(pos);
//     } else {
//       std::string quote = "'";
//       return quote + var.label(pos);
//     }
//   }
//
//   // Print the parents of the attribute separated by ,
//   std::string
//   IBALBridge::__print_parents(const Class& c, const Attribute& attr)
//   {
//     std::stringstream buffer;
//     bool first = true;
//     for (DAG::ArcIterator iter = c.dag().parents(attr.id()).begin();
//          iter != c.dag().parents(attr.id()).end(); ++iter)
//     {
//       if (not first) { buffer << ", "; }
//       else           { first = false; }
//       switch (c.get(iter->tail()).elt_type()) {
//         case ClassElement::prm_slotchain:
//         case ClassElement::prm_attribute: {
//                                             buffer << c.get(iter->tail()).name();
//                                             break;
//                                           }
//         case ClassElement::prm_aggregate: {
//                                             buffer << c.get(iter->tail()).name();
//                                             buffer << "(" <<
//                                             __paramData[&c]->paramName[&(c.get(iter->tail()))]
//                                             << ")";
//                                             break;
//                                           }
//         default: { /* do nothing */ break; }
//       }
//     }
//     return buffer.str();
//   }
//
//   // Prints the given model in the stream in the IBAL formalism.
//   void
//   IBALBridge::__print_model(std::ostream& output, const PRM& prm, const Model& m)
//   {
//     for (Sequence<Class*>::const_iterator iter = prm.classes().begin();
//          iter != prm.classes().end(); ++iter) {
//       try {
//         for (Set<Instance*>::const_iterator jter = m.get(**iter).begin();
//              jter != m.get(**iter).end(); ++jter) {
//           __print_instance(output, prm, m, **jter);
//         }
//       } catch (NotFound&) {
//         // Happens if the given class has no instantiation
//       }
//     }
//   }
//
//   // Prints the given instance in the stream in the IBAL formalism.
//   void
//   IBALBridge::__print_instance(std::ostream& output, const PRM& prm,
//                                const Model& m, const Instance& i)
//   {
//     std::stringstream buffer;
//
//     buffer << __cleanInstanceName(i.name()) << " = " <<
//     __removeDots(i.type().name());
//     buffer << "(";
//     bool first = true;
//     typedef Sequence< const ClassElement* >::iterator param_iter;
//     for(param_iter iter = __paramData[&(i.type())]->paramSequence.begin();
//         iter != __paramData[&(i.type())]->paramSequence.end(); ++iter) {
//       switch ((*iter)->elt_type()) {
//         case ClassElement::prm_refslot: {
//                                           if (not first) { buffer << ", "; }
//                                           else           { first = false; }
//                                           buffer <<
//                                           __cleanInstanceName(i.getInstance((*iter)->id()).name());
//                                           break;
//                                         }
//         case ClassElement::prm_aggregate: {
//                                           if (not first) { buffer << ", "; }
//                                           else           { first = false; }
//                                           buffer << __print_agg_parents_list(m, i,
//                                           (*iter)->id());
//                                           break;
//                                           }
//         default: { /* do nothing */ break; }
//       }
//     }
//     buffer << ");" << std::endl;
//
//     output << toLowerCase(buffer.str());
//   }
//
//   // Prints the list of parents of a given aggregator in the IBAL formalism.
//   std::string
//   IBALBridge::__print_agg_parents_list(const Model& m, const Instance& i, NodeId
//   id)
//   {
//     std::stringstream buffer;
//     buffer << "[ ";
//
//     typedef Set<Instance*>::iterator inst_iter;
//     bool first = true;
//     for (DAG::ArcIterator iter = i.type().dag().parents(id).begin(); iter !=
//     i.type().dag().parents(id).end(); ++iter) {
//       std::string attr_name = static_cast<const
//       SlotChain&>(i.type().get(iter->tail())).lastElt().name();
//       for (inst_iter jter = i.getInstances(iter->tail()).begin();
//           jter != i.getInstances(iter->tail()).end(); ++jter) {
//         if (not first) { buffer << ", "; }
//         else           { first = false; }
//         buffer << __cleanInstanceName((*jter)->name()) << "." << attr_name;
//       }
//     }
//
//     buffer << " ]";
//     return toLowerCase(buffer.str());
//   }
//
//   // Clean instance's name.
//   std::string
//   IBALBridge::__cleanInstanceName(const std::string& s)
//   {
//     return __removeDots(__removeDots(s, '[', '_'), ']', '_');
//   }
//
//
//   // Transforms 1 and 0 in 1.0 and 0.0
//   std::string
//   IBALBridge::__cleanFloatValue(prm_float f)
//   {
//     if (f == (prm_float) 0) { return "0.0"; }
//     else if (f == (prm_float) 1) { return "1.0"; }
//     else {
//       std::stringstream retVal;
//       retVal << f;
//       return retVal.str();
//     }
//   }
//
//   void
//   IBALBridge::printObservations(std::ostream& output, const PRM& prm,
//                                 const Model& m, const PRMInference& obs)
//   {
//     for (Model::const_iterator iter = m.begin(); not iter.isEnd(); ++iter) {
//       if (obs.hasEvidence(*iter)) {
//         std::string prefix = toLowerCase(__cleanInstanceName(iter->name()));
//         for (PRMInference::EMapConstIterator jter = obs.evidence(*iter).begin();
//              jter != obs.evidence(*iter).end(); ++jter) {
//           output << "obs " << prefix << "." <<
//           toLowerCase(iter->get(jter.key()).name()) << " = ";
//           Instantiation i(**jter);
//           const DiscreteVariable* var = (**jter).variablesSequence().atPos(0);
//           for (i.setFirst(); not i.end(); i.inc()) {
//             if ((**jter).get(i) == (prm_float) 1) {
//               output << "'" << toLowerCase(var->label(i.val(var))) << ";" <<
//               std::endl;
//             }
//           }
//         }
//       }
//     }
//   }
//
//   std::string
//   IBALBridge::toLowerCase(std::string strToConvert)
//   {
//     for(unsigned int i=0;i<strToConvert.length();i++) {
//       strToConvert[i] = std::tolower(strToConvert[i]);
//     }
//     return strToConvert;
//   }
//
// } /* namespace prm */
// } /* namespace gum */
//
