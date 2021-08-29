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

void
CQChartsFillPattern::
setImage(const Image &i)
{
  image_ = i;

  if      (type_ == Type::TEXTURE)
    image_.setImageType("texture");
  else if (type_ == Type::MASK)
    image_.setImageType("mask");
  else
    image_.setImageType("image");
}

QString
CQChartsFillPattern::
toString() const
{
  if (! isValid())
    return "";

  auto str = typeToString(type_);

  if      (type_ == Type::PALETTE) {
    auto name = palette().toString();

    if (name != "")
      str += ",name=" + name;
  }
  else if (type_ == Type::IMAGE || type_ == Type::TEXTURE || type_ == Type::MASK) {
    auto name = image().toString();

    if (name != "")
      str += ",name=" + name;
  }

  if (altColor().isValid()) {
    str += QString(",altColor=%1").arg(altColor().toString());

    if (altAlpha() != Alpha())
      str += QString(",altAlpha=%1").arg(altAlpha().toString());
  }

  if (scale() != 1.0)
    str += QString(",scale=%1").arg(scale());

  if (! angle().isZero())
    str += QString(",angle=%1").arg(angle().toString());

  return str;
}

bool
CQChartsFillPattern::
fromString(const QString &s)
{
  return setValue(s);
}

// format:
//   <typeStr>[,name=<dataStr>][,altColor=<altColor>][,altAlpha=<altAlpha>]
//            [,scale=<scale>][,angle=<angle>]
bool
CQChartsFillPattern::
setValue(const QString &s)
{
  reset();

  if (s.trimmed() == "")
    return true;

  //---

  auto strs = s.trimmed().split(',', QString::SkipEmptyParts);

  for (int i = 0; i < strs.length(); ++i) {
    if (i == 0) {
      auto type = stringToType(strs[0]);
      if (type == Type::NONE) return false;

      type_ = type;
    }
    else {
      auto pos = strs[i].indexOf("=");

      auto name  = strs[i].mid(0, pos).trimmed();
      auto value = strs[i].mid(pos + 1).trimmed();

      if      (name == "name") {
        if      (type_ == Type::PALETTE)
          palette_ = CQChartsPaletteName(name);
        else if (type_ == Type::IMAGE || type_ == Type::TEXTURE || type_ == Type::MASK) {
          image_ = CQChartsImage(name); // TODO

          if      (type_ == Type::TEXTURE)
            image_.setImageType("texture");
          else if (type_ == Type::MASK)
            image_.setImageType("mask");
          else
            image_.setImageType("image");
        }
      }
      else if (name == "altColor") {
        altColor_ = Color(value);
      }
      else if (name == "altAlpha") {
        bool ok;
        altAlpha_ = Alpha(value.toDouble(&ok));
        if (! ok) altAlpha_ = Alpha();
      }
      else if (name == "scale") {
        bool ok;
        scale_ = value.toDouble(&ok);
        if (! ok) scale_ = 1.0;
      }
      else if (name == "angle") {
        bool ok;
        angle_ = Angle(value.toDouble(&ok));
        if (! ok) angle_ = Angle();
      }
    }
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
  if (ustr == "IMAGE"     ) return Type::IMAGE;
  if (ustr == "TEXTURE"   ) return Type::TEXTURE;
  if (ustr == "MASK"      ) return Type::MASK;

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
    case Type::TEXTURE:   return "TEXTURE";
    case Type::MASK:      return "MASK";
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
    case Type::TEXTURE:   return Qt::TexturePattern;
    case Type::MASK:      return Qt::TexturePattern;
    default:              return Qt::NoBrush;
  }
}

QStringList
CQChartsFillPattern::
enumNames() const
{
  static auto names = QStringList() <<
    "SOLID" << "HATCH" << "DENSE" << "HORIZONTAL" << "VERTICAL" << "FDIAG" << "BDIAG" <<
    "LGRADIENT" << "RGRADIENT" << "PALETTE" << "IMAGE" << "TEXTURE" << "MASK";

  return names;
}
