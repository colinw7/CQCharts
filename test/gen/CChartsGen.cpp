#include <CArgv.h>
#include <CMathRand.h>
#include <vector>
#include <map>
#include <set>
#include <cassert>

int
main(int argc, char **argv)
{
  int  n      { 10 };
  int  seed   { -1 };
  bool fromTo { false };
  bool size   { false };
  bool header { false };
  bool group  { false };

  CArgv::visit(argc, argv,
   [&](const std::string &opt, CArgv::State &state) { // opt
     if      (opt == "n") {
       if (state.hasNext()) {
         n = std::stoi(state.next());
       }
     }
     else if (opt == "s") {
       if (state.hasNext()) {
         seed = std::stoi(state.next());
       }
     }
     else if (opt == "from_to") {
       fromTo = true;
     }
     else if (opt == "size") {
       size = true;
     }
     else if (opt == "header") {
       header = true;
     }
     else if (opt == "group") {
       group = true;
     }
     else {
       state.unhandled();
     }
   },
   [&](const std::string &, CArgv::State &state) { // arg
     state.unhandled();
   });

  if (seed < 0)
    CMathRand::timeSeedRand();
  else
    CMathRand::seedRand(seed);

  int ng = std::max(n/5, 2);

  if (fromTo) {
    std::cout << "From,To,Value";

    if (group)
      std::cout << ",Group";

    if (size)
      std::cout << ",Attributes";

    std::cout << "\n";

    //---

    std::vector<std::string> names;

    for (int i = 0; i < n; ++i)
      names.push_back(CMathRand::randString());

    //---

    // init node sizes
    using NameSize   = std::map<std::string, int>;
    using SizeOutput = std::set<std::string>;

    NameSize   nameSize;
    SizeOutput sizeOutput;

    if (size) {
      for (const auto &name : names) {
        int s = CMathRand::randInRange(1, 100);

        nameSize[name] = s;
      }
    }

    //---

    // init node groups
    using NameGroup   = std::map<std::string, int>;
  //using GroupOutput = std::set<std::string>;

    NameGroup   nameGroup;
  //GroupOutput groupOutput;

    if (group) {
      for (const auto &name : names) {
        int ig = CMathRand::randInRange(1, ng);

        nameGroup[name] = ig;
      }
    }

    //---

    while (names.size() > 1) {
      // get src name
      auto name = names.back(); names.pop_back();

      // get random number of connected names
      int nn = CMathRand::randInRange(0, int(names.size()));

      for (int i = 0; i < nn; ++i) {
        auto name1 = names[i];

        // get number of connections
        auto nc = CMathRand::randInRange(1, 100);

        std::cout << name << "," << name1 << "," << nc;

        if (group) {
          auto p1 = nameGroup.find(name);
          assert(p1 != nameGroup.end());

          std::cout << "," << (*p1).second;
        }

        if (size) {
          auto p1 = sizeOutput.find(name);
          auto p2 = sizeOutput.find(name1);

          if (p1 == sizeOutput.end() || p2 == sizeOutput.end()) {
            std::cout << ",\"";

            bool added = false;

            if (p1 == sizeOutput.end()) {
              std::cout << "src_value=" << nameSize[name];

              sizeOutput.insert(name);

              added = true;
            }

            if (p2 == sizeOutput.end()) {
              if (added)
                std::cout << ",";

              std::cout << "dest_value=" << nameSize[name1];

              sizeOutput.insert(name1);
            }

            std::cout << "\"";
          }
        }

        std::cout << "\n";
      }
    }

    if (size) {
      for (const auto &name : names) {
        auto p = sizeOutput.find(name);

        if (p == sizeOutput.end()) {
          std::cout << name << ",,";

          if (group)
            std::cout << ",";

          std::cout << name << ",value=" << nameSize[name] << "\n";

          sizeOutput.insert(name);
        }
      }
    }
  }
  else {
    std::cout << "Name,Value\n";

    for (int i = 0; i < n; ++i) {
      auto name  = CMathRand::randString();
      auto value = CMathRand::randInRange(1, 100);

      std::cout << name << ",\"" << value << "\"\n";
    }
  }

  return 0;
}
