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
 *
 * @author Ni NI & Lionel TORTI
 */

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

#include "Parser.h"
#include "session.h"

void print_help_and_exit(int retVal) {
  std::stringstream help;
  help << "usage: prm_run [-vh] [-o output] [-c paths] filename" << std::endl;
  help << "  h : prints this message" << std::endl;
  help << "  v : enables verbose mode" << std::endl;
  help << "  s : syntax check mode" << std::endl;
  help << "  o output : prints all messages in output" << std::endl;
  help << "  c paths : defines the class path, where paths is a colon sperated list of paths (don't use quotes)" << std::endl;
  std::cout << help.str();
  std::exit(retVal);
}

void fill_class_path(std::vector<std::string>& class_path, std::string s) {
  size_t idx = s.find_first_of(':');
  if (*(s.rbegin()) == ':') s = s.substr(0, s.length() - 1);
  class_path.push_back(s.substr(0, idx));
  while (idx != std::string::npos) {
    size_t last = idx + 1;
    idx = s.find(':', last);
    class_path.push_back(s.substr(last, idx));
  }
}

void readOptions(Session& session, int argc, char *argv[]) {
  char c;
  while ((c = getopt (argc, argv, "hsc:o:")) != -1)
    switch (c)
    {
      case 'h':
        session.help = true;
      case 's':
        session.syntax = true;
        break;
      case 'c':
        session.cpaths = optarg;
        break;
      case 'o':
        session.output = optarg;
        break;
      case '?':
        if (optopt == 'c' || optopt == 'o')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
              "Unknown option character `\\x%x'.\n",
              optopt);
        std::exit(EXIT_FAILURE);
      default:
        abort ();
    }
}

int main (int argc, char *argv[]) {
  Session session;
  readOptions(session, argc, argv);
  if (argc == 1)
    print_help_and_exit(EXIT_FAILURE);
  std::ifstream input_stream(argv[argc - 1]);
  Scanner* s = 0;
  Parser* p = 0;
  std::ofstream* output = 0;
  if (input_stream.is_open() and input_stream.good()) {
    try {
      s = new Scanner(argv[argc - 1]);
      if (session.output.size()) {
        output = new std::ofstream(session.output.c_str());
        if (output->is_open() and output->good()) {
          p = new Parser(s, *output);
        } else {
          std::cerr << "Could not open output file, dumping log on the standard output." << std::endl;
          p = new Parser(s);
        }
      } else {
        p = new Parser(s);
      }
      p->verbose = session.verbose;
      p->setSyntaxMode(session.syntax);
      p->Parse();
    } catch (gum::Exception& e) {
      std::cerr << "Something went wrong: " << e.getContent();
      session.fail = true;
    } catch (std::string&) { // Syntax error, error message already printed
      session.fail = true;
    }
  } else {
    std::cerr << "Could not open input file." << std::endl;
    session.fail = true;
  }
  if (p) delete p;
  if (s) delete s;
  if (output) delete output;
  if (session.fail)
    std::exit(EXIT_FAILURE);
  else
    std::exit(EXIT_SUCCESS);
}

//  bool option_comment = false;
//  bool option_outputfile = false;
//  std::string output_file;
//  std::vector<std::string> args, class_path;
//  // Checking up arguments
//  for (int idx = 1; idx < argc; ++idx)
//    args.push_back(argv[idx]);
//  if (std::find(args.begin(), args.end(), "-v") != args.end())
//    option_comment = true;
//  std::vector<std::string>::iterator it = std::find(args.begin(), args.end(), "-o");
//  if (it != args.end()) {
//    if ((it + 1) != args.end()) {
//      output_file = *(it + 1);
//      option_outputfile = true;
//    } else {
//      print_help_and_exit(EXIT_FAILURE);
//    }
//  }
//  it = std::find(args.begin(), args.end(), "-c");
//  if (it != args.end()) {
//    if ((it + 1) != args.end()) fill_class_path(class_path, *(it + 1));
//    else                        print_help_and_exit(EXIT_FAILURE);
//  }
//  it == std::find(args.begin(), args.end(), "-h");
//  if (it != args.end()) print_help_and_exit(EXIT_SUCCESS);
//  // Now we can start parsing the request file
//  std::string file = args.back();
//  std::ifstream input_stream(file.c_str());
//  Scanner* s = 0;
//  Parser* p = 0;
//  std::ofstream* output = 0;
//  if (input_stream.is_open() and input_stream.good()) {
//    try {
//      s = new Scanner(file);
//      if (option_outputfile) {
//        output = new std::ofstream(output_file.c_str());
//        if (input_stream.is_open() and input_stream.good()) {
//          p = new Parser(s, *output);
//        } else {
//          std::cerr << "Could not open output file, dumping log on the standard output." << std::endl;
//          p = new Parser(s);
//        }
//      } else {
//        p = new Parser(s);
//      }
//      p->verbose = option_comment;
//      p->Parse();
//    } catch (gum::Exception& e) {
//      std::cerr << "Something went wrong: " << e.getContent();
//      std::exit(EXIT_FAILURE);
//    } catch (std::string&) { // Syntax error, error message already printed
//      std::exit(EXIT_FAILURE);
//    }
//  } else {
//    std::cerr << "Could not open input file." << std::endl;
//    if (p != 0) delete p;
//    if (s != 0) delete s;
//    print_help_and_exit(EXIT_FAILURE);
//  }
//  if (p) delete p;
//  if (s) delete s;
//  if (output) delete output;
//  std::exit(EXIT_SUCCESS);
// }

