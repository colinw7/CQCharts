#include <CArgv.h>
#include <CMathRand.h>
#include <vector>
#include <map>
#include <set>
#include <cassert>

int
main(int argc, char **argv)
{
  int    n        { 10 };
  int    seed     { -1 };
  bool   fromTo   { false };
  int    series   { 0 };
  bool   scatter  { false };
  bool   size     { false };
  bool   header   { false };
  bool   group    { false };
  bool   labels   { false };
  bool   discreet { false };
  bool   stacked  { false };
  bool   values   { false };
  double rmin     { 1 };
  double rmax     { 100 };
  int    imin     { 1 };
  int    imax     { 100 };

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
     else if (opt == "series") {
       if (state.hasNext()) {
         series = std::stoi(state.next());
       }
     }
     else if (opt == "rmin") {
       if (state.hasNext()) {
         rmin = std::stod(state.next());
       }
     }
     else if (opt == "rmax") {
       if (state.hasNext()) {
         rmax = std::stod(state.next());
       }
     }
     else if (opt == "scatter") {
       scatter = true;
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
     else if (opt == "labels") {
       labels = true;
     }
     else if (opt == "discreet") {
       discreet = true;
     }
     else if (opt == "stacked") {
       stacked = true;
     }
     else if (opt == "values") {
       values = true;
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

  // connections from/to
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
        int s = CMathRand::randInRange(imin, imax);

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

      for (size_t i = 0; i < size_t(nn); ++i) {
        auto name1 = names[i];

        // get number of connections
        auto nc = CMathRand::randInRange(rmin, rmax);

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
  // X and 1 or more Y
  else if (series > 0) {
    if (series == 1) {
      std::cout << "X,Y\n";

      for (int i = 0; i < n; ++i) {
        auto y = CMathRand::randInRange(rmin, rmax);

        std::cout << (i + 1) << "," << y << "\n";
      }
    }
    else {
      std::cout << "X";

      for (int is = 0; is < series; ++is)
        std::cout << ",Y" << std::to_string(is + 1);

      if (labels)
        std::cout << ",Label";

      std::cout << "\n";

      std::vector<int> ranges;

      int ds = 100/series;

      for (int i = 0; i < n; ++i) {
        std::cout << (i + 1);

        for (int is = 0; is < series; ++is) {
          auto y = (! stacked ? CMathRand::randInRange(rmin, rmax) :
                                CMathRand::randInRange(is*ds, (is + 1)*ds));

          std::cout << "," << y;
        }

        if (labels)
          std::cout << "," << CMathRand::randString();

        std::cout << "\n";
      }
    }
  }
  // x,y
  else if (scatter) {
    using Labels = std::map<int, std::string>;

    int    nl;
    Labels dlabels;

    if (labels && discreet) {
      nl = std::max(n/10, 3);

      for (int i = 0; i < nl; ++i)
        dlabels[i] = CMathRand::randString();
    }

    std::cout << "X,Y";

    if (labels)
      std::cout << ",Label";

    std::cout << "\n";

    for (int i = 0; i < n; ++i) {
      auto x = CMathRand::randInRange(rmin, rmax);
      auto y = CMathRand::randInRange(rmin, rmax);

      std::cout << x << "," << y;

      if (labels) {
        if (discreet) {
          int id = CMathRand::randInRange(0, nl - 1);

          std::cout << "," << dlabels[id];
        }
        else
          std::cout << "," << CMathRand::randString();
      }

      std::cout << "\n";
    }
  }
  // values (single column)
  else if (values) {
    std::vector<std::string> groups;

    if (group) {
      groups.resize(size_t(ng));

      for (int i = 0; i < ng; ++i)
        groups[size_t(i)] = CMathRand::randString();
    }

    std::cout << "Value";

    if (labels)
      std::cout << ",Label";

    if (group)
      std::cout << ",Group";

    std::cout << "\n";

    for (int i = 0; i < n; ++i) {
      auto v = CMathRand::randInRange(rmin, rmax);

      std::cout << v;

      if (labels)
        std::cout << "," << CMathRand::randString();

      if (group) {
        auto ig = CMathRand::randInRange(0, ng - 1);

        std::cout << "," << groups[size_t(ig)];
      }

      std::cout << "\n";
    }
  }
  // name value
  else {
    std::cout << "Name,Value\n";

    for (int i = 0; i < n; ++i) {
      auto name  = CMathRand::randString();
      auto value = CMathRand::randInRange(rmin, rmax);

      std::cout << name << ",\"" << value << "\"\n";
    }
  }

  return 0;
}
