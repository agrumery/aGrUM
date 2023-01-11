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
#  include <agrum/BN/io/cnf/FactorisedValuesCNFWriter.h>

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE
     FactorisedValuesCNFWriter< GUM_SCALAR, IApproximationPolicy >::FactorisedValuesCNFWriter() {
    GUM_CONSTRUCTOR(FactorisedValuesCNFWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE
     FactorisedValuesCNFWriter< GUM_SCALAR, IApproximationPolicy >::~FactorisedValuesCNFWriter() {
    GUM_DESTRUCTOR(FactorisedValuesCNFWriter);
  }

  //
  // Writes a Bayesian network in the output stream using the BN format.
  //
  // @param output The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE void FactorisedValuesCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
     std::ostream&                  output,
     const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.")

    std::stringstream strfile, strfile2;

    Idx num      = 0;
    Idx numparam = 0;

    for (auto node: bn.nodes())
      numparam += bn.variable(node).domainSize();

    Idx                                clause = 0;
    std::stringstream                  clausstr;
    gum::HashTable< std::string, Idx > vartable;   // key name::label val num;
    gum::HashTable< std::string, Idx > protable;

    for (auto node: bn.nodes()) {
      for (Idx i = 0; i < bn.variable(node).domainSize(); i++) {
        std::stringstream str;
        str << bn.variable(node).name() << "_" << bn.variable(node).label(i);
        vartable.insert(str.str(), ++num);
        strfile << num << "::" << str.str() << "\n";
      }

      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);

      Instantiation inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        std::stringstream strinst;
        strinst << inst.toString();
        strinst << "_val=" << this->fromExact(cpt[inst]);

        if (!protable.exists(strinst.str())) {
          protable.insert(inst.toString(), ++numparam);
          strfile2 << numparam << "::" << strinst.str() << "\n";
        }
      }
    }

    for (auto node: bn.nodes()) {
      std::stringstream str0, str2;

      for (Idx i = 0; i < bn.variable(node).domainSize(); i++) {
        std::stringstream stri;   //= bn.variable(iter).name()+"_"+
        // bn.variable(iter).label( i ) ;
        stri << bn.variable(node).name() << "_" << bn.variable(node).label(i);
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n";
      clause++;
      clausstr << str0.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        if (this->fromExact(cpt[inst]) != 1.0) {
          for (Idx i = 0; i < inst.nbrDim(); i++) {
            std::stringstream str;
            str << inst.variable(i).name() << "_" << inst.val(inst.variable(i));
            str2 << "-" << vartable[str.str()] << " ";
          }

          if (this->fromExact(cpt[inst])) {
            std::stringstream strinst;
            strinst << bn.variable(node).name();
            strinst << "_val=" << this->fromExact(cpt[inst]);
            str2 << protable[strinst.str()];
          }

          str2 << " 0\n";
          clause++;
        }
      }

      clausstr << str2.str();
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
  INLINE void FactorisedValuesCNFWriter< GUM_SCALAR, IApproximationPolicy >::_doWrite(
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
        if (this->fromExact(cpt[inst]) && this->fromExact(cpt[inst]) != 1.0) {
          std::stringstream strinst;
          strinst << var.name();
          strinst << "_val=" << this->fromExact(cpt[inst]);

          if (!protable.exists(strinst.str())) {
            protable.insert(strinst.str(), ++numparam);
            strfile2 << numparam << "::" << strinst.str() << "\n";
          }
        }
      }
    }

    for (auto node: bn.nodes()) {
      std::stringstream str0, str2;

      for (Idx i = 0; i < bn.variable(node).domainSize(); i++) {
        std::stringstream stri;   //= bn.variable(iter).name()+"_"+
        // bn.variable(iter).label( i ) ;
        stri << bn.variable(node).name() << "_" << bn.variable(node).label(i);
        str0 << vartable[stri.str()] << " ";
      }

      str0 << "0\n";
      clause++;
      clausstr << str0.str();
      const Potential< GUM_SCALAR >& cpt = bn.cpt(node);
      Instantiation                  inst(cpt);

      for (inst.setFirst(); !inst.end(); ++inst) {
        if (this->fromExact(cpt[inst]) != 1.0) {
          for (Idx i = 0; i < inst.nbrDim(); i++) {
            std::stringstream str;
            str << inst.variable(i).name() << "_" << inst.val(inst.variable(i));
            str2 << "-" << vartable[str.str()] << " ";
          }

          if (this->fromExact(cpt[inst])) {
            std::stringstream strinst;
            strinst << bn.variable(node).name();
            strinst << "_val=" << this->fromExact(cpt[inst]);
            str2 << protable[strinst.str()];
          }

          str2 << " 0\n";
          clause++;
        }
      }

      clausstr << str2.str();
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
  /*  template<typename GUM_SCALAR> INLINE
      std::string
      OCNFWriter<GUM_SCALAR>:: _variableCPT_( const Potential<GUM_SCALAR>& cpt )
    {
        std::stringstream str;
        str << "";
        return str.str();
      }

    // Returns the header of the BN file.
    template<typename GUM_SCALAR> INLINE
      std::string
      OCNFWriter<GUM_SCALAR>:: _header_( const IBayesNet<GUM_SCALAR>& ) {
        std::stringstream str;
        str << "";
        return str.str();
      }

    // Returns a bloc defining a variable in the BN format.
    template<typename GUM_SCALAR> INLINE
      std::string
      OCNFWriter<GUM_SCALAR>:: _variableBloc_( const DiscreteVariable& var ) {
        std::stringstream str;
        str << "" ;
        return str.str();
      }*/

  // Returns the modalities labels of the variables in varsSeq

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
