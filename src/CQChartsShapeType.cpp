#include <CQChartsShapeType.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsShapeType, toString, fromString)

int CQChartsShapeType::metaTypeId;

//---

QString
CQChartsShapeType::
typeToName(Type type)
{
  switch (type) {
    case Type::BOX:           return "box";
    case Type::POLYGON:       return "polygon";
    case Type::CIRCLE:        return "circle";
    case Type::DOT:           return "dot";
    case Type::TRIANGLE:      return "triangle";
    case Type::DIAMOND:       return "diamond";
    case Type::TRAPEZIUM:     return "trapezium";
    case Type::PARALLELOGRAM: return "parallelogram";
    case Type::HOUSE:         return "house";
    case Type::PENTAGON:      return "pentagon";
    case Type::HEXAGON:       return "hexagon";
    case Type::SEPTAGON:      return "septagon";
    case Type::OCTAGON:       return "octagon";
    case Type::DOUBLE_CIRCLE: return "double_circle";
    case Type::STAR:          return "star";
    case Type::UNDERLINE:     return "underline";
    case Type::CYLINDER:      return "cylinder";
    case Type::NOTE:          return "note";
    case Type::TAB:           return "tab";
    case Type::FOLDER:        return "folder";
    case Type::BOX3D:         return "box3d";
    case Type::COMPONENT:     return "component";
    case Type::CDS:           return "cds";
    case Type::ARROW:         return "arrow";
    case Type::LPROMOTER:     return "lpromoter";
    case Type::RPROMOTER:     return "rpromoter";
    default:
    case Type::NONE:          return "none";
  }
}

CQChartsShapeType::Type
CQChartsShapeType::
nameToType(const QString &str)
{
  auto lstr = str.toLower();

  if (lstr == "box"         ||
      lstr == "rect"        ||
      lstr == "rectangle"   ||
      lstr == "square"       ) return Type::BOX;
  if (lstr == "polygon"      ) return Type::POLYGON;
  if (lstr == "circle"      ||
      lstr == "ellipse"     ||
      lstr == "oval"         ) return Type::CIRCLE;
  if (lstr == "dot"          ) return Type::DOT;
  if (lstr == "triangle"     ) return Type::TRIANGLE;
  if (lstr == "diamond"      ) return Type::DIAMOND;
  if (lstr == "parallelogram") return Type::PARALLELOGRAM;
  if (lstr == "house"        ) return Type::HOUSE;
  if (lstr == "pentagon"     ) return Type::PENTAGON;
  if (lstr == "hexagon"      ) return Type::HEXAGON;
  if (lstr == "septagon"     ) return Type::SEPTAGON;
  if (lstr == "octagon"      ) return Type::OCTAGON;
  if (lstr == "double_circle") return Type::DOUBLE_CIRCLE;
  if (lstr == "star"         ) return Type::STAR;
  if (lstr == "underline"    ) return Type::UNDERLINE;
  if (lstr == "cylinder"     ) return Type::CYLINDER;
  if (lstr == "note"         ) return Type::NOTE;
  if (lstr == "tab"          ) return Type::TAB;
  if (lstr == "folder"       ) return Type::FOLDER;
  if (lstr == "box3d"        ) return Type::BOX3D;
  if (lstr == "component"    ) return Type::COMPONENT;
  if (lstr == "cds"          ) return Type::CDS;
  if (lstr == "arrow"        ) return Type::ARROW;
  if (lstr == "lpromoter"    ) return Type::LPROMOTER;
  if (lstr == "rpromoter"    ) return Type::RPROMOTER;

  return Type::NONE;
}

QStringList
CQChartsShapeType::
typeNames()
{
  static auto typeNames = QStringList() <<
    "box" << "polygon" << "circle" << "dot" << "triangle" << "diamond" <<
    "parallelogram" << "house" << "pentagon" << "hexagon" << "septagon" << "octagon" <<
    "double_circle" << "star" << "underline" << "cylinder" << "note" << "tab" <<
    "folder" << "box3d" << "component" << "cds" << "arrow" << "lpromoter" << "rpromoter";

  return typeNames;
}

//---

void
CQChartsShapeType::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsShapeType);

  CQPropertyViewMgrInst->setUserName("CQChartsShapeType", "shapeType");
}

CQChartsShapeType::
CQChartsShapeType(Type type) :
 type_(type)
{
  assert(isValidType(type_));
}

CQChartsShapeType::
CQChartsShapeType(const QString &s)
{
  if (! fromString(s))
    type_ = Type::NONE;
}

QString
CQChartsShapeType::
toString() const
{
  return typeToName(type_);
}

bool
CQChartsShapeType::
fromString(const QString &s)
{
  type_ = nameToType(s);

  return (type_ != Type::NONE);
}
