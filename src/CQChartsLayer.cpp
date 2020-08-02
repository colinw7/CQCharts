#include <CQChartsLayer.h>

namespace {

using LayerTypeName = std::map<CQChartsLayer::Type, QString>;
using NameLayerType = std::map<QString, CQChartsLayer::Type>;

static LayerTypeName s_LayerTypeName;
static NameLayerType s_NameLayerType;

void initLayerTypeName() {
  auto addLayerTypeName = [](CQChartsLayer::Type type, const QString &name) {
    s_LayerTypeName[type] = name;
    s_NameLayerType[name] = type;
  };

  addLayerTypeName(CQChartsLayer::Type::NONE       , "none");
  addLayerTypeName(CQChartsLayer::Type::BACKGROUND , "background");
  addLayerTypeName(CQChartsLayer::Type::BG_AXES    , "bg_axes");
  addLayerTypeName(CQChartsLayer::Type::BG_KEY     , "bg_key");
  addLayerTypeName(CQChartsLayer::Type::BG_PLOT    , "bg_plot");
  addLayerTypeName(CQChartsLayer::Type::MID_PLOT   , "mid_plot");
  addLayerTypeName(CQChartsLayer::Type::FG_PLOT    , "fg_plot");
  addLayerTypeName(CQChartsLayer::Type::FG_AXES    , "fg_axes");
  addLayerTypeName(CQChartsLayer::Type::FG_KEY     , "fg_key");
  addLayerTypeName(CQChartsLayer::Type::TITLE      , "title");
  addLayerTypeName(CQChartsLayer::Type::ANNOTATION , "annotation");
  addLayerTypeName(CQChartsLayer::Type::FOREGROUND , "foreground");
  addLayerTypeName(CQChartsLayer::Type::EDIT_HANDLE, "edit_handle");
  addLayerTypeName(CQChartsLayer::Type::BOXES      , "boxes");
  addLayerTypeName(CQChartsLayer::Type::SELECTION  , "selection");
  addLayerTypeName(CQChartsLayer::Type::MOUSE_OVER , "mouse_over");
}

}

QStringList
CQChartsLayer::
typeNames()
{
  initLayerTypeName();

  QStringList names;

  for (const auto &pn : s_LayerTypeName)
    names.push_back(pn.second);

  return names;
}

QString
CQChartsLayer::
typeName(const Type &type)
{
  initLayerTypeName();

  auto p = s_LayerTypeName.find(type);
  if (p == s_LayerTypeName.end()) return "none";

  return (*p).second;
}

CQChartsLayer::Type
CQChartsLayer::
nameType(const QString &name)
{
  initLayerTypeName();

  auto p = s_NameLayerType.find(name);
  if (p == s_NameLayerType.end()) return Type::NONE;

  return (*p).second;
}

//---

CQChartsLayer::
CQChartsLayer(const Type &type, const CQChartsBuffer::Type &buffer) :
 type_(type), buffer_(buffer)
{
}

CQChartsLayer::
~CQChartsLayer()
{
}
