#include <CQChartsCmdArgs.h>
#include <CQTclUtil.h>

CQChartsCmdBaseArgs::
CQChartsCmdBaseArgs(const QString &cmdName, const Args &argv) :
 CQTclCmd::CmdArgs(cmdName, argv)
{
}

//---

bool
CQChartsCmdBaseArgs::
getOptValue(Qt::Alignment &a)
{
  QString str;

  if (! getOptValue(str))
    return false;

  a = CQAlignEdit::fromString(str);

  return true;
}

#if 0
bool
CQChartsCmdBaseArgs::
getOptValue(QFont &f)
{
  QString str;

  if (! getOptValue(str))
    return false;

  f = QFont(str);

  return true;
}
#endif

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsFont &f)
{
  QString str;

  if (! getOptValue(str))
    return false;

  f = CQChartsFont(str);

  return true;
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsLength &l)
{
  return getOptValue<CQChartsLength>(l);
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsPosition &p)
{
  return getOptValue<CQChartsPosition>(p);
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsRect &r)
{
  return getOptValue<CQChartsRect>(r);
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsColor &c)
{
  return getOptValue<CQChartsColor>(c);
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsLineDash &d)
{
  return getOptValue<CQChartsLineDash>(d);
}

bool
CQChartsCmdBaseArgs::
getOptValue(Polygon &poly)
{
  QString str;

  if (! getOptValue(str))
    return false;

  poly = stringToPolygon(str);

  return poly.size();
}

bool
CQChartsCmdBaseArgs::
getOptValue(CQChartsReals &r)
{
  return getOptValue<CQChartsReals>(r);
}

//---

bool
CQChartsCmdBaseArgs::
handleParseArg(CQTclCmd::CmdArg *cmdArg, const QString &opt)
{
  // handle color option (string)
  if      (cmdArg->type() == int(Type::Color)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle line dash option (string)
  else if (cmdArg->type() == int(Type::LineDash)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle length option (string)
  else if (cmdArg->type() == int(Type::Length)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle position option (string)
  else if (cmdArg->type() == int(Type::Position)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle rect option (string)
  else if (cmdArg->type() == int(Type::Rect)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle polygon option (string)
  else if (cmdArg->type() == int(Type::Polygon)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle align option (string)
  else if (cmdArg->type() == int(Type::Align)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle sides option (string)
  else if (cmdArg->type() == int(Type::Sides)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle column option (string)
  else if (cmdArg->type() == int(Type::Column)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle row option (string)
  else if (cmdArg->type() == int(Type::Row)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // handle position option (string)
  else if (cmdArg->type() == int(Type::Reals)) {
    QString str;

    if (getOptValue(str)) {
      parseStr_[opt].push_back(str);
    }
    else {
      return valueError(opt);
    }
  }
  // invalid type (assert ?)
  else {
    if (CQTclCmd::CmdArgs::handleParseArg(cmdArg, opt))
      return true;

    std::cerr << "Invalid type for '" << opt.toStdString() << "'\n";

    return false;
  }

  return true;
}

//---

Qt::Alignment
CQChartsCmdBaseArgs::
getParseAlign(const QString &name, Qt::Alignment def) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return def;

  return CQAlignEdit::fromString((*p).second[0]);
}

#if 0
QFont
CQChartsCmdBaseArgs::
getParseFont(const QString &name, const QFont &def) const
{
  return getParseValue<QFont>(name, def);
}
#endif

CQChartsCmdBaseArgs::Polygon
CQChartsCmdBaseArgs::
getParsePoly(const QString &name, const Polygon &def) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return def;

  QString value = (*p).second[0];

  Polygon poly;

  if (! parsePoly(value, poly))
    return def;

  return poly;
}

CQChartsFont
CQChartsCmdBaseArgs::
getParseFont(const QString &name, const CQChartsFont &def) const
{
  return getParseValue<CQChartsFont>(name, def);
}

CQChartsColor
CQChartsCmdBaseArgs::
getParseColor(const QString &name, const CQChartsColor &def) const
{
  return getParseValue<CQChartsColor>(name, def);
}

CQChartsAlpha
CQChartsCmdBaseArgs::
getParseAlpha(const QString &name, const CQChartsAlpha &def) const
{
  return getParseValue<CQChartsAlpha>(name, def);
}

CQChartsAngle
CQChartsCmdBaseArgs::
getParseAngle(const QString &name, const CQChartsAngle &def) const
{
  return getParseValue<CQChartsAngle>(name, def);
}

CQChartsLineDash
CQChartsCmdBaseArgs::
getParseLineDash(const QString &name, const CQChartsLineDash &def) const
{
  return getParseValue<CQChartsLineDash>(name, def);
}

CQChartsSides
CQChartsCmdBaseArgs::
getParseSides(const QString &name, const CQChartsSides &def) const
{
  return getParseValue<CQChartsSides>(name, def);
}

//----

CQChartsCmdBaseArgs::Polygon
CQChartsCmdBaseArgs::
stringToPolygon(const QString &str) const
{
  Polygon poly;

  CQStrParse parse(str);

  while (! parse.eof()) {
    parse.skipSpace();

    QString xstr;

    if (! parse.readNonSpace(xstr))
      break;

    parse.skipSpace();

    QString ystr;

    if (! parse.readNonSpace(ystr))
      break;

    parse.skipSpace();

    double x, y;

    if (! CQChartsUtil::toReal(xstr, x))
      break;

    if (! CQChartsUtil::toReal(ystr, y))
      break;

    Point p(x, y);

    poly.addPoint(p);
  }

  return poly;
}

bool
CQChartsCmdBaseArgs::
parsePoly(const QString &str, Polygon &poly) const
{
  CQStrParse parse(str);

  return parsePoly(parse, poly);
}

bool
CQChartsCmdBaseArgs::
parsePoly(CQStrParse &parse, Polygon &poly) const
{
  parse.skipSpace();

  if (parse.isChar('{')) {
    int pos1 = parse.getPos();

    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('{')) {
      parse.setPos(pos1);

      if (! parse.skipBracedString())
        return false;

      int pos2 = parse.getPos();

      QString str = parse.getAt(pos1 + 1, pos2 - pos1 - 2);

      return parsePoly(str, poly);
    }

    parse.setPos(pos1);
  }

  //--

  while (! parse.eof()) {
    Point p;

    if (! parsePoint(parse, p))
      return false;

    poly.addPoint(p);
  }

  return true;
}

bool
CQChartsCmdBaseArgs::
parsePoint(const QString &str, Point &pos) const
{
  CQStrParse parse(str);

  return parsePoint(parse, pos);
}

bool
CQChartsCmdBaseArgs::
parsePoint(CQStrParse &parse, Point &pos) const
{
  parse.skipSpace();

  if (parse.isChar('{')) {
    int pos1 = parse.getPos();

    if (! parse.skipBracedString())
      return false;

    int pos2 = parse.getPos();

    QString str = parse.getAt(pos1 + 1, pos2 - pos1 - 2);

    return parsePoint(str, pos);
  }

  QString xstr;

  if (! parse.readNonSpace(xstr))
    return false;

  parse.skipSpace();

  QString ystr;

  if (! parse.readNonSpace(ystr))
    return false;

  parse.skipSpace();

  double x, y;

  if (! CQChartsUtil::toReal(xstr, x))
    return false;

  if (! CQChartsUtil::toReal(ystr, y))
    return false;

  pos = Point(x, y);

  return true;
}

//---

CQChartsCmdArgs::
CQChartsCmdArgs(const QString &cmdName, const Args &argv) :
 CQChartsCmdBaseArgs(cmdName, argv)
{
}

CQChartsLength
CQChartsCmdArgs::
getParseLength(CQChartsView *view, CQChartsPlot *plot, const QString &name,
               const CQChartsLength &def) const
{
  return getParseValue<CQChartsLength>(view, plot, name, def);
}

CQChartsPosition
CQChartsCmdArgs::
getParsePosition(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                 const CQChartsPosition &def) const
{
  return getParseValue<CQChartsPosition>(view, plot, name, def);
}

CQChartsRect
CQChartsCmdArgs::
getParseRect(CQChartsView *view, CQChartsPlot *plot, const QString &name,
             const CQChartsRect &def) const
{
  return getParseValue<CQChartsRect>(view, plot, name, def);
}

CQChartsMargin
CQChartsCmdArgs::
getParseMargin(CQChartsView *view, CQChartsPlot *plot, const QString &name,
               const CQChartsMargin &def) const
{
  return getParseValue<CQChartsMargin>(view, plot, name, def);
}

CQChartsPoints
CQChartsCmdArgs::
getParsePoints(CQChartsView *view, CQChartsPlot *plot, const QString &name,
               const CQChartsPoints &def) const
{
  return getParseValue<CQChartsPoints>(view, plot, name, def);
}

//---

CQChartsColumn
CQChartsCmdArgs::
getParseColumn(const QString &name, QAbstractItemModel *model, const CQChartsColumn &def) const
{
  return getParseValue<CQChartsColumn>(model, name, def);
}

#if 0
CQChartsColumn
CQChartsCmdArgs::
getParseColumn(const QString &name, QAbstractItemModel *model) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return CQChartsColumn();

  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model, (*p).second[0], column))
    return CQChartsColumn();

  return column;
}
#endif

CQChartsRow
CQChartsCmdArgs::
getParseRow(const QString &name, CQChartsPlot *plot, const CQChartsRow &def) const
{
  return getParseValue<CQChartsRow>(plot, name, def);
}

#if 0
CQChartsRow
CQChartsCmdArgs::
getParseRow(const QString &name, CQChartsPlot *plot) const
{
  auto p = parseStr_.find(name);
  if (p == parseStr_.end()) return -1;

  QString rowName = (*p).second[0];

  bool ok;

  int irow = CQChartsUtil::toInt(rowName, ok);

  if (! ok) {
    if (plot)
      irow = plot->getRowForId(rowName);
    else
      irow = -1;
  }

  return CQChartsRow(irow);
}
#endif

CQChartsReals
CQChartsCmdArgs::
getParseReals(const QString &name, const CQChartsReals &def) const
{
  return getParseValue<CQChartsReals>(name, def);
}
