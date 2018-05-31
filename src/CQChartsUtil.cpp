#include <CQChartsUtil.h>
#include <CQChartsColumnType.h>
#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQCharts.h>

#include <CQUtil.h>
#include <CExpr.h>
#include <CQStrParse.h>
#include <QFontMetricsF>

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

  int nc = model->columnCount(QModelIndex());

  visitor.init(nc);

  QModelIndex parent;

  (void) visitModelIndex(model, parent, visitor);

  visitor.term();

  return true;
}

bool visitModel(QAbstractItemModel *model, const QModelIndex &parent, int r,
                CQChartsModelVisitor &visitor) {
  if (! model)
    return false;

  int nc = model->columnCount(QModelIndex());

  visitor.init(nc);

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
  if (column.type() != CQChartsColumn::Type::DATA) {
    // TODO: for custom expression should determine expression result type (if possible)
    columnType = CQBaseModel::Type::STRING;
    return true;
  }

  //---

  // get column number and validate
  assert(model);

  int icolumn = column.column();

  if (icolumn < 0 || icolumn >= model->columnCount()) {
    columnType = CQBaseModel::Type::NONE;
    return false;
  }

  //---

  // use defined column type if available
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  if (columnTypeMgr->getModelColumnType(model, column, columnType, nameValues))
    return true;

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

// use column format string to format a value as data (used by axis)
//  TODO: separate format string from column type to remove dependence
bool
formatColumnTypeValue(CQCharts *charts, const QString &typeStr, double value, QString &str) {
  CQChartsNameValues nameValues;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  if (! typeData)
    return false;

  QVariant var = typeData->dataName(value, nameValues);

  if (! var.isValid())
    return false;

  variantToString(var, str);

  return true;
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

  QVariant var = typeData->dataName(value, nameValues);

  if (! var.isValid())
    return false;

  variantToString(var, str);

  return true;
}

// use column type details to format an internal model value (variant) to a display value
QVariant
columnDisplayData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                  const QVariant &var) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  return columnTypeMgr->getDisplayData(model, column, var);
}

// use column type details to format an internal model value (variant) to a editable value
QVariant
columnUserData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  return columnTypeMgr->getUserData(model, column, var);
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

// set type string for column (type name and name values)
bool
setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                 const CQChartsColumn &column, const QString &typeStr) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // decode to type name and name values
  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  if (! typeData)
    return false;

  // store in model
  CQBaseModel::Type columnType = typeData->type();

  return columnTypeMgr->setModelColumnType(model, column, columnType, nameValues);
}

}

//------

namespace CQChartsUtil {

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

QString toString(const std::vector<int> &columns) {
  QString str;

  for (std::size_t i = 0; i < columns.size(); ++i) {
    if (str.length())
      str += " ";

    str += QString("%1").arg(columns[i]);
  }

  return str;
}

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
  else {
    assert(false);

    return false;
  }

  return true;
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

namespace CQChartsUtil {

bool evalExpr(int row, const QString &exprStr, double &r) {
  CExpr expr;

  expr.createVariable("x", expr.createRealValue(row));

  CExprValuePtr value;

  if (! expr.evaluateExpression(exprStr.toStdString(), value))
    return false;

  r = 0.0;

  if (value->isRealValue())
    value->getRealValue(r);

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
