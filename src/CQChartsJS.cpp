#include <CQChartsJS.h>

namespace CQChartsJS {

void
writeProcs(std::ostream &os, const CQCharts::Procs &procs)
{
  os << "\n";

  for (const auto &pp : procs) {
    const auto &proc = pp.second;

    os << "function " << proc.name.toStdString() << "(" << proc.args.toStdString() << ") {\n";
    os << "  " << proc.body.toStdString() << "\n";
    os << "}\n";
    os << "\n";
  }
}

}
