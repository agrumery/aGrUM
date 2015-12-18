#include <sys/stat.h>

#include <iostream>
#include <string>

#include <agrum/core/utils_dir.h>
#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

void execute(std::string path, std::string request, std::ostream& output=std::cout) {
  try {
    gum::prm::o3prmr::O3prmrInterpreter interpreter;
    interpreter.addPath(path);
    interpreter.interpretFile(request);

    if (interpreter.errorsContainer().count()) {
      interpreter.showElegantErrorsAndWarnings(output);
      output << std::endl;
    }

    if (interpreter.results().size())
    {
      output << "Results: ";
      for (size_t i = 0; i < interpreter.results().size(); ++i)
      {
        auto result = interpreter.results()[i];
        output << std::endl << "    " << result.command << ": ";
        for (size_t j = 0; j < result.values.size(); ++j)
        {
          auto value = result.values[j];
          output << "\n" << "        " << value.label << ": " << value.p;
        }
      }
    }
    std::cout << std::endl;
  } catch (gum::Exception &e) {
    output << "Failed: " << e.errorContent() << std::endl;
    output << e.errorCallStack() << std::endl;
  }
}

bool fileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void printHelp() {
  std::cout << "PRM run tool v.2.0" << std::endl;
  std::cout << "(c) 2007-15 aGrUM Team" << std::endl;
  std::cout << std::endl;
  std::cout << "Usage: prm_run project_dir request_file" << std::endl;
}

int main(int argc, char *argv[]) {
  switch (argc) {
    case 3: 
    {
      std::string path(argv[1]);
      std::string file(argv[2]);
      if (gum::Directory::isDir(path) and fileExists(file)) {
        execute(path, file);
        break;
      }
    }
    default:
    {
      printHelp();
      return 0;
    }
    return 0;
  }
}
