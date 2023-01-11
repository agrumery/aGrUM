/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#  include <agrum/BN/io/cnf/GeneralizedCNFWriter.h>

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE GeneralizedCNFWriter< GUM_SCALAR, IApproximationPolicy >::GeneralizedCNFWriter() {
    GUM_CONSTRUCTOR(GeneralizedCNFWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE GeneralizedCNFWriter< GUM_SCALAR, IApproximationPolicy >::~GeneralizedCNFWriter() {
    GUM_DESTRUCTOR(GeneralizedCNFWriter);
  }

  //
  // Writes a Bayesian network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE void GeneralizedCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
     std::ostream&                  output,
     const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.")

    std::stringstream strfile, strfile2;

    Size num      = 0;
    Size numparam = 0;

    for (auto node: bn.nodes())
      numparam += bn.variable(node).domainSize();

    Idx                                clause = 0;
    std::stringstream                  clausstr;
    gum::HashTable< std::string, Idx > vartable;   // key name::label val num;
    gum::HashTable< std::string, Idx > protable;

    for (auto node: bn.nodes()) {
      const auto& var = bn.variable(node);

      for (Idx i = 0; i < var.domainSize(); i++) {
        std::stringstream str;
        str << var.name() << "_" << var.label(i);
        vartable.insert(str.str(), ++num);
        strfile << num << "::" << str.str() << "\n";
      }

      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

      Instantiation inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        std::stringstream strinst;
        strinst << inst.toString();
        strinst << "_val=" << this->fromExact(cpt[inst]);

        protable.insert(inst.toString(), ++numparam);
        strfile2 << numparam << "::" << strinst.str() << "\n";
      }
    }

    for (auto node: bn.nodes()) {
      const auto&       var = bn.variable(node);
      std::stringstream str0, str1, str2, str3;

      for (Idx i = 0; i < var.domainSize(); i++) {
        std::stringstream stri;   //= bn.variable(iter).name()+"_"+
        // bn.variable(iter).label( i ) ;
        stri << var.name() << "_" << var.label(i);
        str0 << vartable[stri.str()] << " ";

        for (Idx j = i + 1; j < var.domainSize(); j++) {
          std::stringstream strj;
          strj << var.name() << "_" << var.label(j);
          str1 << "-" << vartable[stri.str()] << " -" << vartable[strj.str()] << " 0\n";
          clause++;
        }
      }

      str0 << "0\n";
      clause++;
      clausstr << str0.str() << str1.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        for (Idx i = 0; i < inst.nbrDim(); i++) {
          std::stringstream str;
          str << inst.variable(i).name() << "_" << inst.val(inst.variable(i));
          str2 << "-" << vartable[str.str()] << " ";
          str3 << "-" << protable[inst.toString()] << " " << vartable[str.str()] << " 0\n";
          clause++;
        }

        str2 << protable[inst.toString()] << " 0\n";
        clause++;
      }

      clausstr << str2.str() << str3.str();
    }

    output << "p cnf " << num + numparam << " " << clause << "\n" << clausstr.str() << std::endl;
    output.flush();
  }

  // Writes a Bayesian network in the referenced file using the BN format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian network.
  // @param bn The Bayesian network writed in the file.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE void GeneralizedCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
     const std::string&             filePath,
     const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);
    std::ofstream outputvar((filePath + ".var").c_str(), std::ios_base::trunc);

    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : " << filePath << " not writable.")

    std::stringstream strfile, strfile2;

    if (!outputvar.good())
      GUM_ERROR(IOError, "Input/Output error : " << (filePath + ".var") << " not writable.")

    Idx num      = 0;
    Idx numparam = 0;

    for (auto node: bn.nodes())
      numparam += bn.variable(node).domainSize();

    Idx                                clause = 0;
    std::stringstream                  clausstr;
    gum::HashTable< std::string, Idx > vartable;   // key name::label val num;
    gum::HashTable< std::string, Idx > protable;

    for (auto node: bn.nodes()) {
      const auto& var = bn.variable(node);

      for (Idx i = 0; i < var.domainSize(); i++) {
        std::stringstream str;
        str << var.name() << "_" << var.label(i);
        vartable.insert(str.str(), ++num);
        strfile << num << "::" << str.str() << "\n";
      }

      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

      Instantiation inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        std::stringstream strinst;
        strinst << inst.toString();
        strinst << "_val=" << this->fromExact(cpt[inst]);

        protable.insert(inst.toString(), ++numparam);
        strfile2 << numparam << "::" << strinst.str() << "\n";
      }
    }

    for (auto node: bn.nodes()) {
      const auto&       var = bn.variable(node);
      std::stringstream str0, str1, str2, str3;

      for (Idx i = 0; i < var.domainSize(); i++) {
        std::stringstream stri;   //= bn.variable(iter).name()+"_"+
        // bn.variable(iter).label( i ) ;
        stri << var.name() << "_" << var.label(i);
        str0 << vartable[stri.str()] << " ";

        for (Idx j = i + 1; j < var.domainSize(); j++) {
          std::stringstream strj;
          strj << var.name() << "_" << var.label(j);
          str1 << "-" << vartable[stri.str()] << " -" << vartable[strj.str()] << " 0\n";
          clause++;
        }
      }

      str0 << "0\n";
      clause++;
      clausstr << str0.str() << str1.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        for (Idx i = 0; i < inst.nbrDim(); i++) {
          std::stringstream str;
          str << inst.variable(i).name() << "_" << inst.val(inst.variable(i));
          str2 << "-" << vartable[str.str()] << " ";
          str3 << "-" << protable[inst.toString()] << " " << vartable[str.str()] << " 0\n";
          clause++;
        }

        str2 << protable[inst.toString()] << " 0\n";
        clause++;
      }

      clausstr << str2.str() << str3.str();
    }

    output << "p cnf " << num + numparam << " " << clause << "\n" << clausstr.str() << std::endl;
    output.flush();
    outputvar << strfile.str() << strfile2.str();
    outputvar.flush();
    outputvar.close();
    output.close();

    if (outputvar.fail()) GUM_ERROR(IOError, "Writing in the ostream failed.")

    if (output.fail()) GUM_ERROR(IOError, "Writing in the ostream failed.")
  }

  // Returns a bloc defining a variable's CPT in the BN format.
  /*template<typename GUM_SCALAR, template<class> class IApproximationPolicy >
  INLINE
    std::string
    CNFWriter<GUM_SCALAR>:: _variableCPT_( const Potential<GUM_SCALAR>& cpt ) {
    std::stringstream str;
    str << "";
    return str.str();
    }

  // Returns the header of the BN file.
  template<typename GUM_SCALAR,> INLINE
  std::string
  CNFWriter<GUM_SCALAR>:: _header_( const IBayesNet<GUM_SCALAR>& ) {
  std::stringstream str;
  str << "";
  return str.str();
  }

  // Returns a bloc defining a variable in the BN format.
  template<typename GUM_SCALAR> INLINE
  std::string
  CNFWriter<GUM_SCALAR>:: _variableBloc_( const DiscreteVariable& var ) {
  std::stringstream str;
  str << "" ;
  return str.str();
  }*/

  // Returns the modalities labels of the variables in varsSeq

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
