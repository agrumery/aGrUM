
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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

#  include <agrum/MN/io/UAI/UAIMNWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE UAIMNWriter< GUM_SCALAR >::UAIMNWriter() {
    GUM_CONSTRUCTOR(UAIMNWriter);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE UAIMNWriter< GUM_SCALAR >::~UAIMNWriter() {
    GUM_DESTRUCTOR(UAIMNWriter);
  }

  /*
   * Writes a Markov net in the given ouput stream.
   *
   * @param output The output stream.
   * @param MN The Markov net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void
     UAIMNWriter< GUM_SCALAR >::write(std::ostream&                   output,
                                      const IMarkovNet< GUM_SCALAR >& MN) {
    if (!output.good()) {
      GUM_ERROR(IOError, "Stream states flags are not all unset.");
    }

    output << __preambule(MN) << std::endl;

    for (const auto& kv: MN.factors())
      output << __factorBloc(MN, *kv.second) << std::endl;

    output << std::endl;

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed."); }
  }

  /*
   * Writes a Markov net in the file referenced by filePath.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the Markov net.
   * @param MN The Markov net writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void
     UAIMNWriter< GUM_SCALAR >::write(const std::string&              filePath,
                                      const IMarkovNet< GUM_SCALAR >& MN) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    write(output, MN);

    output.close();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed."); }
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         UAIMNWriter< GUM_SCALAR >::__preambule(const IMarkovNet< GUM_SCALAR >& MN) {
    std::stringstream str;

    str << "MARKOV" << std::endl;

    str << MN.size() << " # nbr Nodes" <<std::endl;

    for (auto node: MN.nodes())
      str << MN.variable(node).domainSize() << " ";
    str << std::endl;

    str << MN.factors().size() << " # nbr Factors "<<std::endl;   // number of cliques

    for (const auto& kv: MN.factors()) {
      const auto& nodeset = kv.first;
      str << nodeset.size() << " ";
      for (auto k: nodeset) {
        str << k << " ";
      }
      str << std::endl;
    }

    return str.str();
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIMNWriter< GUM_SCALAR >::__factorBloc(
     const IMarkovNet< GUM_SCALAR >& MN, const Potential< GUM_SCALAR >& clikpot) {
    std::stringstream str;

    str << clikpot.domainSize() << " # {";

    for (Idx k = 0; k < clikpot.nbrDim(); k++) {
      str << MN.idFromName(clikpot.variable(k).name());
      if (k == clikpot.nbrDim() - 1)
        str << "}";
      else
        str << ", ";
    }
    Instantiation I(clikpot);
    for (I.setFirst(); !I.end(); ++I) {
      if (I.val(0) == 0) str << std::endl << "  ";
      str << clikpot[I] << " ";
    }
    str << std::endl;

    return str.str();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS