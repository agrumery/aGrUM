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
// ============================================================================
#ifndef BIF_SCANNER_H
#define BIF_SCANNER_H
// ============================================================================
#ifndef YY_DECL
#define YY_DECL                                        \
    gum_bif::BIFParser::token_type                         \
    gum_bif::BIFScanner::lex(                              \
        gum_bif::BIFParser::semantic_type* yylval,         \
        gum_bif::BIFParser::location_type* yylloc          \
    )
#endif // YY_DECL
// ============================================================================
#ifndef __FLEX_LEXER_H
#define yyFlexLexer BIFFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif // __FLEX_LEXER_H
// ============================================================================
#include <agrum/BN/io/BIFparser/generated/BIFParser.hpp>
// Workaround since same flag is used for all bison generated parser
#undef PARSER_HEADER_H
// ============================================================================
namespace gum_bif
// ============================================================================
{
// ============================================================================
  /**
   * @class BIFScanner
   * @brief This class is frontend for a flex lexer over skoob files.
   */
  // ==========================================================================
  class BIFScanner: public BIFFlexLexer
  // ==========================================================================
  {
  // ==========================================================================
  public:
  // ==========================================================================
  /// @name Constructor & Destructor.
  // ==========================================================================
      // ======================================================================
      /// @{
      // ======================================================================
      /**
       * Constructor.
       */
      // ======================================================================
      BIFScanner(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0);
      // ======================================================================
      /**
       * Destructor.
       */
      // ======================================================================
      virtual ~BIFScanner();
      // ======================================================================
      /// @}
  // ==========================================================================
  /// @name Flex methods.
  // ==========================================================================
      /// @{
      // ======================================================================
      /**
       * Performs the same role as yylex() does for ordinary flex scanners: it
       * scans the input stream, consuming tokens, until a rule's action return
       * a value.
       */
      // ======================================================================
      virtual BIFParser::token_type lex( BIFParser::semantic_type* yylval,
                                         BIFParser::location_type* yylloc
                                       );
      // ======================================================================
      /**
       * Sets the debugging flag for the scanner.
       */
      // ======================================================================
      void set_debug(bool b);
      // ======================================================================
      /// @}
  // ==========================================================================
  };
// ============================================================================
} // namespace gum_bif
// ============================================================================
#endif // BIF_SCANNER_H
// ============================================================================
