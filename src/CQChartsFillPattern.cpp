#include <CQChartsFillPattern.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsFillPattern, toString, fromString)

int CQChartsFillPattern::metaTypeId;

void
CQChartsFillPattern::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsFillPattern);

  CQPropertyViewMgrInst->setUserName("CQChartsFillPattern", "fill_pattern");
}

QString
CQChartsFillPattern::
toString() const
{
  return typeToString(type_);
}

bool
CQChartsFillPattern::
fromString(const QString &s)
{
  Type type = stringToType(s);

  if (type == Type::NONE)
    return false;

  type_ = type;

  return true;
}

CQChartsFillPattern::Type
CQChartsFillPattern::
stringToType(const QString &str)
{
  if (str == "SOLID"     ) return Type::SOLID;
  if (str == "HATCH"     ) return Type::HATCH;
  if (str == "DENSE"     ) return Type::DENSE;
  if (str == "HORIZ"     ) return Type::HORIZ;
  if (str == "HORIZONTAL") return Type::HORIZ;
  if (str == "VERT"      ) return Type::VERT;
  if (str == "VERTICAL"  ) return Type::VERT;
  if (str == "FDIAG"     ) return Type::FDIAG;
  if (str == "BDIAG"     ) return Type::BDIAG;

  return Type::NONE;
}

QString
CQChartsFillPattern::
typeToString(const Type &type)
{
  switch (type) {
    case Type::SOLID: return "SOLID";
    case Type::HATCH: return "HATCH";
    case Type::DENSE: return "DENSE";
    case Type::HORIZ: return "HORIZONTAL";
    case Type::VERT : return "VERTICAL";
    case Type::FDIAG: return "FDIAG";
    case Type::BDIAG: return "BDIAG";
    default         : return "NONE";
  }
}

CQChartsFillPattern::Type
CQChartsFillPattern::
styleToType(const Qt::BrushStyle &style)
{
  switch (style) {
    case Qt::SolidPattern : return Type::SOLID;
    case Qt::CrossPattern : return Type::HATCH;
    case Qt::Dense5Pattern: return Type::DENSE;
    case Qt::HorPattern   : return Type::HORIZ;
    case Qt::VerPattern   : return Type::VERT;
    case Qt::FDiagPattern : return Type::FDIAG;
    case Qt::BDiagPattern : return Type::BDIAG;
    default               : return Type::NONE;
  }
}

Qt::BrushStyle
CQChartsFillPattern::
typeToStyle(const Type &type)
{
  switch (type) {
    case Type::SOLID: return Qt::SolidPattern;
    case Type::HATCH: return Qt::CrossPattern;
    case Type::DENSE: return Qt::Dense5Pattern;
    case Type::HORIZ: return Qt::HorPattern;
    case Type::VERT : return Qt::VerPattern;
    case Type::FDIAG: return Qt::FDiagPattern;
    case Type::BDIAG: return Qt::BDiagPattern;
    default         : return Qt::SolidPattern;
  }
}

QStringList
CQChartsFillPattern::
enumNames() const
{
  static QStringList names = QStringList() <<
    "SOLID" << "HATCH" << "DENSE" << "HORIZONTAL" << "VERTICAL" << "FDIAG" << "BDIAG";

  return names;
}
