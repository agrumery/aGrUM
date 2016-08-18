/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <unistd.h>
#define GetCurrentDir getcwd

#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

namespace po = boost::program_options;
namespace o3prmr = gum::prm::o3prmr;

enum class OutputType { TXT, CSV };

/**
 * @return Returns the options accepted by prm_run.
 */
auto options() {
  int opt;
  po::options_description desc( "usage: prm_run [options] file1 file2 [...]" );
  desc.add_options()( "help,h", "prints this help message" );
  desc.add_options()( "syntax,s", "no inference, only syntax check" );
  desc.add_options()( "classpath,p",
                      po::value<std::vector<std::string>>(),
                      "a folder where to find O3PRM files" );
  desc.add_options()( "output,o", po::value<std::string>(), "output file" );
  desc.add_options()( "output-type,t",
                      po::value<std::string>(),
                      "output file type (txt or csv)" );
  desc.add_options()(
      "input-file", po::value<std::vector<std::string>>(), "input files" );
  return std::move( desc );
}

/**
 * @return Returns the positional options accepted by prm_run.
 */
auto positional_options() {
  auto p = po::positional_options_description{};
  p.add( "input-file", -1 );
  return std::move( p );
}

/**
 * @brief Print inference results in stream in the CSV format.
 * @param engine The O3PRMR interpreter.
 * @param output The output stream.
 */
void print_results_csv( o3prmr::O3prmrInterpreter& engine,
                        std::ostream& output ) {
  for ( auto result : engine.results() ) {
    output << result.command;
    for ( auto val : result.values ) {
      output << ", " << val.label << ", " << val.p;
    }
    output << std::endl;
  }
}

/**
 * @brief Print inference results in stream in a textual format.
 * @param engine The O3PRMR interpreter.
 * @param output The output stream.
 */
void print_results_txt( o3prmr::O3prmrInterpreter& engine,
                        std::ostream& output ) {
  for ( auto result : engine.results() ) {
    output << result.command << ": " << std::endl;
    for ( auto val : result.values ) {
      output << "\t" << val.label << ": " << val.p << std::endl;
    }
  }
}

/**
 * @brief Prints inference results in the corresponding output_type format.
 * @param engine The O3PRMR interpreter.
 * @param output The output stream.
 * @param output_type The output format, @see OutputType.
 */
void print_results( o3prmr::O3prmrInterpreter& engine,
                    std::ostream& output,
                    OutputType output_type ) {

  switch ( output_type ) {
    case OutputType::CSV: {
      print_results_csv( engine, output );
      break;
    }
    default: {
      print_results_txt( engine, output );
      break;
    }
  }
}

/**
 * @brief Read input from a file.
 * @param engine The O3PRMR interpreter.
 * @param output The input file.
 * @param vm The options variable map.
 */
void read_file( o3prmr::O3prmrInterpreter& engine,
                const std::string& input,
                po::variables_map& vm ) {
  auto output_type = OutputType::TXT;

  if ( vm.count( "output-type" ) ) {
    if ( vm["output-type"].as<std::string>() == "csv" ) {
      output_type = OutputType::CSV;
    }
  }

  engine.interpretFile( input );
  engine.showElegantErrorsAndWarnings( std::cerr );

  if ( vm.count( "output" ) ) {
    auto filename = vm["output"].as<std::string>();
    auto output = std::ofstream{filename};
    if ( output.good() ) {
      print_results( engine, std::cout, output_type );
    } else {
      std::cerr << "could not open file " << filename << std::endl;
      print_results( engine, std::cout, output_type );
    }
  } else {
    print_results( engine, std::cout, output_type );
  }
}

/**
 * @brief The prm_run main function.
 *
 * If position options are present, this will try to open then and parse them
 * using the O3PRM interpreter.
 */
int main( int argc, char* argv[] ) {
  try {

    auto desc = options();
    auto pos = positional_options();
    auto vm = po::variables_map{};

    po::store( po::command_line_parser( argc, argv )
                   .options( desc )
                   .positional( pos )
                   .run(),
               vm );
    po::notify( vm );

    if ( vm.count( "help" ) ) {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
    }

    auto engine = o3prmr::O3prmrInterpreter{};

    char* buffer;
    if ( (buffer = getcwd( NULL, 0 ) ) != NULL ) {
      auto path = std::string( buffer );
      engine.addPath( path );
    }

    if ( vm.count( "classpath" ) ) {
      for ( auto p : vm["classpath"].as<std::vector<std::string>>() ) {
        engine.addPath( p );
      }
    }

    if ( vm.count( "syntax" ) ) {
      engine.setSyntaxMode( true );
    }

    if ( vm.count( "input-file" ) ) {
      for ( auto input : vm["input-file"].as<std::vector<std::string>>() ) {
        read_file( engine, input, vm );
      }
    }

    return EXIT_SUCCESS;
  } catch ( gum::Exception& e ) {
    std::cerr << "an unknown errore occured" << std::endl;
    return EXIT_FAILURE;
  }
}

