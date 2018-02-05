#include <CQChartsUtil.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <QFontMetrics>

namespace CQChartsUtil {

bool visitModel(QAbstractItemModel *model, ModelVisitor &visitor) {
  visitor.init();

  if (! model)
    return false;

  QModelIndex parent;

  visitModelIndex(model, parent, visitor);

  visitor.term();

  return true;
}

void visitModelIndex(QAbstractItemModel *model, const QModelIndex &parent, ModelVisitor &visitor) {
  int nr = model->rowCount(parent);

  visitor.setNumRows(nr);

  for (int r = 0; r < nr; ++r) {
    QModelIndex ind1 = model->index(r, 0, parent);

    if (model->rowCount(ind1) > 0) {
      visitModelIndex(model, ind1, visitor);
    }
    else {
      ModelVisitor::State preState = visitor.preVisit(model, parent, r);
      if (preState == ModelVisitor::State::TERMINATE) break;
      if (preState == ModelVisitor::State::SKIP     ) continue;

      ModelVisitor::State state = visitor.visit(model, parent, r);
      if (state == ModelVisitor::State::TERMINATE) break;
      if (state == ModelVisitor::State::SKIP     ) continue;

      visitor.step();
    }
  }
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

ModelColumnDetails::
ModelColumnDetails(CQCharts *charts, QAbstractItemModel *model, int column) :
 charts_(charts), model_(model), column_(column)
{
}

QString
ModelColumnDetails::
typeName() const
{
  if (! initialized_)
    (void) const_cast<ModelColumnDetails *>(this)->init();

  return typeName_;
}

QVariant
ModelColumnDetails::
minValue() const
{
  if (! initialized_)
    (void) const_cast<ModelColumnDetails *>(this)->init();

  return minValue_;
}

QVariant
ModelColumnDetails::
maxValue() const
{
  if (! initialized_)
    (void) const_cast<ModelColumnDetails *>(this)->init();

  return maxValue_;
}

int
ModelColumnDetails::
numRows() const
{
  if (! initialized_)
    (void) const_cast<ModelColumnDetails *>(this)->init();

  return numRows_;
}

bool
ModelColumnDetails::
init()
{
  initialized_ = true;

  //---

  if (! model_)
    return false;

  int numColumns = model_->columnCount();

  if (column_ < 0 || column_ >= numColumns)
    return false;

  //---

  class DetailVisitor : public ModelVisitor {
   public:
    DetailVisitor(ModelColumnDetails *details) :
     details_(details) {
      CQChartsColumnTypeMgr *columnTypeMgr = details_->charts()->columnTypeMgr();

      CQChartsNameValues nameValues;

      if (columnTypeMgr->getModelColumnType(details_->model(), details_->column(),
                                            type_, nameValues)) {
        CQChartsColumnType *columnType = columnTypeMgr->getType(type_);

        if (columnType)
          typeName_ = columnType->name();
      }
      else {
        type_ = CQBaseModel::Type::STRING;
      }
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QModelIndex ind = model->index(row, details_->column(), parent);

      if      (type_ == CQBaseModel::Type::INTEGER) {
        bool ok;

        long i = CQChartsUtil::modelInteger(model, ind, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(int(i)))
          return State::SKIP;

        imin_ = (! iset_ ? i : std::min(imin_, i));
        imax_ = (! iset_ ? i : std::max(imax_, i));

        iset_ = true;
      }
      else if (type_ == CQBaseModel::Type::REAL) {
        bool ok;

        double r = CQChartsUtil::modelReal(model, ind, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(r))
          return State::SKIP;

        rmin_ = (! rset_ ? r : std::min(rmin_, r));
        rmax_ = (! rset_ ? r : std::max(rmax_, r));

        rset_ = true;
      }
      else {
        bool ok;

        QString s = CQChartsUtil::modelString(model, ind, ok);
        if (! ok) return State::SKIP;

        if (! details_->checkRow(s))
          return State::SKIP;

        smin_ = (! sset_ ? s : std::min(smin_, s));
        smax_ = (! sset_ ? s : std::max(smax_, s));

        sset_ = true;
      }

      return State::OK;
    }

    CQBaseModel::Type type() const { return type_; }

    QString typeName() const { return typeName_; }

    QVariant minValue() const {
      if      (type_ == CQBaseModel::Type::INTEGER) return QVariant(int(imin_));
      else if (type_ == CQBaseModel::Type::REAL   ) return QVariant(rmin_);
      else                                          return QVariant(smin_);
    }

    QVariant maxValue() const {
      if      (type_ == CQBaseModel::Type::INTEGER) return QVariant(int(imax_));
      else if (type_ == CQBaseModel::Type::REAL   ) return QVariant(rmax_);
      else                                          return QVariant(smax_);
    }

   private:
    ModelColumnDetails *details_ { nullptr };
    CQBaseModel::Type   type_;
    QString             typeName_;
    long                imin_ { 0 }; long    imax_ { 0 }; bool iset_ { false };
    double              rmin_ { 0 }; double  rmax_ { 0 }; bool rset_ { false };
    QString             smin_      ; QString smax_      ; bool sset_ { false };
  };

  //---

  DetailVisitor detailVisitor(this);

  visitModel(model_, detailVisitor);

  //---

  typeName_ = detailVisitor.typeName();
  minValue_ = detailVisitor.minValue();
  maxValue_ = detailVisitor.maxValue();
  numRows_  = detailVisitor.numRows();

  return true;
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

      if (i == 0 || i >= len) // don't break if at start or end
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

bool
formatStringInRect(const QString &str, const QFont &font, const QRectF &rect, QStringList &strs)
{
  QString sstr = str.simplified();

  if (! sstr.length()) { // empty
    strs.push_back(sstr);
    return false;
  }

  //---

  QFontMetricsF fm(font);

  double w = fm.width(sstr);

  if (w < rect.width()) { // fits
    strs.push_back(sstr);
    return false;
  }

  double h = fm.height();

  if (h >= rect.height()) { // rect can only fit single line of text
    strs.push_back(sstr);
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
      strs.push_back(sstr);
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
    strs.push_back(sstr);
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
    strs.push_back(str1);
    strs.push_back(str2);

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

    strs.push_back(str2);
  }
  else {
    double splitHeight = rect.height() - h;

    QRect rect2(rect.left(), rect.top() + h, rect.width(), splitHeight);

    QStringList strs2;

    formatStringInRect(str2, font, rect2, strs2);

    strs.push_back(str1);

    strs += strs2;
  }

  return true;
}

}
