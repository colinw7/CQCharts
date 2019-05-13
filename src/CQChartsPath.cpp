#include <CQChartsPath.h>
#include <CQPropertyView.h>
#include <CQStrParse.h>

CQUTIL_DEF_META_TYPE(CQChartsPath, toString, fromString)

int CQChartsPath::metaTypeId;

void
CQChartsPath::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPath);

  CQPropertyViewMgrInst->setUserName("CQChartsPath", "path");
}

QString
CQChartsPath::
toString() const
{
  if (! path_)
    return "";

  QString str;

  int n = path_->elementCount();

  if (n == 0)
    return "";

  for (int i = 0; i < n; ++i) {
    const QPainterPath::Element &e = path_->elementAt(i);

    if (str.length())
      str += " ";

    if      (e.isMoveTo())
      str += QString("M %1 %2").arg(e.x).arg(e.y);
    else if (e.isLineTo())
      str += QString("L %1 %2").arg(e.x).arg(e.y);
    else if (e.isCurveTo())
      str += QString("C %1 %2").arg(e.x).arg(e.y);
  }

  return str;
}

bool
CQChartsPath::
fromString(const QString &str)
{
  delete path_;

  path_ = new QPainterPath;

  //---

  CQStrParse parse(str);

  auto parseChar = [&]() -> QChar {
    parse.skipSpace();

    QChar c = parse.getChar();

    parse.skipSpace();

    return c;
  };

  auto parseReal = [&](double &r) -> bool {
    if (! parse.readReal(&r))
      return false;

    parse.skipSpace();

    return true;
  };

  auto parsePoint = [&](QPointF &p) -> bool {
    double x = 0.0, y = 0.0;

    if (! parseReal(x)) return false;
    if (! parseReal(y)) return false;

    p = QPointF(x, y);

    return true;
  };

  //---

  bool valid = true;

  while (! parse.eof()) {
    QChar c = parseChar();

    if      (c == 'M') {
      QPointF p;

      if (! parsePoint(p)) { valid = false; break; }

      path_->moveTo(p);
    }
    else if (c == 'L') {
      QPointF p;

      if (! parsePoint(p)) { valid = false; break; }

      path_->lineTo(p);
    }
    else if (c == 'C') {
      QPointF p1, p2, p3;

      if (! parsePoint(p1)) { valid = false; break; }
      if (! parsePoint(p2)) { valid = false; break; }
      if (! parsePoint(p3)) { valid = false; break; }

      path_->cubicTo(p1, p2, p3);
    }
    else if (c == 'Z' || c == 'z') {
      path_->closeSubpath();
    }
    else {
      valid = false; break;
    }
  }

#if 0
  if (! valid)
    std::cerr << "Invalid path: " << str.toStdString() << "\n";
#endif

  return valid;
}
