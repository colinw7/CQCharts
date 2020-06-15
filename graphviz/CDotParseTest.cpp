#include <CDotParse.h>
#include <iostream>

int
main(int argc, char **argv)
{
  std::string filename;
  bool        debug = false;
  bool        print = false;
  bool        csv   = false;

  for (auto i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg(&argv[i][1]);

      if      (arg == "debug")
        debug = true;
      else if (arg == "print")
        print = true;
      else if (arg == "csv")
        csv = true;
      else
        std::cerr << "Unhandled option: " << arg << "\n";
    }
    else
      filename = argv[i];
  }

  if (filename == "") {
    std::cerr << "Missing filename\n";
    exit(1);
  }

  CDotParse parse(filename);

  parse.setDebug(debug);
  parse.setPrint(print);
  parse.setCSV  (csv);

  if (! parse.parse()) {
    std::cerr << "Parse failed\n";
    exit(1);
  }

  exit(0);
}
