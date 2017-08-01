#include <CQChartsAxisBase.h>
//#include <CQPropertyTree.h>

CQChartsAxisBase::
CQChartsAxisBase(CQChartsPlot *plot, Direction direction, double start, double end) :
 plot_(plot), direction_(direction)
{
  start_ = std::min(start, end);
  end_   = std::max(start, end);
}

void
CQChartsAxisBase::
setRange(double start, double end)
{
  start_ = std::min(start, end);
  end_   = std::max(start, end);

  calc();
}

#if 0
void
CQChartsAxisBase::
addProperties(CQPropertyTree *tree, const QString &path)
{
  tree->addProperty(path, this, "visible"       );
  tree->addProperty(path, this, "label"         );
  tree->addProperty(path, this, "labelFont"     );
  tree->addProperty(path, this, "labelColor"    );
  tree->addProperty(path, this, "lineDisplayed" );
  tree->addProperty(path, this, "lineColor"     );
  tree->addProperty(path, this, "labelDisplayed");
  tree->addProperty(path, this, "gridColor"     );
  tree->addProperty(path, this, "gridDisplayed" );
}
#endif
