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
//  * @brief Headers of IBALBridge.
//  *
//  * @author Lionel TORTI and Pierre-Henri WUILLEMIN
//  */
//
// #include <cctype>
// #include <cstdlib>
// #include <string>
// #include <sstream>
// #include <ostream>
//
// #include <agrum/PRM/PRM.h>
// #include <agrum/PRM/PRMInference.h>
//
// #ifndef GUM_IBAL_BRIDGE_H
// #define GUM_IBAL_BRIDGE_H
// namespace gum {
// namespace prm {
// REMOVED FROM DOCS (see agrum.doxy.in)
// /**
//  * @class IBALBridge IbalBridge.h <agrum/PRM/ibal/IbalBridge.h>
//  * @brief This class writes a PRM in the IBAL formalism.
//  *
//  * @todo Add private flag when it's correctly implemented.
//  */
// class IBALBridge {
//   public:
//   // ========================================================================
//   /// @name Constructor & destructor.
//   // ========================================================================
//     /// @{
//
//     /// Default constructor.
//     IBALBridge();
//
//     /// Destructor.
//     ~IBALBridge();
//
//     /// @}
//
//     /**
//      * Prints the given PRM's classes and models in the stream in the IBAL
//      * formalism.
//      * @param output The stream in which the result is printed.
//      * @param prm A constant reference over the printed PRM.
//      */
//     void print(std::ostream& output, const PRM& prm);
//
//     /**
//      * Prints the observation in obs over instances in m in the stream in the
//      * IBAL formalism.
//      */
//     void printObservations(std::ostream& output, const PRM& prm,
//                            const Model& m, const PRMInference& obs);
//
//   private:
//   // ========================================================================
//   /// @name Private methods.
//   // ========================================================================
//     /// @{
//
//     /// Copy constructor is illegal.
//     IBALBridge(const IBALBridge& source);
//
//     /// Copy operator is illegal.
//     IBALBridge& operator=(const IBALBridge& source);
//
//     /// In IBAL dots in names are not welcome, this method replace them with
//     /// "_".
//     std::string __removeDots(const std::string& str, char replace = '.',
//                                                      char with = '_');
//
//     /// Clean instance's name.
//     std::string __cleanInstanceName(const std::string& s);
//
//     /// Transforms 1 and 0 in 1.0 and 0.0
//     std::string __cleanFloatValue(prm_float f);
//
//     std::string toLowerCase(std::string strToConvert);
//
//     // Print the value of label prefix by a quote if it's not a boolean
//     std::string __print_label(const DiscreteVariable& var, Idx pos);
//
//     // Print the parents of the attribute separated by ,
//     std::string __print_parents(const Class& c, const Attribute& attr);
//
//     /// @}
//   // ========================================================================
//   /// @name Private methods for printing Class related content.
//   // ========================================================================
//     /// @{
//
//     /// Prints the given class in the stream in the IBAL formalism.
//     void __print_class(std::ostream& output, const PRM& prm, const Class& c);
//
//     /// Print a parameter if necessary and add it to the parameter map for
//     /// future use.
//     void __print_param(std::ostream& output, const PRM& prm,
//                        const Class& c, const ClassElement& elt,
//                        Size& counter, bool& first);
//
//     /// Call the good sub method for the given ClassElement
//     void
//     __print_c_elt(std::ostream& output, const PRM& prm,
//                   const Class& c, const ClassElement& elt);
//
//     /// Prints the given aggregate in the stream in the IBAL formalism.
//     void __print_agg(std::ostream& output, const PRM& prm,
//                      const Class& c, const Aggregate& agg);
//
//     /// Prints the given attribute in the stream in the IBAL formalism.
//     void __print_att(std::ostream& output, const PRM& prm,
//                      const Class& c, const Attribute& attr);
//
//     /// Prints the given attribute with no parent.
//     void __print_att_no_parents(std::ostream& output, const PRM& prm,
//                                 const Class& c, const Attribute& attr);
//
//     /// Prints the given attribute with one parents.
//     void __print_att_one_parents(std::ostream& output, const PRM& prm,
//                                  const Class& c, const Attribute& attr);
//
//     /// Prints the given attribute with several parents.
//     void __print_att_lots_of_parents(std::ostream& output, const PRM& prm,
//                                      const Class& c, const Attribute& attr);
//
//     /// @}
//   // ========================================================================
//   /// @name Private methods for printing Model related content.
//   // ========================================================================
//     /// @{
//
//     /// Prints the given model in the stream in the IBAL formalism.
//     void __print_model(std::ostream& output, const PRM& prm, const Model& m);
//
//     /// Prints the given instance in the stream in the IBAL formalism.
//     void __print_instance(std::ostream& output, const PRM& prm, const Model& m,
//     const Instance& i);
//
//
//     /// Prints the list of parents of a given aggregator in the IBAL formalism.
//     std::string __print_agg_parents_list(const Model& m, const Instance& i, NodeId
//     id);
//
//     /// @}
//   #ifndef DOXYGEN_SHOULD_SKIP_THIS
//   // ========================================================================
//   /// @name Private class.
//   // ========================================================================
//     /// @{
//
//     /// @class ParamData
//     /// @brief Used to hold data about parameters of a class in it's IBAL
//     ///        equivalent.
//     struct ParamData
//     {
//       HashTable<const ClassElement*, std::string> paramName;
//       Sequence<const ClassElement*> paramSequence;
//     };
//
//     /// @}
//   #endif // DOXYGEN_SHOULD_SKIP_THIS
//   // ========================================================================
//   /// @name Private members.
//   // ========================================================================
//     /// @{
//
//     /// Mapping between Class and data about their parameters in their IBAL
//     /// equivalent.
//     HashTable<const ClassElementContainer*, IBALBridge::ParamData*> __paramData;
//
//     /// @}
// };
//
// } /* namespace prm */
// } /* namespace gum */
// #ifndef GUM_NO_INLINE
// #include <agrum/PRM/bridges/IbalBridge.inl>
// #endif // GUM_NO_INLINE
//
// #endif /* GUM_IBAL_BRIDGE_H */
//
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
