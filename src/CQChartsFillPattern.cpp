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
  auto str = typeToString(type_);

  if (type_ == Type::PALETTE)
    str += ":" + palette_;

  if (angle() != 0.0)
    str += QString("@%1").arg(angle());

  return str;
}

bool
CQChartsFillPattern::
fromString(const QString &s)
{
  return setValue(s);
}

bool
CQChartsFillPattern::
setValue(const QString &s)
{
  auto s1 = s;

  QString s2;
  QString s3;

  auto pos = s1.indexOf(":");

  if (pos >= 0) {
    s2 = s1.mid(pos + 1);
    s1 = s1.mid(0, pos);

    auto pos1 = s2.indexOf("@");

    if (pos1 >= 0) {
      s3 = s2.mid(pos1 + 1);
      s2 = s2.mid(0, pos1);
    }
  }
  else {
    auto pos1 = s1.indexOf("@");

    if (pos1 >= 0) {
      s3 = s1.mid(pos1 + 1);
      s1 = s1.mid(0, pos1);
    }
  }

  auto type = stringToType(s1);

  if (type == Type::NONE)
    return false;

  type_ = type;

  if (type_ == Type::PALETTE)
    palette_ = s2;

  if (s3.length()) {
    bool ok;

    angle_ = s3.toDouble(&ok);

    if (! ok)
      return false;
  }

  return true;
}

CQChartsFillPattern::Type
CQChartsFillPattern::
stringToType(const QString &str)
{
  auto ustr = str.toUpper();

  if (ustr == "SOLID"     ) return Type::SOLID;
  if (ustr == "HATCH"     ) return Type::HATCH;
  if (ustr == "DENSE"     ) return Type::DENSE;
  if (ustr == "HORIZ"     ) return Type::HORIZ;
  if (ustr == "HORIZONTAL") return Type::HORIZ;
  if (ustr == "VERT"      ) return Type::VERT;
  if (ustr == "VERTICAL"  ) return Type::VERT;
  if (ustr == "FDIAG"     ) return Type::FDIAG;
  if (ustr == "BDIAG"     ) return Type::BDIAG;
  if (ustr == "LGRADIENT" ) return Type::LGRADIENT;
  if (ustr == "RGRADIENT" ) return Type::RGRADIENT;
  if (ustr == "PALETTE"   ) return Type::PALETTE;

  return Type::NONE;
}

QString
CQChartsFillPattern::
typeToString(const Type &type)
{
  switch (type) {
    case Type::SOLID:     return "SOLID";
    case Type::HATCH:     return "HATCH";
    case Type::DENSE:     return "DENSE";
    case Type::HORIZ:     return "HORIZONTAL";
    case Type::VERT :     return "VERTICAL";
    case Type::FDIAG:     return "FDIAG";
    case Type::BDIAG:     return "BDIAG";
    case Type::LGRADIENT: return "LGRADIENT";
    case Type::RGRADIENT: return "RGRADIENT";
    case Type::PALETTE:   return "PALETTE";
    default:              return "NONE";
  }
}

CQChartsFillPattern::Type
CQChartsFillPattern::
styleToType(const Qt::BrushStyle &style)
{
  switch (style) {
    case Qt::SolidPattern         : return Type::SOLID;
    case Qt::CrossPattern         : return Type::HATCH;
    case Qt::Dense5Pattern        : return Type::DENSE;
    case Qt::HorPattern           : return Type::HORIZ;
    case Qt::VerPattern           : return Type::VERT;
    case Qt::FDiagPattern         : return Type::FDIAG;
    case Qt::BDiagPattern         : return Type::BDIAG;
    case Qt::LinearGradientPattern: return Type::LGRADIENT;
    case Qt::RadialGradientPattern: return Type::RGRADIENT;
    default                       : return Type::NONE;
  }
}

Qt::BrushStyle
CQChartsFillPattern::
typeToStyle(const Type &type)
{
  switch (type) {
    case Type::SOLID:     return Qt::SolidPattern;
    case Type::HATCH:     return Qt::CrossPattern;
    case Type::DENSE:     return Qt::Dense5Pattern;
    case Type::HORIZ:     return Qt::HorPattern;
    case Type::VERT :     return Qt::VerPattern;
    case Type::FDIAG:     return Qt::FDiagPattern;
    case Type::BDIAG:     return Qt::BDiagPattern;
    case Type::LGRADIENT: return Qt::LinearGradientPattern;
    case Type::RGRADIENT: return Qt::RadialGradientPattern;
    case Type::PALETTE:   return Qt::LinearGradientPattern;
    default:              return Qt::NoBrush;
  }
}

QStringList
CQChartsFillPattern::
enumNames() const
{
  static QStringList names = QStringList() <<
    "SOLID" << "HATCH" << "DENSE" << "HORIZONTAL" << "VERTICAL" << "FDIAG" << "BDIAG" <<
    "LGRADIENT" << "RGRADIENT" << "PALETTE";

  return names;
}
