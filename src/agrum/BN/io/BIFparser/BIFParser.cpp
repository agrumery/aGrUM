
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   gum_biflex

/* First part of user declarations.  */

/* Line 311 of lalr1.cc  */
#line 20 "BIFParser.ypp"
 /*** C/C++ Declarations ***/
#include <string>
#include <vector>
#include <sstream>

#include <agrum/core/exceptions.h>


/* Line 311 of lalr1.cc  */
#line 52 "BIFParser.cpp"


#include "BIFParser.hpp"

/* User implementation prologue.  */

/* Line 317 of lalr1.cc  */
#line 117 "BIFParser.ypp"

#include <agrum/BN/io/BIFparser/BIFDriver.h>
#include <agrum/BN/io/BIFparser/BIFScanner.h>

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer().lex

// My variable
std::vector<std::string> __variables;



/* Line 317 of lalr1.cc  */
#line 77 "BIFParser.cpp"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 380 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace gum_bif {

/* Line 380 of lalr1.cc  */
#line 146 "BIFParser.cpp"
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  BIFParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  BIFParser::BIFParser (class BIFDriver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {
  }

  BIFParser::~BIFParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  BIFParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  BIFParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  BIFParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 25: /* "TOK_WORD" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->StringVal); };

/* Line 480 of lalr1.cc  */
#line 253 "BIFParser.cpp"
	break;
      case 51: /* "tableValues" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->DVectorVal); };

/* Line 480 of lalr1.cc  */
#line 262 "BIFParser.cpp"
	break;
      case 55: /* "parents" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->SVectorVal); };

/* Line 480 of lalr1.cc  */
#line 271 "BIFParser.cpp"
	break;
      case 56: /* "modalityValues" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->DVectorVal); };

/* Line 480 of lalr1.cc  */
#line 280 "BIFParser.cpp"
	break;
      case 57: /* "word" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->StringVal); };

/* Line 480 of lalr1.cc  */
#line 289 "BIFParser.cpp"
	break;
      case 58: /* "words" */

/* Line 480 of lalr1.cc  */
#line 115 "BIFParser.ypp"
	{ delete (yyvaluep->StringVal); };

/* Line 480 of lalr1.cc  */
#line 298 "BIFParser.cpp"
	break;

	default:
	  break;
      }
  }

  void
  BIFParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BIFParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BIFParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BIFParser::debug_level_type
  BIFParser::debug_level () const
  {
    return yydebug_;
  }

  void
  BIFParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  BIFParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
/* Line 553 of lalr1.cc  */
#line 55 "BIFParser.ypp"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &(const_cast<std::string&>(driver.streamName()));
}

/* Line 553 of lalr1.cc  */
#line 384 "BIFParser.cpp"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 4:

/* Line 678 of lalr1.cc  */
#line 140 "BIFParser.ypp"
    { driver.factory().startNetworkDeclaration(); }
    break;

  case 5:

/* Line 678 of lalr1.cc  */
#line 142 "BIFParser.ypp"
    { driver.factory().addNetworkProperty("name", *(yysemantic_stack_[(5) - (3)].StringVal));
    driver.factory().endNetworkDeclaration(); }
    break;

  case 9:

/* Line 678 of lalr1.cc  */
#line 153 "BIFParser.ypp"
    { driver.factory().startVariableDeclaration(); }
    break;

  case 10:

/* Line 678 of lalr1.cc  */
#line 155 "BIFParser.ypp"
    { driver.factory().endVariableDeclaration(); }
    break;

  case 11:

/* Line 678 of lalr1.cc  */
#line 160 "BIFParser.ypp"
    { try { driver.factory().variableName(*(yysemantic_stack_[(2) - (2)].StringVal)); }
    catch (gum::DuplicateElement& e) { driver.error((yyloc), e.getContent()); } }
    break;

  case 15:

/* Line 678 of lalr1.cc  */
#line 181 "BIFParser.ypp"
    { try { driver.factory().addModality(*(yysemantic_stack_[(3) - (1)].StringVal)); }
      catch (gum::DuplicateElement& e) { driver.error((yyloc), e.getContent()); }
      try { driver.factory().addModality(*(yysemantic_stack_[(3) - (3)].StringVal)); }
      catch (gum::DuplicateElement& e) { driver.error((yyloc), e.getContent()); }
    }
    break;

  case 16:

/* Line 678 of lalr1.cc  */
#line 187 "BIFParser.ypp"
    { try { driver.factory().addModality(*(yysemantic_stack_[(3) - (3)].StringVal)); }
      catch (gum::DuplicateElement& e) { driver.error((yyloc), e.getContent()); }
    }
    break;

  case 17:

/* Line 678 of lalr1.cc  */
#line 193 "BIFParser.ypp"
    { __variables.clear(); }
    break;

  case 19:

/* Line 678 of lalr1.cc  */
#line 199 "BIFParser.ypp"
    { __variables.push_back(*(yysemantic_stack_[(3) - (2)].StringVal));
      try { driver.factory().startParentsDeclaration(__variables[0]);}
      catch (gum::NotFound& e) { driver.error((yyloc), e.getContent()); } }
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 203 "BIFParser.ypp"
    { __variables.push_back(*(yysemantic_stack_[(2) - (2)].StringVal));
      try { driver.factory().startParentsDeclaration(__variables[0]);}
      catch (gum::NotFound& e) { driver.error((yyloc), e.getContent()); } }
    break;

  case 22:

/* Line 678 of lalr1.cc  */
#line 211 "BIFParser.ypp"
    { __variables.push_back(*(yysemantic_stack_[(1) - (1)].StringVal));
      try { driver.factory().addParent(*(yysemantic_stack_[(1) - (1)].StringVal)); }
      catch (gum::NotFound& e) { driver.error((yyloc), e.getContent()); } }
    break;

  case 23:

/* Line 678 of lalr1.cc  */
#line 215 "BIFParser.ypp"
    { __variables.push_back(*(yysemantic_stack_[(2) - (2)].StringVal));
      try { driver.factory().addParent(*(yysemantic_stack_[(2) - (2)].StringVal)); }
      catch (gum::NotFound& e) { driver.error((yyloc), e.getContent()); } }
    break;

  case 25:

/* Line 678 of lalr1.cc  */
#line 222 "BIFParser.ypp"
    { driver.factory().startFactorizedProbabilityDeclaration(__variables[0]); }
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 224 "BIFParser.ypp"
    { driver.factory().endFactorizedProbabilityDeclaration(); }
    break;

  case 27:

/* Line 678 of lalr1.cc  */
#line 228 "BIFParser.ypp"
    { driver.factory().startRawProbabilityDeclaration(__variables[0]); }
    break;

  case 28:

/* Line 678 of lalr1.cc  */
#line 230 "BIFParser.ypp"
    { driver.factory().rawConditionalTable(*(yysemantic_stack_[(3) - (3)].DVectorVal));
    driver.factory().endRawProbabilityDeclaration(); }
    break;

  case 29:

/* Line 678 of lalr1.cc  */
#line 235 "BIFParser.ypp"
    { (yyval.DVectorVal) = new std::vector<double>(); (yyval.DVectorVal)->push_back((yysemantic_stack_[(1) - (1)].Float)); }
    break;

  case 30:

/* Line 678 of lalr1.cc  */
#line 237 "BIFParser.ypp"
    { (yyval.DVectorVal) = new std::vector<double>(*(yysemantic_stack_[(2) - (1)].DVectorVal)); (yyval.DVectorVal)->push_back((yysemantic_stack_[(2) - (2)].Float)); }
    break;

  case 33:

/* Line 678 of lalr1.cc  */
#line 247 "BIFParser.ypp"
    { driver.factory().setVariableValues(*(yysemantic_stack_[(2) - (2)].DVectorVal)); }
    break;

  case 34:

/* Line 678 of lalr1.cc  */
#line 249 "BIFParser.ypp"
    { driver.factory().setVariableValues(*(yysemantic_stack_[(2) - (2)].DVectorVal)); }
    break;

  case 35:

/* Line 678 of lalr1.cc  */
#line 254 "BIFParser.ypp"
    { for (size_t i = 0; i < (yysemantic_stack_[(3) - (2)].SVectorVal)->size(); ++i) {
      driver.factory().setParentModality(__variables[i+1], (yysemantic_stack_[(3) - (2)].SVectorVal)->at(i));
    }
  }
    break;

  case 36:

/* Line 678 of lalr1.cc  */
#line 261 "BIFParser.ypp"
    { (yyval.SVectorVal) = new std::vector<std::string>(); (yyval.SVectorVal)->push_back(*(yysemantic_stack_[(1) - (1)].StringVal)); }
    break;

  case 37:

/* Line 678 of lalr1.cc  */
#line 263 "BIFParser.ypp"
    { (yyval.SVectorVal) = new std::vector<std::string>(*(yysemantic_stack_[(3) - (1)].SVectorVal)); (yyval.SVectorVal)->push_back(*(yysemantic_stack_[(3) - (3)].StringVal)); }
    break;

  case 38:

/* Line 678 of lalr1.cc  */
#line 267 "BIFParser.ypp"
    { (yyval.DVectorVal) = new std::vector<double>(); (yyval.DVectorVal)->push_back((yysemantic_stack_[(1) - (1)].Float)); }
    break;

  case 39:

/* Line 678 of lalr1.cc  */
#line 268 "BIFParser.ypp"
    { (yyval.DVectorVal) = new std::vector<double>(*(yysemantic_stack_[(2) - (1)].DVectorVal)); (yyval.DVectorVal)->push_back((yysemantic_stack_[(2) - (2)].Float)); }
    break;

  case 40:

/* Line 678 of lalr1.cc  */
#line 272 "BIFParser.ypp"
    { (yyval.StringVal) = new std::string(*(yysemantic_stack_[(1) - (1)].StringVal)); }
    break;

  case 41:

/* Line 678 of lalr1.cc  */
#line 273 "BIFParser.ypp"
    { std::stringstream s; s << (yysemantic_stack_[(1) - (1)].Int); (yyval.StringVal) = new std::string(s.str()); }
    break;

  case 42:

/* Line 678 of lalr1.cc  */
#line 274 "BIFParser.ypp"
    { std::stringstream s; s << (yysemantic_stack_[(1) - (1)].Float); (yyval.StringVal) = new std::string(s.str()); }
    break;

  case 43:

/* Line 678 of lalr1.cc  */
#line 275 "BIFParser.ypp"
    { (yyval.StringVal) = new std::string(*(yysemantic_stack_[(3) - (2)].StringVal)); }
    break;

  case 44:

/* Line 678 of lalr1.cc  */
#line 276 "BIFParser.ypp"
    { (yyval.StringVal) = new std::string(*(yysemantic_stack_[(3) - (2)].StringVal)); }
    break;

  case 45:

/* Line 678 of lalr1.cc  */
#line 280 "BIFParser.ypp"
    { (yyval.StringVal) = new std::string(*(yysemantic_stack_[(1) - (1)].StringVal)); }
    break;

  case 46:

/* Line 678 of lalr1.cc  */
#line 281 "BIFParser.ypp"
    { (yyval.StringVal) = new std::string((*(yysemantic_stack_[(2) - (1)].StringVal)) + (*(yysemantic_stack_[(2) - (2)].StringVal))); }
    break;

  case 47:

/* Line 678 of lalr1.cc  */
#line 285 "BIFParser.ypp"
    { (yyval.Float) = (double) (yysemantic_stack_[(1) - (1)].Int); }
    break;

  case 48:

/* Line 678 of lalr1.cc  */
#line 286 "BIFParser.ypp"
    { (yyval.Float) = (double) (yysemantic_stack_[(1) - (1)].Float); }
    break;



/* Line 678 of lalr1.cc  */
#line 761 "BIFParser.cpp"
	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  BIFParser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char BIFParser::yypact_ninf_ = -52;
  const signed char
  BIFParser::yypact_[] =
  {
        -6,     6,   -52,    13,    14,   -52,    14,    -1,   -52,    17,
     -52,    20,    15,    15,   -52,   -52,   -52,    35,    -1,    37,
      34,   -52,   -10,     0,    41,   -52,    28,    -1,    44,   -52,
     -52,   -52,   -52,    29,    45,   -52,    47,    43,    31,    48,
     -52,    -1,   -52,    42,    50,    -5,   -52,    36,    32,     3,
      46,    -1,   -52,    -1,     9,    -5,   -52,     9,     9,    52,
     -52,    -1,    -1,    -8,   -52,    19,   -52,   -52,   -52,     9,
     -52,   -52,     9,     9,   -52,   -52,   -52,   -52,   -52,   -52,
     -52,    -1,   -52,   -52,   -52
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  BIFParser::yydefact_[] =
  {
         6,     0,     6,     0,     3,     1,     2,     0,     7,     0,
       8,     0,     0,     0,    40,    42,    41,     0,     0,     0,
       0,    45,     0,     0,     0,    11,     0,     0,     0,    43,
      46,    44,     5,     0,     0,    12,     0,    20,    25,     0,
      10,     0,    19,     0,     0,     0,    24,     0,     0,     0,
       0,     0,    18,     0,     0,    26,    31,     0,     0,     0,
      13,     0,     0,     0,    22,     0,    36,    48,    47,    33,
      38,    32,    34,    28,    29,    14,    16,    15,    21,    23,
      35,     0,    39,    30,    37
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BIFParser::yypgoto_[] =
  {
       -52,   -52,   -52,   -52,    59,   -52,   -52,   -52,   -52,   -52,
     -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,   -52,
     -52,   -52,   -52,     7,   -52,   -52,     8,   -18,    51,   -51
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  BIFParser::yydefgoto_[] =
  {
        -1,     1,     2,     3,     4,     8,     9,    19,    34,    35,
      36,    49,    10,    11,    28,    43,    63,    44,    45,    46,
      47,    73,    55,    56,    57,    65,    69,    17,    22,    70
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char BIFParser::yytable_ninf_ = -28;
  const signed char
  BIFParser::yytable_[] =
  {
        25,    78,    29,    53,    12,    13,     5,    74,    60,    37,
      -4,    12,    13,    31,    61,    30,    54,    14,    82,    15,
      16,    82,    83,    50,    14,    30,    15,    16,    80,     7,
      81,    -9,   -17,    64,    18,    66,    67,    68,    20,    24,
      21,    26,    27,    76,    77,    79,    32,    33,    38,    39,
      40,    41,    42,   -27,    48,    52,    51,    62,    58,    75,
      59,     6,    71,    84,    23,    72
  };

  /* YYCHECK.  */
  const unsigned char
  BIFParser::yycheck_[] =
  {
        18,     9,    12,     8,    12,    13,     0,    58,     5,    27,
      16,    12,    13,    13,    11,    25,    21,    25,    69,    27,
      28,    72,    73,    41,    25,    25,    27,    28,     9,    16,
      11,    17,    18,    51,    17,    53,    27,    28,    18,     4,
      25,     4,     8,    61,    62,    63,     5,    19,     4,    20,
       5,     4,     9,    22,     6,     5,    14,    11,    22,     7,
      28,     2,    55,    81,    13,    57
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BIFParser::yystos_[] =
  {
         0,    31,    32,    33,    34,     0,    34,    16,    35,    36,
      42,    43,    12,    13,    25,    27,    28,    57,    17,    37,
      18,    25,    58,    58,     4,    57,     4,     8,    44,    12,
      25,    13,     5,    19,    38,    39,    40,    57,     4,    20,
       5,     4,     9,    45,    47,    48,    49,    50,     6,    41,
      57,    14,     5,     8,    21,    52,    53,    54,    22,    28,
       5,    11,    11,    46,    57,    55,    57,    27,    28,    56,
      59,    53,    56,    51,    59,     7,    57,    57,     9,    57,
       9,    11,    59,    59,    57
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  BIFParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BIFParser::yyr1_[] =
  {
         0,    30,    31,    31,    33,    32,    34,    34,    34,    36,
      35,    37,    38,    39,    40,    41,    41,    43,    42,    44,
      45,    44,    46,    46,    47,    48,    47,    50,    49,    51,
      51,    52,    52,    53,    53,    54,    55,    55,    56,    56,
      57,    57,    57,    57,    57,    58,    58,    59,    59
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BIFParser::yyr2_[] =
  {
         0,     2,     2,     1,     0,     5,     0,     2,     2,     0,
       5,     2,     1,     4,     5,     3,     3,     0,     6,     3,
       0,     6,     1,     2,     1,     0,     2,     0,     3,     1,
       2,     1,     2,     2,     2,     3,     1,     3,     1,     2,
       1,     1,     1,     3,     3,     1,     2,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const BIFParser::yytname_[] =
  {
    "$end", "error", "$undefined", "END", "TOK_BEG_CURLY", "TOK_END_CURLY",
  "TOK_BEG_BRACKET", "TOK_END_BRACKET", "TOK_BEG_PARENTHESIS",
  "TOK_END_PARENTHESIS", "TOK_SEMICOLON", "TOK_COMMA", "TOK_DOUBLE_QUOTE",
  "TOK_QUOTE", "TOK_BAR", "TOK_EQUAL", "TOK_NETWORK", "TOK_VARIABLE",
  "TOK_PROBABILITY", "TOK_TYPE", "TOK_DISCRETE", "TOK_DEFAULT",
  "TOK_TABLE", "TOK_PROPERTY", "TOK_PROPERTY_VALUE", "TOK_WORD",
  "TOK_MODALITY", "TOK_FLOAT", "TOK_INTEGER", "TOK_VERTICAL_BAR",
  "$accept", "start", "NetworkUnit", "$@1", "DeclarationUnit",
  "VariableUnit", "$@2", "VariableDeclaration", "VariableContent",
  "VariableDiscreteUnit", "VariableDiscrete", "VariableModalities",
  "ProbaUnit", "$@3", "ProbaVariables", "$@4", "ProbaRightVars",
  "ProbaContent", "$@5", "ProbaByTable", "$@6", "tableValues",
  "ProbaByModalities", "ModalityEntry", "ParentsInstantiation", "parents",
  "modalityValues", "word", "words", "number", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BIFParser::rhs_number_type
  BIFParser::yyrhs_[] =
  {
        31,     0,    -1,    32,    34,    -1,    34,    -1,    -1,    33,
      16,    57,     4,     5,    -1,    -1,    34,    35,    -1,    34,
      42,    -1,    -1,    36,    37,     4,    38,     5,    -1,    17,
      57,    -1,    39,    -1,    40,     4,    41,     5,    -1,    19,
      20,     6,    28,     7,    -1,    57,    11,    57,    -1,    41,
      11,    57,    -1,    -1,    43,    18,    44,     4,    47,     5,
      -1,     8,    57,     9,    -1,    -1,     8,    57,    45,    14,
      46,     9,    -1,    57,    -1,    46,    57,    -1,    49,    -1,
      -1,    48,    52,    -1,    -1,    50,    22,    51,    -1,    59,
      -1,    51,    59,    -1,    53,    -1,    52,    53,    -1,    21,
      56,    -1,    54,    56,    -1,     8,    55,     9,    -1,    57,
      -1,    55,    11,    57,    -1,    59,    -1,    56,    59,    -1,
      25,    -1,    28,    -1,    27,    -1,    12,    58,    12,    -1,
      13,    58,    13,    -1,    25,    -1,    58,    25,    -1,    28,
      -1,    27,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  BIFParser::yyprhs_[] =
  {
         0,     0,     3,     6,     8,     9,    15,    16,    19,    22,
      23,    29,    32,    34,    39,    45,    49,    53,    54,    61,
      65,    66,    73,    75,    78,    80,    81,    84,    85,    89,
      91,    94,    96,    99,   102,   105,   109,   111,   115,   117,
     120,   122,   124,   126,   130,   134,   136,   139,   141
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BIFParser::yyrline_[] =
  {
         0,   135,   135,   136,   140,   140,   146,   148,   149,   153,
     153,   159,   166,   171,   176,   180,   186,   193,   193,   198,
     203,   202,   210,   214,   221,   222,   222,   228,   228,   235,
     236,   241,   242,   246,   248,   253,   261,   262,   267,   268,
     272,   273,   274,   275,   276,   280,   281,   285,   286
  };

  // Print the state stack on the debug stream.
  void
  BIFParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BIFParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  BIFParser::token_number_type
  BIFParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BIFParser::yyeof_ = 0;
  const int BIFParser::yylast_ = 65;
  const int BIFParser::yynnts_ = 30;
  const int BIFParser::yyempty_ = -2;
  const int BIFParser::yyfinal_ = 5;
  const int BIFParser::yyterror_ = 1;
  const int BIFParser::yyerrcode_ = 256;
  const int BIFParser::yyntokens_ = 30;

  const unsigned int BIFParser::yyuser_token_number_max_ = 284;
  const BIFParser::token_number_type BIFParser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // gum_bif

/* Line 1054 of lalr1.cc  */
#line 1255 "BIFParser.cpp"


/* Line 1056 of lalr1.cc  */
#line 289 "BIFParser.ypp"
 /*** Additional Code ***/

void gum_bif::BIFParser::error(const BIFParser::location_type& l,
                               const std::string& m)
{
  driver.error(l, m);
}


