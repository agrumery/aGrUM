/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C

      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
   know about them.  */
enum yytokentype {
  TOK_DATABASE_TAG_BEGIN = 258,
  TOK_DATABASE_TAG_END = 259,
  TOK_VARIABLE_TAG_BEGIN = 260,
  TOK_VARIABLE_TAG_END = 261,
  TOK_END_VARIABLE_TAG = 262,
  TOK_MODALITY_TAG_BEGIN = 263,
  TOK_MODALITY_TAG_END = 264,
  TOK_ID_TAG = 265,
  TOK_DATA_TAG = 266,
  TOK_END_DATA_TAG = 267,
  TOK_CASE_TAG = 268,
  TOK_END_CASE_TAG = 269,
  TOK_NAME_TAG = 270,
  TOK_STRING = 271,
  TOK_EQUAL = 272,
  TOK_ERROR = 273,
  TOK_INTEGER = 274,
  TOK_MISSING = 275
};
#endif
/* Tokens.  */
#define TOK_DATABASE_TAG_BEGIN 258
#define TOK_DATABASE_TAG_END 259
#define TOK_VARIABLE_TAG_BEGIN 260
#define TOK_VARIABLE_TAG_END 261
#define TOK_END_VARIABLE_TAG 262
#define TOK_MODALITY_TAG_BEGIN 263
#define TOK_MODALITY_TAG_END 264
#define TOK_ID_TAG 265
#define TOK_DATA_TAG 266
#define TOK_END_DATA_TAG 267
#define TOK_CASE_TAG 268
#define TOK_END_CASE_TAG 269
#define TOK_NAME_TAG 270
#define TOK_STRING 271
#define TOK_EQUAL 272
#define TOK_ERROR 273
#define TOK_INTEGER 274
#define TOK_MISSING 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE {

  /* Line 2068 of yacc.c  */
#line 19 "/home/phw/Documents/gits/lip6/aGrUM-dev/src/agrum/learning/XmlCsvParser/parseXmlCsv.ypp"

  char  Char;
  int   Int;



  /* Line 2068 of yacc.c  */
#line 97 "parseXmlCsv.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




