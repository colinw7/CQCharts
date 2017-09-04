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
  tree->addProperty(path, this, "border"     );
  tree->addProperty(path, this, "borderColor");
  tree->addProperty(path, this, "borderWidth");
}
