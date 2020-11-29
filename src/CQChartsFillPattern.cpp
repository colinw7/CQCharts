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

  if      (type_ == Type::PALETTE)
    str += ":" + palette().toString();
  else if (type_ == Type::IMAGE)
    str += ":" + image().toString();
  else if (altColor().isValid())
    str += ":";

  if (altColor().isValid())
    str += QString(":%1").arg(altColor().toString());

  if (scale() != 1.0)
    str += QString("*%1").arg(scale());

  if (! angle().isZero())
    str += QString("@%1").arg(angle().toString());

  return str;
}

bool
CQChartsFillPattern::
fromString(const QString &s)
{
  return setValue(s);
}

// format:
//   <typeStr>[:<dataStr>][:<altColor>][*<scale>][@<angle>]
bool
CQChartsFillPattern::
setValue(const QString &s)
{
  QString typeStr, dataStr, altColorStr, scaleStr, angleStr;

  auto lhs = s;

  QString rhs;

  auto pos = lhs.indexOf(":");

  // :<dataStr>[:<altColor>][*<scale>][@<angle>]
  if (pos >= 0) {
    rhs     = lhs.mid(pos + 1);
    typeStr = lhs.mid(0, pos);

    auto pos1 = rhs.indexOf(":");

    // :<altColor>[*<scale>][@<angle>]
    if (pos1 >= 0) {
      dataStr = rhs.mid(0, pos1);
      rhs     = rhs.mid(pos1 + 1);

      auto pos2 = rhs.indexOf("*");

      // *<scale>[@<angle>]
      if (pos2 >= 0) {
        altColorStr = rhs.mid(0, pos2);
        rhs         = rhs.mid(pos2 + 1);

        auto pos3 = rhs.indexOf("@");

        // @<angle>
        if (pos3 >= 0) {
          angleStr = rhs.mid(pos3 + 1);
          scaleStr = rhs.mid(0, pos3);
        }
        else
          scaleStr = rhs;
      }
      // [@<angle>]
      else {
        auto pos3 = rhs.indexOf("@");

        // @<angle>
        if (pos3 >= 0) {
          angleStr    = rhs.mid(pos3 + 1);
          altColorStr = rhs.mid(0, pos3);
        }
        else
          altColorStr = rhs;
      }
    }
    // [*<scale>][@<angle>]
    else {
      auto pos2 = rhs.indexOf("*");

      // *<scale>[@<angle>]
      if (pos2 >= 0) {
        dataStr = rhs.mid(0, pos2);
        rhs     = rhs.mid(pos2 + 1);

        auto pos3 = rhs.indexOf("@");

        // @<angle>
        if (pos3 >= 0) {
          angleStr = rhs.mid(pos3 + 1);
          scaleStr = rhs.mid(0, pos3);
        }
        else
          scaleStr = rhs;
      }
      // [@<angle>]
      else {
        auto pos3 = rhs.indexOf("@");

        // @<angle>
        if (pos3 >= 0) {
          angleStr = rhs.mid(pos3 + 1);
          dataStr  = rhs.mid(0, pos3);
        }
        else
          dataStr = rhs;
      }
    }
  }
  // [*<scale>][@<angle>]
  else {
    rhs = lhs;

    auto pos1 = rhs.indexOf("*");

    // *<scale>[@<angle>]
    if (pos1 >= 0) {
      typeStr = rhs.mid(0, pos1);
      rhs     = rhs.mid(pos1 + 1);

      auto pos2 = rhs.indexOf("@");

      // @<angle>
      if (pos2 >= 0) {
        angleStr = rhs.mid(pos2 + 1);
        scaleStr = rhs.mid(0, pos2);
      }
      else
        scaleStr = rhs;
    }
    // [@<angle>]
    else {
      auto pos2 = rhs.indexOf("@");

      if (pos2 >= 0) {
        angleStr = rhs.mid(pos2 + 1);
        typeStr  = rhs.mid(0, pos2);
      }
      else
        typeStr = rhs;
    }
  }

  auto type = stringToType(typeStr);

  if (type == Type::NONE)
    return false;

  type_ = type;

  if (scaleStr != "") {
    bool ok;
    scale_ = scaleStr.toDouble(&ok);
    if (! ok) scale_ = 1.0;
  }

  if      (type_ == Type::PALETTE)
    palette_ = dataStr;
  else if (type_ == Type::IMAGE)
    image_ = dataStr;

  if (angleStr != "") {
    bool ok;
    angle_ = Angle(angleStr.toDouble(&ok));
    if (! ok) angle_ = Angle();
  }

  if (altColorStr != "")
    altColor_ = Color(altColorStr);

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
  if (ustr == "IMAGE"     ) return Type::IMAGE;

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
    case Type::IMAGE:     return "IMAGE";
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
    case Qt::TexturePattern       : return Type::IMAGE;
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
    case Type::IMAGE:     return Qt::TexturePattern;
    default:              return Qt::NoBrush;
  }
}

QStringList
CQChartsFillPattern::
enumNames() const
{
  static QStringList names = QStringList() <<
    "SOLID" << "HATCH" << "DENSE" << "HORIZONTAL" << "VERTICAL" << "FDIAG" << "BDIAG" <<
    "LGRADIENT" << "RGRADIENT" << "PALETTE" << "IMAGE";

  return names;
}
