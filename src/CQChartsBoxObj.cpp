#include <CQChartsBoxObj.h>
#include <CQPropertyTree.h>

CQChartsBoxObj::
CQChartsBoxObj()
{
}

void
CQChartsBoxObj::
addProperties(CQPropertyTree *tree, const QString &path)
{
  tree->addProperty(path, this, "margin"     );
  tree->addProperty(path, this, "background" );

  QString borderPath = path + "/border";

  tree->addProperty(borderPath, this, "border"     , "displayed");
  tree->addProperty(borderPath, this, "borderColor", "color"    );
  tree->addProperty(borderPath, this, "borderWidth", "width"    );
}
