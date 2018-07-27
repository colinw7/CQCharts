#include <CQChartsUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsModelFilter.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQDataModel.h>

#include <CQUtil.h>
#include <CQStrParse.h>
#include <QSortFilterProxyModel>
#include <QFontMetricsF>

namespace {

void errorMsg(const QString &msg) {
  std::cerr << msg.toStdString() << std::endl;
}

}

//---

namespace CQChartsUtil {

bool isHierarchical(QAbstractItemModel *model) {
  if (! model)
    return false;

  QModelIndex parent;

  int nr = model->rowCount(parent);

  for (int row = 0; row < nr; ++row) {
    QModelIndex index1 = model->index(row, 0, parent);

    if (model->hasChildren(index1))
      return true;
  }

  return false;
}

int hierRowCount(QAbstractItemModel *model) {
  CQChartsModelVisitor visitor;

  visitModel(model, visitor);

  return visitor.numRows();
}

bool visitModel(QAbstractItemModel *model, CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  visitor.init(model);

  QModelIndex parent;

  (void) visitModelIndex(model, parent, visitor);

  visitor.term();

  return true;
}

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  visitor.init(model);

  (void) visitModelRow(model, parent, r, visitor);

  visitor.term();

  return true;
}

CQChartsModelVisitor::State
visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent,
                CQChartsModelVisitor &visitor) {
  int nr = model->rowCount(parent);

  visitor.setNumRows(nr);

  for (int r = 0; r < nr; ++r) {
    CQChartsModelVisitor::State state = visitModelRow(model, parent, r, visitor);

    if (state == CQChartsModelVisitor::State::TERMINATE) return state;
    if (state == CQChartsModelVisitor::State::SKIP     ) continue;
  }

  return CQChartsModelVisitor::State::OK;
}

CQChartsModelVisitor::State
visitModelRow(QAbstractItemModel *model, const QModelIndex &parent, int r,
              CQChartsModelVisitor &visitor) {
  QModelIndex ind1 = model->index(r, 0, parent);

  if (visitor.maxRows() > 0 && r > visitor.maxRows())
    return CQChartsModelVisitor::State::TERMINATE;

  if (model->hasChildren(ind1)) {
    CQChartsModelVisitor::State state = visitor.hierVisit(model, parent, r);
    if (state != CQChartsModelVisitor::State::OK) return state;

    CQChartsModelVisitor::State iterState = visitModelIndex(model, ind1, visitor);
    if (iterState != CQChartsModelVisitor::State::OK) return iterState;

    CQChartsModelVisitor::State postState = visitor.hierPostVisit(model, parent, r);
    if (postState != CQChartsModelVisitor::State::OK) return postState;
  }
  else {
    CQChartsModelVisitor::State preState = visitor.preVisit(model, parent, r);
    if (preState != CQChartsModelVisitor::State::OK) return preState;

    CQChartsModelVisitor::State state = visitor.visit(model, parent, r);
    if (state != CQChartsModelVisitor::State::OK) return state;

    visitor.step();

    // postVisit ?
  }

  return CQChartsModelVisitor::State::OK;
}

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent) {
  QString path;

  QModelIndex pind = parent;

  while (pind.isValid()) {
    bool ok;

    QString str = CQChartsUtil::modelString(model, pind, ok);

    if (! ok)
      break;

    if (path.length())
      path = "/" + path;

    path = str + path;

    pind = pind.parent();
  }

  return path;
}

//------

// get type and associated name values for column
//  . column can be model column, header or custom expresssion
bool
columnValueType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                CQBaseModel::Type &columnType, CQChartsNameValues &nameValues) {
  assert(model);

  if (column.type() == CQChartsColumn::Type::DATA ||
      column.type() == CQChartsColumn::Type::DATA_INDEX) {
    // get column number and validate
    int icolumn = column.column();

    if (icolumn < 0 || icolumn >= model->columnCount()) {
      columnType = CQBaseModel::Type::NONE;
      return false;
    }

    //---

    // use defined column type if available
    CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

    if (columnTypeMgr->getModelColumnType(model, icolumn, columnType, nameValues)) {
      if (column.type() == CQChartsColumn::Type::DATA_INDEX) {
        CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

        CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

        if (typeData) {
          columnType = typeData->indexType(column.index());
        }
      }

      return true;
    }

    //---

    // determine column type from values
    // TODO: cache (in plot ?), max visited values

    // process model data
    class ColumnTypeVisitor : public CQChartsModelVisitor {
     public:
      ColumnTypeVisitor(int column) :
       column_(column) {
      }

      State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
        QModelIndex ind = model->index(row, column_, parent);

        // if column can be integral, check if value is valid integer
        if (isInt_) {
          bool ok;

          (void) CQChartsUtil::modelInteger(model, ind, ok);

          if (ok)
            return State::SKIP;

          QString str = CQChartsUtil::modelString(model, ind, ok);

          if (! str.length())
            return State::SKIP;

          isInt_ = false;
        }

        // if column can be real, check if value is valid real
        if (isReal_) {
          bool ok;

          (void) CQChartsUtil::modelReal(model, ind, ok);

          if (ok)
            return State::SKIP;

          QString str = CQChartsUtil::modelString(model, ind, ok);

          if (! str.length())
            return State::SKIP;

          isReal_ = false;
        }

        // not value real or integer so assume string and we are done
        return State::TERMINATE;
      }

      CQBaseModel::Type columnType() {
        if      (isInt_ ) return CQBaseModel::Type::INTEGER;
        else if (isReal_) return CQBaseModel::Type::REAL;
        else              return CQBaseModel::Type::STRING;
      }

     private:
      int  column_ { -1 };   // column to check
      bool isInt_  { true }; // could be integeral
      bool isReal_ { true }; // could be real
    };

    // determine column value type by looking at model values
    ColumnTypeVisitor columnTypeVisitor(icolumn);

    visitModel(model, columnTypeVisitor);

    columnType = columnTypeVisitor.columnType();

    return true;
  }
  else if (column.type() == CQChartsColumn::Type::GROUP) {
    columnType = CQBaseModel::Type::INTEGER;
    return true;
  }
  else {
    // TODO: for custom expression should determine expression result type (if possible)
    columnType = CQBaseModel::Type::STRING;
    return true;
  }
}

// use column format string to format a value as data (used by axis)
//  TODO: separate format string from column type to remove dependence
bool
formatColumnTypeValue(CQCharts *charts, const QString &typeStr, double value, QString &str) {
  CQChartsNameValues nameValues;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  if (! typeData)
    return false;

  return formatColumnTypeValue(typeData, nameValues, value, str);
}

// use column type details to format an internal value (real) to a display value
//  TODO: value should be variant ?
bool
formatColumnValue(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                  double value, QString &str) {
  CQBaseModel::Type  columnType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

  if (! typeData)
    return false;

  return formatColumnTypeValue(typeData, nameValues, value, str);
}

bool
formatColumnTypeValue(CQChartsColumnType *typeData, const CQChartsNameValues &nameValues,
                      double value, QString &str) {
  bool converted;

  QVariant var = typeData->dataName(value, nameValues, converted);

  if (! var.isValid())
    return false;

  variantToString(var, str);

  return true;
}

// use column type details to format an internal model value (variant) to a display value
QVariant
columnDisplayData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                  const QVariant &var, bool &converted) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getDisplayData(model, column, var, converted);

  return var1;
}

// use column type details to format an internal model value (variant) to a editable value
QVariant
columnUserData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var, bool &converted) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getUserData(model, column, var, converted);

  return var1;
}

// get type string for column (type name and name values)
bool
columnTypeStr(CQCharts *charts, QAbstractItemModel *model,
              const CQChartsColumn &column, QString &typeStr) {
  CQBaseModel::Type  columnType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  typeStr = columnTypeMgr->encodeTypeData(columnType, nameValues);

  return true;
}

bool
setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes)
{
  bool rc = true;

  // split into multiple column type definitions
  QStringList fstrs = columnTypes.split(";", QString::KeepEmptyParts);

  for (int i = 0; i < fstrs.length(); ++i) {
    QString typeStr = fstrs[i].simplified();

    if (! typeStr.length())
      continue;

    // default column to index
    CQChartsColumn column(i);

    // if #<col> then use that for column index
    int pos = typeStr.indexOf("#");

    if (pos >= 0) {
      QString columnStr = typeStr.mid(0, pos).simplified();

      CQChartsColumn column1;

      if (stringToColumn(model, columnStr, column1))
        column = column1;
      else {
        charts->errorMsg("Bad column name '" + columnStr + "'");
        rc = false;
      }

      typeStr = typeStr.mid(pos + 1).simplified();
    }

    //---

    if (! setColumnTypeStr(charts, model, column, typeStr)) {
      charts->errorMsg(QString("Invalid type '" + typeStr + "' for column '%1'").
                         arg(column.toString()));
      rc = false;
      continue;
    }
  }

  return rc;
}

// set type string for column (type name and name values)
bool
setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                 const QString &typeStr) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // decode to type name and name values
  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  if (! typeData)
    return false;

  // store in model
  CQBaseModel::Type columnType = typeData->type();

  if (! columnTypeMgr->setModelColumnType(model, column, columnType, nameValues))
    return false;

  return true;
}

}

//------

namespace CQChartsUtil {

double toReal(const QString &str, bool &ok) {
  ok = true;

  double r = 0.0;

  //---

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return r;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0)
    p = &c_str[i + 3];
  else {
    errno = 0;

    r = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      ok = false;
      return r;
    }
  }
#else
  errno = 0;

  r = strtod(&c_str[i], (char **) &p);

  if (errno == ERANGE) {
    ok = false;
    return r;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    ok = false;

  return r;
}

bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

//------

long toInt(const QString &str, bool &ok) {
  ok = true;

  long integer = 0;

  std::string sstr = str.toStdString();

  const char *c_str = sstr.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return integer;
  }

  const char *p;

  errno = 0;

  integer = strtol(&c_str[i], (char **) &p, 10);

  if (errno == ERANGE) {
    ok = false;
    return integer;
  }

  if (*p == '.') {
    ++p;

    while (*p == '0')
      ++p;
  }

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0') {
    ok = false;
    return integer;
  }

  return integer;
}

bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

QString toString(double r, const QString &fmt) {
#ifdef ALLOW_NAN
  if (COS::is_nan(real))
    return "NaN";
#endif

  if (fmt == "%g" && isZero(r))
    return "0.0";

  static char buffer[128];

  if (fmt == "%T") {
    // format real in buffer
    bool negative = (r < 0);

    if (negative)
      r = -r;

    ::sprintf(buffer, "%g", r);

    // find decimal point - if less than 4 digits to left then done
    QString res = buffer;

    int pos = res.indexOf('.');

    if (pos < 0)
      pos = res.length();

    if (pos <= 3) {
      if (negative)
        return "-" + res;
      else
        return res;
    }

    // add commas to value to left of decimal point
    QString rhs = res.mid(pos);

    int ncomma = pos/3; // number of comma to add

    QString lhs;

    if (negative)
      lhs = "-";

    // add digits before first comma
    int pos1 = pos - ncomma*3;

    lhs += res.mid(0, pos1);

    for (int i = 0; i < ncomma; ++i) {
      // add comma and next set of 3 digits
      lhs += "," + res.mid(pos1, 3);

      pos1 += 3;
    }

    return lhs + rhs;
  }
  else {
    // format real in buffer
    ::sprintf(buffer, fmt.toLatin1().constData(), r);

    return buffer;
  }
}

QString toString(long i, const QString &fmt) {
  static char buffer[64];

  ::sprintf(buffer, fmt.toLatin1().constData(), i);

  return buffer;
}

//---

QString toString(const std::vector<CQChartsColumn> &columns) {
  QString str;

  for (std::size_t i = 0; i < columns.size(); ++i) {
    if (str.length())
      str += " ";

    str += QString("%1").arg(columns[i].toString());
  }

  return str;
}

bool fromString(const QString &str, std::vector<CQChartsColumn> &columns) {
  bool ok = true;

  columns.clear();

  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.size(); ++i) {
    bool ok1;

    int col = strs[i].toInt(&ok1);

    if (ok1)
      columns.push_back(col);
    else
      ok = false;
  }

  return ok;
}

//---

bool intersectLines(const QPointF &l1s, const QPointF &l1e,
                    const QPointF &l2s, const QPointF &l2e, QPointF &pi) {
  double dx1 = l1e.x() - l1s.x();
  double dy1 = l1e.y() - l1s.y();
  double dx2 = l2e.x() - l2s.x();
  double dy2 = l2e.y() - l2s.y();

  double delta = dx1*dy2 - dy1*dx2;

  if (fabs(delta) < 1E-6) // parallel
    return false;

  double idelta = 1.0/delta;

  double dx = l2s.x() - l1s.x();
  double dy = l2s.y() - l1s.y();

  double m1 = (dx*dy2 - dy*dx2)*idelta;
//double m2 = (dx*dy1 - dy*dx1)*idelta;

  double xi = l1s.x() + m1*dx1;
  double yi = l1s.y() + m1*dy1;

  pi = QPointF(xi, yi);

  return true;
}

bool intersectLines(double x11, double y11, double x21, double y21,
                    double x12, double y12, double x22, double y22,
                    double &xi, double &yi) {
  QPointF pi;

  bool rc = intersectLines(QPointF(x11, y11), QPointF(x21, y21),
                           QPointF(x12, y12), QPointF(x22, y22), pi);

  xi = pi.x();
  yi = pi.y();

  return rc;
}

//---

QColor bwColor(const QColor &c) {
  int g = qGray(c.red(), c.green(), c.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
}

QColor invColor(const QColor &c) {
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue());
}

QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

QColor blendColors(const std::vector<QColor> &colors) {
  if (colors.empty())
    return QColor();

  double f = 1.0/colors.size();

  double r = 0.0;
  double g = 0.0;
  double b = 0.0;

  for (const auto &c : colors) {
    r += c.redF  ()*f;
    g += c.greenF()*f;
    b += c.blueF ()*f;
  }

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

//------

void penSetLineDash(QPen &pen, const CQChartsLineDash &dash) {
  int num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < num; ++i)
      dashes << dash.getLength(i)*w;

    if (num & 1)
      dashes << dash.getLength(0)*w;

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}

//---

QString polygonToString(const QPolygonF &poly) {
  int np = poly.size();

  QString str;

  for (int i = 0; i < np; ++i) {
    const QPointF &p = poly[i];

    str += QString("{%1 %2}").arg(p.x()).arg(p.y());
  }

  return str;
}

QString rectToString(const QRectF &rect) {
  const QPointF &tl = rect.topLeft    ();
  const QPointF &br = rect.bottomRight();

  return QString("{%1 %2 %3 %4}").arg(tl.x()).arg(tl.y()).arg(br.x()).arg(br.y());
}

QString pointToString(const QPointF &p) {
  return QString("%1 %2").arg(p.x()).arg(p.y());
}

bool variantToString(const QVariant &var, QString &str) {
  if      (var.type() == QVariant::String) {
    str = var.toString();
  }
  else if (var.type() == QVariant::Double) {
    str = toString(var.toDouble());
  }
  else if (var.type() == QVariant::Int) {
    str = toString((long) var.toInt());
  }
  else if (var.canConvert(QVariant::String)) {
    str = var.toString();
  }
  else if (var.type() == QVariant::PolygonF) {
    QPolygonF poly = var.value<QPolygon>();

    str = polygonToString(poly);
  }
  else if (var.type() == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    str = rectToString(rect);
  }
  else if (var.type() == QVariant::UserType) {
#if 0
    if      (var.userType() == CQChartsPath::metaType()) {
      CQChartsPath path = var.value<CQChartsPath>();

      str = path.toString();
    }
    else if (var.userType() == CQChartsStyle::metaType()) {
      CQChartsStyle style = var.value<CQChartsStyle>();

      str = style.toString();
    }
    else {
      assert(false);
    }
#else
    if (! CQUtil::userVariantToString(var, str))
      assert(false);
#endif
  }
  else if (var.type() == QVariant::List) {
    QList<QVariant> vars = var.toList();

    QStringList strs;

    for (int i = 0; i < vars.length(); ++i) {
      QString str1;

      if (variantToString(vars[i], str1))
        strs.push_back(str1);
    }

    str = "{" + strs.join(" ") + "}";

    return true;
  }
  else {
    assert(false);

    return false;
  }

  return true;
}

}

//------

namespace CQChartsUtil {

// distance between two points
double PointPointDistance(const CQChartsGeom::Point &point1, const CQChartsGeom::Point &point2) {
  double dx = point1.x - point2.x;
  double dy = point1.y - point2.y;

  return std::hypot(dx, dy);
}

// distance between point and line
bool PointLineDistance(const CQChartsGeom::Point &point, const CQChartsGeom::Point &lineStart,
                       const CQChartsGeom::Point &lineEnd, double *dist) {
  double dx1 = lineEnd.x - lineStart.x;
  double dy1 = lineEnd.y - lineStart.y;

  double dx2 = point.x - lineStart.x;
  double dy2 = point.y - lineStart.y;

  double u1 = dx2*dx1 + dy2*dy1;
  double u2 = dx1*dx1 + dy1*dy1;

  if (u2 <= 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }

  double u = u1/u2;

  if      (u < 0.0) {
    *dist = PointPointDistance(point, lineStart);
    return false;
  }
  else if (u > 1.0) {
    *dist = PointPointDistance(point, lineEnd);
    return false;
  }
  else {
    CQChartsGeom::Point intersection = lineStart + u*CQChartsGeom::Point(dx1, dy1);

    *dist = PointPointDistance(point, intersection);

    return true;
  }
}

}

//------

namespace CQChartsUtil {

bool fileToLines(const QString &filename, QStringList &lines, int maxLines) {
  auto open = [&](FILE* &fp, const QString &filename) -> bool {
    fp = fopen(filename.toStdString().c_str(), "r");
    if (! fp) return false;

    return true;
  };

  auto readLine = [](FILE *fp, QString &line) {
    line = "";

    if (feof(fp)) return false;

    int c = fgetc(fp);

    if (c == EOF)
      return false;

    while (! feof(fp) && c != '\n') {
      line += c;

      c = fgetc(fp);
    }

    return true;
  };

  auto close = [](FILE* &fp) {
    if (fp)
      fclose(fp);

    fp = 0;
  };

  //---

  FILE *fp = nullptr;

  if (! open(fp, filename))
    return false;

  QString line;

  while (readLine(fp, line)) {
    lines.push_back(line);

    if (maxLines >= 0 && int(lines.size()) > maxLines)
      break;
  }

  close(fp);

  //---

  return true;
}

}

//------

namespace {

void findStringSplits1(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 1; i < len; ++i) {
    if (str[i].isSpace())
      splits.push_back(i);
  }
}

void findStringSplits2(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 0; i < len; ++i) {
    if (str[i].isPunct()) {
      int i1 = i;

      // keep consecutive punctuation together (::, ..., etc)
      while (i < len - 1 && str[i].isPunct())
        ++i;

      if (i1 == 0 || i >= len) // don't break if at start or end
        continue;

      splits.push_back(i1);
    }
  }
}

void findStringSplits3(const QString &str, std::vector<int> &splits) {
  int len = str.length();
  assert(len);

  for (int i = 1; i < len; ++i) {
    if (str[i - 1].isLower() && str[i].isUpper())
      splits.push_back(i);
  }
}

}

//------

namespace CQChartsUtil {

int nameToRole(const QString &name) {
  if      (name == "display"       ) return Qt::DisplayRole;
  else if (name == "edit"          ) return Qt::EditRole;
  else if (name == "user"          ) return Qt::UserRole;
  else if (name == "font"          ) return Qt::FontRole;
  else if (name == "size_hint"     ) return Qt::SizeHintRole;
  else if (name == "tool_tip"      ) return Qt::ToolTipRole;
  else if (name == "background"    ) return Qt::BackgroundRole;
  else if (name == "foreground"    ) return Qt::ForegroundRole;
  else if (name == "text_alignment") return Qt::TextAlignmentRole;
  else if (name == "text_color"    ) return Qt::TextColorRole;
  else if (name == "decoration"    ) return Qt::DecorationRole;

  else if (name == "type"              ) return (int) CQBaseModel::Role::Type;
  else if (name == "type_values"       ) return (int) CQBaseModel::Role::TypeValues;
  else if (name == "min"               ) return (int) CQBaseModel::Role::Min;
  else if (name == "max"               ) return (int) CQBaseModel::Role::Max;
  else if (name == "raw_value"         ) return (int) CQBaseModel::Role::RawValue;
  else if (name == "intermediate_value") return (int) CQBaseModel::Role::IntermediateValue;
  else if (name == "cached_value"      ) return (int) CQBaseModel::Role::CachedValue;
  else if (name == "output_value"      ) return (int) CQBaseModel::Role::OutputValue;

  return -1;
}

}

//------

namespace CQChartsUtil {

std::vector<double> varToReals(const QVariant &var, bool &ok) {
  std::vector<double> reals;

  QString str;

  if (! variantToString(var, str))
    return reals;

  return stringToReals(str, ok);
}

std::vector<double> stringToReals(const QString &str, bool &ok) {
  std::vector<double> reals;

  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('{')) {
    QString str1;

    if (! parse.readBracedString(str1)) {
      ok = false;
      return reals;
    }

    return stringToReals(str1, ok);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    double r;

    if (! parse.readReal(&r))
      break;

    reals.push_back(r);
  }

  return reals;
}

}

//------

namespace CQChartsUtil {

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, int role, bool &ok) {
  ok = false;

  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return QVariant();

  int icolumn = column.column();

  if (icolumn < 0)
    return QVariant();

  QVariant var = model->headerData(icolumn, orientation, role);

  ok = var.isValid();

  return var;
}

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, bool &ok) {
  return modelHeaderValue(model, column, orientation, column.role(Qt::DisplayRole), ok);
}

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, role, ok);
}

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok) {
  ok = false;

  if (! column.isValid())
    return "";

  QVariant var = modelHeaderValue(model, column, orient, role, ok);
  if (! var.isValid()) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, bool &ok) {
  return modelHeaderString(model, column, orient, column.role(Qt::DisplayRole), ok);
}

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, role, ok);
}

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orient, const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  return model->setHeaderData(column.column(), orient, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orientation, const QVariant &var) {
  return setModelHeaderValue(model, column, orientation, var, column.role(Qt::EditRole));
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var, int role) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, column.role(Qt::EditRole));
}

//--

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  QModelIndex ind = model->index(row, column.column(), QModelIndex());

  return model->setData(ind, var, role);
}

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var) {
  return setModelValue(model, row, column, var, column.role(Qt::EditRole));
}

//--

QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  if (! ind.isValid()) {
    ok = false;

    return QVariant();
  }

  QVariant var = model->data(ind, role);

  ok = var.isValid();

  return var;
}

QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, Qt::EditRole, ok);

  if (! ok)
    var = modelValue(model, ind, Qt::DisplayRole, ok);

  return var;
}

QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent,
                    int role, bool &ok) {
  if (! column.isValid()) {
    ok = false;

    return QVariant();
  }

  if      (column.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, column.column(), parent);

    return modelValue(model, ind, role, ok);
  }
  else if (column.type() == CQChartsColumn::Type::DATA_INDEX) {
    QModelIndex ind = model->index(row, column.column(), parent);

    QVariant var = modelValue(model, ind, role, ok);

    CQBaseModel::Type  columnType;
    CQChartsNameValues nameValues;

    if (! columnValueType(charts, model, column.column(), columnType, nameValues))
      return var;

    CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

    CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    return typeData->indexVar(var, column.index());
  }
  else if (column.type() == CQChartsColumn::Type::VHEADER) {
    QVariant var = model->headerData(row, Qt::Vertical, role);

    ok = var.isValid();

    return var;
  }
  else if (column.type() == CQChartsColumn::Type::GROUP) {
    QVariant var = model->headerData(row, Qt::Vertical, (int) CQBaseModel::Role::Group);

    ok = var.isValid();

    return var;
  }
  else if (column.type() == CQChartsColumn::Type::EXPR) {
    QVariant var;

    ok = CQChartsEvalInst->evalExpr(row, column.expr(), var);

    return var;
  }
  else {
    ok = false;

    return QVariant();
  }
}

QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QVariant var;

  if (! column.hasRole()) {
    var = modelValue(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      var = modelValue(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    var = modelValue(charts, model, row, column, parent, column.role(), ok);

  return var;
}

//---

QString modelString(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QString str = modelString(model, ind, Qt::EditRole, ok);

  if (! ok)
    str = modelString(model, ind, Qt::DisplayRole, ok);

  return str;
}

QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = variantToString(var, str);
  assert(rc);

  return str;
}

QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QString str;

  if (! column.hasRole()) {
    str = modelString(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      str = modelString(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    str = modelString(charts, model, row, column, parent, column.role(), ok);

  return str;
}

QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok) {
  QString s = modelString(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(charts, model, row1, column, parent1, role, ok);
    }
  }

  return s;
}

QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QString s = modelString(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(charts, model, row1, column, parent1, ok);
    }
  }

  return s;
}

//---

double modelReal(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0.0;

  return toReal(var, ok);
}

double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  double r = modelReal(model, ind, Qt::EditRole, ok);

  if (! ok)
    r = modelReal(model, ind, Qt::DisplayRole, ok);

  return r;
}

double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return 0.0;

  return toReal(var, ok);
}

double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  double r;

  if (! column.hasRole()) {
    r = modelReal(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      r = modelReal(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    r = modelReal(charts, model, row, column, parent, column.role(), ok);

  return r;
}

double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  double r = modelReal(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(charts, model, row1, column, parent1, role, ok);
    }
  }

  return r;
}

double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  double r = modelReal(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(charts, model, row1, column, parent1, ok);
    }
  }

  return r;
}

//---

long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0;

  return toInt(var, ok);
}

long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  long l = modelInteger(model, ind, Qt::EditRole, ok);

  if (! ok)
    l = modelInteger(model, ind, Qt::DisplayRole, ok);

  return l;
}

long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return 0;

  return toInt(var, ok);
}

long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  long l;

  if (! column.hasRole()) {
    l = modelInteger(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      l = modelInteger(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    l = modelInteger(charts, model, row, column, parent, column.role(), ok);

  return l;
}

long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent,
                      int role, bool &ok) {
  long l = modelInteger(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(charts, model, row1, column, parent1, role, ok);
    }
  }

  return l;
}

long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  long l = modelInteger(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(charts, model, row1, column, parent1, ok);
    }
  }

  return l;
}

//---

CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return CQChartsColor();

  if (isColor(var))
    return CQChartsColor(var.value<QColor>());

  CQChartsColor color;

  if (isReal(var)) {
    double r;

    if (toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsUtil::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  CQChartsColor c = modelColor(model, ind, Qt::EditRole, ok);

  if (! ok)
    c = modelColor(model, ind, Qt::DisplayRole, ok);

  return c;
}

CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return CQChartsColor();

  if (isColor(var))
    return CQChartsColor(var.value<QColor>());

  CQChartsColor color;

  if (isReal(var)) {
    double r;

    if (toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsUtil::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  CQChartsColor c;

  if (! column.hasRole()) {
    c = modelColor(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      c = modelColor(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    c = modelColor(charts, model, row, column, parent, column.role(), ok);

  return c;
}

}

//------

namespace CQChartsUtil {

bool stringToPolygons(const QString &str, std::vector<QPolygonF> &polygons) {
  CQStrParse parse(str);

  parse.skipSpace();

  int pos = parse.getPos();

  bool braced = false;

  if (parse.isChar('{')) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('{')) {
      braced = true;
    }
    else
      parse.setPos(pos);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString polyStr;

    if (! parse.readBracedString(polyStr))
      return false;

    QPolygonF poly;

    if (! stringToPolygon(polyStr, poly))
      return false;

    polygons.push_back(poly);

    parse.skipSpace();

    if (braced && parse.isChar('}')) {
      parse.skipChar();

      parse.skipSpace();

      break;
    }
  }

  return true;
}

bool stringToPolygon(const QString &str, QPolygonF &poly) {
  CQStrParse parse(str);

  parse.skipSpace();

  int pos = parse.getPos();

  bool braced = false;

  if (parse.isChar('{')) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('{')) {
      braced = true;
    }
    else
      parse.setPos(pos);
  }

  while (! parse.eof()) {
    parse.skipSpace();

    if (! parse.isChar('{'))
      return false;

    QString pointStr;

    if (! parse.readBracedString(pointStr))
      return false;

    QPointF point;

    if (! stringToPoint(pointStr, point))
      return false;

    poly.push_back(point);

    parse.skipSpace();

    if (braced && parse.isChar('}')) {
      parse.skipChar();

      parse.skipSpace();

      break;
    }
  }

  return true;
}

bool stringToRect(const QString &str, QRectF &rect) {
  CQStrParse parse(str);

  // read x1 y1 x2 y2 strings
  // TODO: skip braces, commas ...

  parse.skipSpace();

  QString x1str, y1str, x2str, y2str;

  if (! parse.readNonSpace(x1str))
    return false;

  parse.skipSpace();

  if (! parse.readNonSpace(y1str))
    return false;

  parse.skipSpace();

  if (! parse.readNonSpace(x2str))
    return false;

  parse.skipSpace();

  if (! parse.readNonSpace(y2str))
    return false;

  parse.skipSpace();

  // TODO: check for extra characters

  //---

  // get x1 y1 x2 y2 values
  double x1, y1, x2, y2;

  if (! CQChartsUtil::toReal(x1str, x1)) return false;
  if (! CQChartsUtil::toReal(y1str, y1)) return false;
  if (! CQChartsUtil::toReal(x2str, x2)) return false;
  if (! CQChartsUtil::toReal(y2str, y2)) return false;

  //---

  rect = QRectF(x1, y1, x2 - x1, y2 - y1);

  return true;
}

bool stringToPoint(const QString &str, QPointF &point) {
  CQStrParse parse(str);

  // read x y strings
  // TODO: skip braces, commas ...

  parse.skipSpace();

  QString xstr, ystr;

  if (! parse.readNonSpace(xstr))
    return false;

  parse.skipSpace();

  if (! parse.readNonSpace(ystr))
    return false;

  parse.skipSpace();

  // TODO: check for extra characters

  //---

  // get x y values
  double x, y;

  if (! CQChartsUtil::toReal(xstr, x)) return false;
  if (! CQChartsUtil::toReal(ystr, y)) return false;

  //---

  point = QPointF(x, y);

  return true;
}

}

//------

namespace CQChartsUtil {

QString pathToString(const CQChartsPath &path) {
  return path.toString();
}

bool stringToPath(const QString &str, CQChartsPath &path) {
  path = CQChartsPath();

  path.fromString(str);

  return true;
}

}

//------

namespace CQChartsUtil {

QString styleToString(const CQChartsStyle &style) {
  return style.toString();
}

bool stringToStyle(const QString &str, CQChartsStyle &style) {
  style = CQChartsStyle();

  style.fromString(str);

  return true;
}

}

//------

namespace CQChartsUtil {

QString timeToString(const QString &fmt, double r) {
  static char buffer[512];

  time_t t(r);

  struct tm *tm1 = localtime(&t);

  if (! tm1)
    return "<no_time>";

  (void) strftime(buffer, 512, fmt.toLatin1().constData(), tm1);

  return buffer;
}

bool stringToTime(const QString &fmt, const QString &str, double &t) {
  struct tm tm1; memset(&tm1, 0, sizeof(tm));

  char *p = strptime(str.toLatin1().constData(), fmt.toLatin1().constData(), &tm1);

  if (! p)
    return false;

  t = mktime(&tm1);

  return true;
}

}

//------

namespace CQChartsUtil {

bool
formatStringInRect(const QString &str, const QFont &font, const QRectF &rect, QStringList &strs) {
  auto addStr = [&](const QString &str) {
    assert(str.length());
    strs.push_back(str);
  };

  //---

  QString sstr = str.simplified();

  if (! sstr.length()) { // empty
    addStr(sstr);
    return false;
  }

  //---

  QFontMetricsF fm(font);

  double w = fm.width(sstr);

  double dw = (rect.width() - w);

  if (dw > 0 || isZero(dw)) { // fits
    addStr(sstr);
    return false;
  }

  double h = fm.height();

  double dh = (rect.height() - h);

  if (dh < 0 || isZero(dh)) { // rect can only fit single line of text (TODO: factor)
    addStr(sstr);
    return false;
  }

  //---

  // get potential split points
  std::vector<int> splits;

  findStringSplits1(sstr, splits);

  if (splits.empty()) {
    findStringSplits2(sstr, splits);

    if (splits.empty())
      findStringSplits3(sstr, splits);

    if (splits.empty()) {
      addStr(sstr);
      return false;
    }
  }

  //---

  // get split closest to middle
  int target   = sstr.length()/2;
  int bestDist = target*2 + 1;
  int bestInd  = -1;

  for (std::size_t i = 0; i < splits.size(); ++i) {
    int dist = std::abs(splits[i] - target);

    if (bestInd < 0 || dist < bestDist) {
      bestDist = dist;
      bestInd  = i;
    }
  }

  if (bestInd < 0) {
    addStr(sstr);
    return false;
  }

  //---

  // split at best and measure
  int split = splits[bestInd];

  QString str1 = sstr.mid(0, split).simplified();
  QString str2 = sstr.mid(split   ).simplified();

  double w1 = fm.width(str1);
  double w2 = fm.width(str2);

  // both fit so we are done
  if (w1 <= rect.width() && w2 <= rect.width()) {
    addStr(str1);
    addStr(str2);

    return true;
  }

  //---

  // if one or both still wider then divide rect and refit
  if      (w1 > rect.width() && w2 > rect.width()) {
    double splitHeight = rect.height()/2.0;

    QRect rect1(rect.left(), rect.top(), rect.width(), splitHeight);
    QRect rect2(rect.left(), rect.top() + splitHeight, rect.width(), rect.height() - splitHeight);

    QStringList strs1, strs2;

    formatStringInRect(str1, font, rect1, strs1);
    formatStringInRect(str2, font, rect2, strs2);

    strs += strs1;
    strs += strs2;
  }
  else if (w1 > rect.width()) {
    double splitHeight = rect.height() - h;

    QRect rect1(rect.left(), rect.top(), rect.width(), splitHeight);

    QStringList strs1;

    formatStringInRect(str1, font, rect1, strs1);

    strs += strs1;

    addStr(str2);
  }
  else {
    double splitHeight = rect.height() - h;

    QRect rect2(rect.left(), rect.top() + h, rect.width(), splitHeight);

    QStringList strs2;

    formatStringInRect(str2, font, rect2, strs2);

    addStr(str1);

    strs += strs2;
  }

  return true;
}

}

//------

namespace CQChartsUtil {

bool isValidModelColumn(QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name) {
  int role = Qt::DisplayRole;

  for (int icolumn = 0; icolumn < model->columnCount(); ++icolumn) {
    QVariant var = model->headerData(icolumn, Qt::Horizontal, role);

    if (! var.isValid())
      continue;

    //QString name1 = CQChartsUtil::toString(var, rc);

    QString name1;

    bool rc = variantToString(var, name1);
    assert(rc);

    if (name == name1)
      return icolumn;
  }

  //---

  bool ok;

  int column = name.toInt(&ok);

  if (ok)
    return column;

  return -1;
}

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column) {
  if (! str.length())
    return false;

  //---

  // handle special column syntax or expression
  CQChartsColumn column1(str);

  if (column1.isValid()) {
    column = column1;

    return true;
  }

  //---

  // get column from name (exact match)
  int icolumn = modelColumnNameToInd(model, str);

  if (icolumn >= 0) {
    column = CQChartsColumn(icolumn);
    return true;
  }

  //---

  // check for column sub value
  QString str1 = str.simplified();

  if (str1.right(1) == "]") {
    int pos = str1.lastIndexOf('[');

    QString lhs = str1.mid(0, pos);
    QString rhs = str1.mid(pos + 1, str1.length() - pos - 2);

    CQChartsColumn column1;

    if (! stringToColumn(model, lhs, column1))
      return false;

    if (column1.type() != CQChartsColumn::Type::DATA)
      return false;

    column = CQChartsColumn(CQChartsColumn::Type::DATA_INDEX,
                            column1.column(), rhs, column1.role());

    return true;
  }

  //---

  // fail
  return false;
}

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns) {
  bool rc = true;

  QStringList strs = str.split(" ", QString::SkipEmptyParts);

  for (int i = 0; i < strs.length(); ++i) {
    const QString &str = strs[i];

    int pos = str.indexOf('~');

    if (pos >= 0) {
      QString lhs = str.mid(0, pos);
      QString rhs = str.mid(pos + 1);

      CQChartsColumn c1, c2;

      if (stringToColumn(model, lhs, c1) && stringToColumn(model, rhs, c2)) {
        if (c1.hasColumn() && c2.hasColumn()) {
          int col1 = c1.column();
          int col2 = c2.column();

          if (col1 > col2)
            std::swap(col1, col2);

          for (int c = col1; c <= col2; ++c)
            columns.push_back(c);
        }
        else
          rc = false;
      }
      else
        rc = false;
    }
    else {
      CQChartsColumn c;

      if (! stringToColumn(model, str, c))
        rc = false;

      columns.push_back(c);
    }
  }

  return rc;
}

}

//------

namespace CQChartsUtil {

void
exportModel(QAbstractItemModel *model, CQBaseModel::DataType type, bool hheader,
            bool vheader, std::ostream &os) {
  if      (type == CQBaseModel::DataType::CSV) {
    CQCsvModel csv;

    csv.setFirstLineHeader  (hheader);
    csv.setFirstColumnHeader(vheader);

    csv.save(model, os);
  }
  else if (type == CQBaseModel::DataType::TSV) {
    CQTsvModel tsv;

    tsv.setFirstLineHeader  (hheader);
    tsv.setFirstColumnHeader(vheader);

    tsv.save(model, os);
  }
  else {
    assert(false);
  }
}

}

//------

namespace CQChartsUtil {

void
processAddExpression(QAbstractItemModel *model, const QString &exprStr)
{
  CQChartsExprModel *exprModel = CQChartsUtil::getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return;
  }

  int column;

  exprModel->addExtraColumn(exprStr, column);
}

int
processExpression(QAbstractItemModel *model, const QString &exprStr)
{
  CQChartsExprModel *exprModel = CQChartsUtil::getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return -1;
  }

  CQChartsExprModel::Function function { CQChartsExprModel::Function::EVAL };
  int                         icolumn  { -1 };
  QString                     expr;

  if (! exprModel->decodeExpressionFn(exprStr, function, icolumn, expr)) {
    errorMsg("Invalid expression '" + exprStr + "'");
    return -1;
  }

  CQChartsColumn column(icolumn);

  return processExpression(model, function, column, expr);
}

int
processExpression(QAbstractItemModel *model, CQChartsExprModel::Function function,
                  const CQChartsColumn &column, const QString &expr)
{
  CQChartsExprModel *exprModel = CQChartsUtil::getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return -1;
  }

  // add column <expr>
  if      (function == CQChartsExprModel::Function::ADD) {
    int column1;

    if (! exprModel->addExtraColumn(expr, column1))
      return -1;

    return column1;
  }
  // delete column <n>
  else if (function == CQChartsExprModel::Function::DELETE) {
    int icolumn = column.column();

    if (icolumn < 0) {
      errorMsg("Inavlid column");
      return -1;
    }

    bool rc = exprModel->removeExtraColumn(icolumn);

    if (! rc) {
      errorMsg(QString("Failed to delete column '%1'").arg(icolumn));
      return -1;
    }

    return icolumn;
  }
  // modify column <n>:<expr>
  else if (function == CQChartsExprModel::Function::ASSIGN) {
    int icolumn = column.column();

    if (icolumn < 0) {
      errorMsg("Inavlid column");
      return -1;
    }

    if (! exprModel->assignExtraColumn(icolumn, expr))
      return -1;

    return icolumn;
  }

  else {
    exprModel->processExpr(expr);

    return -1;
  }
}

CQChartsExprModel *
getExprModel(QAbstractItemModel *model) {
  CQChartsExprModel *exprModel = qobject_cast<CQChartsExprModel *>(model);

  if (! exprModel) {
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

    if (proxyModel)
      exprModel = qobject_cast<CQChartsExprModel *>(proxyModel->sourceModel());
  }

  return exprModel;
}

CQDataModel *
getDataModel(QAbstractItemModel *model) {
  CQChartsModelFilter *modelFilter = dynamic_cast<CQChartsModelFilter *>(model);
  if (! modelFilter) return nullptr;

  CQDataModel *dataModel = dynamic_cast<CQDataModel *>(modelFilter->baseModel());
  if (! dataModel) return nullptr;

  return dataModel;
}

}

//------

namespace CQChartsUtil {

bool getBoolEnv(const char *name, bool def) {
  char *env = getenv(name);

  if (! env)
    return def;

  if      (strcmp(env, "0") == 0 || strcmp(env, "false") == 0 || strcmp(env, "no" ) == 0)
    return false;
  else if (strcmp(env, "1") == 0 || strcmp(env, "true" ) == 0 || strcmp(env, "yes") == 0)
    return true;

  assert(false);

  return true;
}

}
