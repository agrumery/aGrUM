/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of SkoolReader.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_SKOOL_READER_H
#define GUM_SKOOL_READER_H

// ============================================================================
#include <fstream>
// ============================================================================
#include <agrum/prm/PRMFactory.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER
#undef COCO_PARSER_H__
#undef COCO_SCANNER_H__
#include <agrum/prm/skool/cocoR/Parser.h>
#endif //DOXYGEN_SHOULD_SKIP_THIS


namespace gum {
namespace prm {
namespace skool {

class SkoolReader {
public:

    SkoolReader();

    ~SkoolReader();

    void readFile(const std::string& file);
    void readString(const std::string & string);

    /**
     * @brief This methods defines the list of paths to look for skool files.
     * Use / for path separator ! Even on Windows !
     *
     * @param class_path A semicolon separated list of paths.
     */
    void setClassPath(const std::string& class_path);

    gum::prm::PRM* prm();

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors();
    /// # of errors
    Size warnings();
    
    /// 
    const ErrorsContainer & getErrorsContainer() const;

    /// line of ith error or warning
    unsigned int errLine( unsigned int i );
    /// col of ith error or warning
    unsigned int errCol( unsigned int i );
    /// filename of ith error or warning
    std::wstring errFilename( unsigned int i );
    /// type of ith error or warning
    bool errIsError( unsigned int i );
    /// message of ith error or warning
    std::string errMsg( unsigned int i );

    /// send on std::cerr the list of errors
    void showElegantErrors();

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings();

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts();
    /// @}

private:
    SkoolReader(const SkoolReader& source);

    SkoolReader& operator=(const SkoolReader& source);

    gum::prm::PRMFactory __factory;

    std::vector<std::string> __class_path;

    Parser* __parser;
    bool __parseDone;
    bool __prmTake;
};

} /* namespace skool */
} /* namespace prm */
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/prm/skool/SkoolReader.inl>
#endif // GUM_NO_INLINE

// ============================================================================
#endif /* GUM_SKOOL_READER_H */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
