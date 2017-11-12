#include <CQExprModel.h>
#include <CQBaseModel.h>
#include <CQStrParse.h>
#include <CExpr.h>
#include <COSNaN.h>

class CQExprModelFn : public CExprFunctionObj {
 public:
  CQExprModelFn(CQExprModel *model) :
   model_(model) {
  }

  bool checkColumn(int col) const {
    if (col < 0 || col >= model_->columnCount()) return false;

    return true;
  }

  bool checkIndex(int row, int col) const {
    if (row < 0 || row >= model_->rowCount   ()) return false;
    if (col < 0 || col >= model_->columnCount()) return false;

    return true;
  }

  CExprValuePtr variantToValue(CExpr *expr, const QVariant &var) const {
    if      (var.type() == QVariant::Double)
      return expr->createRealValue(var.toDouble());
    else if (var.type() == QVariant::Int)
      return expr->createIntegerValue((long) var.toInt());
    else
      return expr->createStringValue(var.toString().toStdString());
  }

  QVariant valueToVariant(CExpr *, const CExprValuePtr &value) const {
    if      (value->isRealValue()) {
      double r = 0.0;
      value->getRealValue(r);
      return QVariant(r);
    }
    else if (value->isRealValue()) {
      long i = 0;
      value->getIntegerValue(i);
      return QVariant((int) i);
    }
    else {
      std::string s;
      value->getStringValue(s);
      return QVariant(s.c_str());
    }
  }

 protected:
  CQExprModel *model_ { nullptr };
};

//---

// column(), column(col), column(col,row) : get column value
class CQExprModelColumnFn : public CQExprModelFn {
 public:
  CQExprModelColumnFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    if      (values.size() == 0) {
      return expr->createIntegerValue(col);
    }
    else if (values.size() == 1) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();
    }
    else if (values.size() == 2) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();

      if (! values[1]->getIntegerValue(row))
        return CExprValuePtr();
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkIndex(row, col))
      return CExprValuePtr();

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    return variantToValue(expr, var);
  }
};

//---

// setColumn(value), setColumn(col,value), setColumn(col,row,value) : set column value
class CQExprModelSetColumnFn : public CQExprModelFn {
 public:
  CQExprModelSetColumnFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    QVariant var;

    if      (values.size() == 1) {
      var = valueToVariant(expr, values[0]);
    }
    else if (values.size() == 2) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();

      var = valueToVariant(expr, values[1]);
    }
    else if (values.size() == 3) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();

      if (! values[1]->getIntegerValue(row))
        return CExprValuePtr();

      var = valueToVariant(expr, values[2]);
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkIndex(row, col))
      return CExprValuePtr();

    QModelIndex ind = model_->index(row, col, QModelIndex());

    bool b = model_->setData(ind, var, Qt::DisplayRole);

    return expr->createBooleanValue(b);
  }
};

//---

// row(), row(max), row(min,max)
class CQExprModelRowFn : public CQExprModelFn {
 public:
  CQExprModelRowFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow();

    if (values.size() == 0)
      return expr->createIntegerValue(row);

    double min = 0.0, max = 1.0;

    if      (values.size() == 1) {
      if (! values[0]->getRealValue(max))
        return CExprValuePtr();
    }
    else if (values.size() == 2) {
      if (! values[0]->getRealValue(min))
        return CExprValuePtr();

      if (! values[1]->getRealValue(max))
        return CExprValuePtr();
    }
    else {
      return CExprValuePtr();
    }

    //---

    // scale row number to 0->1
    double x = 0.0;

    if (model_->rowCount())
      x = (1.0*row)/model_->rowCount();

    // map 0->1 -> min->max
    double x1 = x*(max - min) + min;

    return expr->createRealValue(x1);
  }
};

//---

// setRow(value), setRow(row,value), setRow(row,col,value) : set row value
class CQExprModelSetRowFn : public CQExprModelFn {
 public:
  CQExprModelSetRowFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    QVariant var;

    if      (values.size() == 1) {
      var = valueToVariant(expr, values[0]);
    }
    else if (values.size() == 2) {
      if (! values[0]->getIntegerValue(row))
        return CExprValuePtr();

      var = valueToVariant(expr, values[1]);
    }
    else if (values.size() == 3) {
      if (! values[0]->getIntegerValue(row))
        return CExprValuePtr();

      if (! values[1]->getIntegerValue(col))
        return CExprValuePtr();

      var = valueToVariant(expr, values[2]);
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkIndex(row, col))
      return CExprValuePtr();

    QModelIndex ind = model_->index(row, col, QModelIndex());

    bool b = model_->setData(ind, var, Qt::DisplayRole);

    return expr->createBooleanValue(b);
  }
};

//---

// header(), header(col)
class CQExprModelHeaderFn : public CQExprModelFn {
 public:
  CQExprModelHeaderFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long col = model_->currentCol();

    if      (values.size() == 0) {
    }
    else if (values.size() == 1) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkColumn(col))
      return CExprValuePtr();

    QVariant var = model_->headerData(col, Qt::Horizontal, Qt::DisplayRole);

    return variantToValue(expr, var);
  }
};

//---

// setHeader(s), setHeader(col,s)
class CQExprModelSetHeaderFn : public CQExprModelFn {
 public:
  CQExprModelSetHeaderFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long col = model_->currentCol();

    QVariant var;

    if      (values.size() == 1) {
      var = valueToVariant(expr, values[0]);
    }
    else if (values.size() == 2) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();

      var = valueToVariant(expr, values[1]);
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkColumn(col))
      return CExprValuePtr();

    bool b = model_->setHeaderData(col, Qt::Horizontal, var, Qt::DisplayRole);

    return expr->createBooleanValue(b);
  }
};

//---

// type(), type(col)
class CQExprModelTypeFn : public CQExprModelFn {
 public:
  CQExprModelTypeFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long col = model_->currentCol();

    if      (values.size() == 0) {
    }
    else if (values.size() == 1) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkColumn(col))
      return CExprValuePtr();

    QVariant var = model_->headerData(col, Qt::Horizontal,
      static_cast<int>(CQBaseModel::Role::Type));

    QString typeName = CQBaseModel::typeName((CQBaseModel::Type) var.toInt());

    return expr->createStringValue(typeName.toStdString());
  }
};

//---

// setType(s), setType(col,s)
class CQExprModelSetTypeFn : public CQExprModelFn {
 public:
  CQExprModelSetTypeFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long col = model_->currentCol();

    QVariant var;

    if      (values.size() == 1) {
      var = valueToVariant(expr, values[0]);
    }
    else if (values.size() == 2) {
      if (! values[0]->getIntegerValue(col))
        return CExprValuePtr();

      var = valueToVariant(expr, values[1]);
    }
    else {
      return CExprValuePtr();
    }

    //---

    if (! checkColumn(col))
      return CExprValuePtr();

    CQBaseModel::Type type = CQBaseModel::nameType(var.toString());

    QVariant typeVar(static_cast<int>(type));

    bool b = model_->setHeaderData(col, Qt::Horizontal, typeVar,
               static_cast<int>(CQBaseModel::Role::Type));

    return expr->createBooleanValue(b);
  }
};

//---

// bucket(col), bucket(col,min,max)
class CQExprModelBucketFn : public CQExprModelFn {
 public:
  CQExprModelBucketFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    if (values.size() < 1)
      return expr->createIntegerValue(0);

    //---

    long col = 0, row = model_->currentRow();

    if (! values[0]->getIntegerValue(col))
      return expr->createIntegerValue(0);

    if (col < 0 || col >= model_->columnCount())
      return expr->createRealValue(0.0);

    //---

    if (! checkIndex(row, col))
      return CExprValuePtr();

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    if      (var.type() == QVariant::Double) {
      double value = var.toDouble();

      double minVal = 0;
      double maxVal = 1;
      long   scale  = 1;

      if      (values.size() == 2) {
        model_->columnRange(col, minVal, maxVal);

        if (! values[0]->getIntegerValue(scale))
          return expr->createIntegerValue(0);
      }
      else if (values.size() == 3) {
        if (! values[1]->getRealValue(minVal) ||
            ! values[2]->getRealValue(maxVal))
          return expr->createIntegerValue(0);
      }
      else {
        return expr->createIntegerValue(0);
      }

      double d = maxVal - minVal;

      int ind = 0;

      if (d) {
        double s = (value - minVal)/d;

        ind = int(s*scale);
      }

      return expr->createIntegerValue((long) ind);
    }
    else if (var.type() == QVariant::Int) {
      return expr->createIntegerValue((long) var.toInt());
    }
    else {
      QString str = var.toString();

      int ind = model_->columnStringBucket(col, str);

      return expr->createIntegerValue((long) ind);
    }
  }
};

//---

// norm(col)
class CQExprModelNormFn : public CQExprModelFn {
 public:
  CQExprModelNormFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    if (values.empty())
      return expr->createRealValue(0.0);

    long col = 0, row = model_->currentRow();

    if (! values[0]->getIntegerValue(col))
      return expr->createRealValue(0.0);

    if (col < 0 || col >= model_->columnCount())
      return expr->createRealValue(0.0);

    //---

    if (! checkIndex(row, col))
      return CExprValuePtr();

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    //---

    if      (var.type() == QVariant::Double) {
      double value = var.toDouble();

      double minVal = 0;
      double maxVal = 1;

      model_->columnRange(col, minVal, maxVal);

      double r;

      if (values.size() > 1 && values[1]->getRealValue(r))
        minVal = r;

      if (values.size() > 2 && values[2]->getRealValue(r))
        maxVal = r;

      double d = maxVal - minVal;
      double s = 0.0;

      if (d)
        s = (value - minVal)/d;

      return expr->createRealValue(s);
    }
    else if (var.type() == QVariant::Int) {
      int value = var.toInt();

      int minVal = 0;
      int maxVal = 1;

      model_->columnRange(col, minVal, maxVal);

      long i;

      if (values.size() > 1 && values[1]->getIntegerValue(i))
        minVal = i;

      if (values.size() > 2 && values[2]->getIntegerValue(i))
        maxVal = i;

      int    d = maxVal - minVal;
      double s = 0.0;

      if (d)
        s = double(value - minVal)/d;

      return expr->createRealValue(s);
    }
    else {
      return expr->createRealValue(0.0);
    }
  }
};

//------

class CQExprModelKeyFn : public CQExprModelFn {
 public:
  CQExprModelKeyFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    std::string str;

    for (std::size_t i = 0; i < values.size(); ++i) {
      std::string str1;

      (void) values[i]->getStringValue(str1);

      if (str.size())
        str += ":" + str1;
      else
        str = str1;
    }

    return expr->createStringValue(str);
  }
};

//------

CQExprModel::
CQExprModel(QAbstractItemModel *model) :
 model_(model)
{
  expr_ = new CExpr;

  expr_->createRealVariable("pi" , M_PI);
  expr_->createRealVariable("NaN", COSNaN::get_nan());

  expr_->addFunction("column"   , "...", new CQExprModelColumnFn   (this));
  expr_->addFunction("setColumn", "...", new CQExprModelSetColumnFn(this));
  expr_->addFunction("row"      , "...", new CQExprModelRowFn      (this));
  expr_->addFunction("setRow"   , "...", new CQExprModelSetRowFn   (this));
  expr_->addFunction("header"   , "...", new CQExprModelHeaderFn   (this));
  expr_->addFunction("setHeader", "...", new CQExprModelSetHeaderFn(this));
  expr_->addFunction("type"     , "...", new CQExprModelTypeFn     (this));
  expr_->addFunction("setType"  , "...", new CQExprModelSetTypeFn  (this));
  expr_->addFunction("bucket"   , "...", new CQExprModelBucketFn   (this));
  expr_->addFunction("norm"     , "...", new CQExprModelNormFn     (this));
  expr_->addFunction("key"      , "...", new CQExprModelKeyFn      (this));

  setSourceModel(model);
}

CQExprModel::
~CQExprModel()
{
  delete expr_;
}

void
CQExprModel::
addExtraColumn(const QString &expr)
{
  int nc = columnCount(QModelIndex());

  beginInsertColumns(QModelIndex(), nc, nc);

  extraColumns_.emplace_back(expr);

  endInsertColumns();
}

bool
CQExprModel::
removeExtraColumn(int column)
{
  int numNonExtra = columnCount() - numExtraColumns();

  int column1 = column - numNonExtra;

  if (column1 < 0 || column1 >= numExtraColumns())
    return false;

  beginRemoveColumns(QModelIndex(), column, column);

  for (int i = column1 + 1; i < numExtraColumns(); ++i)
    extraColumns_[i - 1] = extraColumns_[i];

  extraColumns_.pop_back();

  endRemoveColumns();

  return true;
}

bool
CQExprModel::
assignExtraColumn(int column, const QString &expr)
{
  int numNonExtra = columnCount() - numExtraColumns();

  int column1 = column - numNonExtra;

  if (column1 < 0 || column1 >= numExtraColumns())
    return false;

  ExtraColumn &extraColumn = extraColumns_[column1];

  extraColumn.values.resize(rowCount());

  for (int r = 0; r < rowCount(); ++r)
    extraColumn.values[r] = getExtraColumnValue(r, column1);

  extraColumn.expr = expr;

  extraColumn.variantMap.clear();

  QModelIndex index1 = index(0             , column, QModelIndex());
  QModelIndex index2 = index(rowCount() - 1, column, QModelIndex());

  emit dataChanged(index1, index2);

  return true;
}

void
CQExprModel::
processExpr(const QString &expr)
{
  QString expr1 = replaceNumericColumns(expr);

  for (int r = 0; r < rowCount(); ++r) {
    currentRow_ = r;

    CExprValuePtr value;

    if (! expr_->evaluateExpression(expr1.toStdString(), value))
      continue;
  }
}

int
CQExprModel::
columnStringBucket(int column, const QString &value) const
{
  ColumnData &columnData = columnDatas_[column];

  auto p = columnData.stringMap.find(value);

  if (p == columnData.stringMap.end())
    p = columnData.stringMap.insert(p, StringMap::value_type(value, columnData.stringMap.size()));

  return (*p).second;
}

bool
CQExprModel::
columnRange(int column, double &minVal, double &maxVal) const
{
  ColumnData &columnData = columnDatas_[column];

  if (columnData.rmin) {
    minVal = *columnData.rmin;
    maxVal = *columnData.rmax;

    return true;
  }

  for (int r = 0; r < rowCount(); ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    double value = var.toDouble();

    if (r == 0) {
      minVal = value;
      maxVal = value;
    }
    else {
      minVal = std::min(minVal, value);
      maxVal = std::max(maxVal, value);
    }
  }

  columnData.rmin = minVal;
  columnData.rmax = maxVal;

  return true;
}

bool
CQExprModel::
columnRange(int column, int &minVal, int &maxVal) const
{
  ColumnData &columnData = columnDatas_[column];

  if (columnData.imin) {
    minVal = *columnData.imin;
    maxVal = *columnData.imax;

    return true;
  }

  for (int r = 0; r < rowCount(); ++r) {
    QModelIndex ind = index(r, column, QModelIndex());

    QVariant var = data(ind, Qt::DisplayRole);

    int value = var.toInt();

    if (r == 0) {
      minVal = value;
      maxVal = value;
    }
    else {
      minVal = std::min(minVal, value);
      maxVal = std::max(maxVal, value);
    }
  }

  columnData.imin = minVal;
  columnData.imax = maxVal;

  return true;
}

QModelIndex
CQExprModel::
index(int row, int column, const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(sourceModel()->index(row, column, mapToSource(parent)));

  if (column < nc)
    return mapFromSource(sourceModel()->index(row, column, mapToSource(parent)));

  return createIndex(row, column, nullptr);
}

QModelIndex
CQExprModel::
parent(const QModelIndex &child) const
{
  return sourceModel()->parent(mapToSource(child));
}

int
CQExprModel::
rowCount(const QModelIndex &parent) const
{
  return sourceModel()->rowCount(mapToSource(parent));
}

int
CQExprModel::
columnCount(const QModelIndex &parent) const
{
  int nc = sourceModel()->columnCount(mapToSource(parent));

  return nc + numExtraColumns();
}

bool
CQExprModel::
hasChildren(const QModelIndex &parent) const
{
  return sourceModel()->hasChildren(mapToSource(parent));
}

QVariant
CQExprModel::
data(const QModelIndex &index, int role) const
{
  int nc = sourceModel()->columnCount(mapToSource(index));

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->data(mapToSource(index), role);

  //---

  int column = index.column() - nc;

  if (role == Qt::DisplayRole) {
    currentRow_ = index.row();
    currentCol_ = index.column();

    return getExtraColumnValue(currentRow_, column);
  }

  if (role == Qt::TextAlignmentRole) {
    CQExprModel *th = const_cast<CQExprModel *>(this);

    ExtraColumn &extraColumn = th->extraColumn(column);

    if (extraColumn.type == CQBaseModel::Type::INTEGER ||
        extraColumn.type == CQBaseModel::Type::REAL)
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  return QVariant();
}

QVariant
CQExprModel::
getExtraColumnValue(int row, int column) const
{
  CQExprModel *th = const_cast<CQExprModel *>(this);

  ExtraColumn &extraColumn = th->extraColumn(column);

  //---

  if (evaluating_ && ! extraColumn.values.empty()) {
    return extraColumn.values[row];
  }

  //---

  auto p = extraColumn.variantMap.find(row);

  if (p == extraColumn.variantMap.end()) {
    evaluating_ = true;

    QString expr = extraColumn.expr;

    int pos = expr.indexOf('=');

    if (pos >= 0) {
      extraColumn.header = expr.mid(0, pos).simplified();

      expr = expr.mid(pos + 1).simplified();
    }

    expr = replaceNumericColumns(expr);

    QVariant var;

    CExprValuePtr value;

    if (expr_->evaluateExpression(expr.toStdString(), value)) {
      if (value.isValid()) {
        std::string str;

        double real    = 0.0;
        long   integer = 0;

        if      (value->getRealValue(real)) {
          var = QVariant(real);

          bool isInt = CQBaseModel::isInteger(real);

          if      (extraColumn.type == CQBaseModel::Type::NONE) {
            if (CQBaseModel::isInteger(real))
              extraColumn.type = CQBaseModel::Type::INTEGER;
            else
              extraColumn.type = CQBaseModel::Type::REAL;
          }
          else if (extraColumn.type == CQBaseModel::Type::INTEGER) {
            if (! isInt)
              extraColumn.type = CQBaseModel::Type::REAL;
          }
          else if (extraColumn.type == CQBaseModel::Type::REAL) {
          }
        }
        else if (value->getIntegerValue(integer)) {
          if (extraColumn.type == CQBaseModel::Type::NONE)
            extraColumn.type = CQBaseModel::Type::INTEGER;
        }
        else if (value->getStringValue(str)) {
          var = QVariant(QString(str.c_str()));

          if (extraColumn.type == CQBaseModel::Type::NONE)
            extraColumn.type = CQBaseModel::Type::STRING;
        }
      }
    }

    if (! extraColumn.values.empty())
      extraColumn.values[row] = var;

    p = extraColumn.variantMap.insert(p, VariantMap::value_type(row, var));

    evaluating_ = false;
  }

  return (*p).second;
}

bool
CQExprModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  int nc = sourceModel()->columnCount(index);

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->setData(mapToSource(index), value, role);

  int column = index.column() - nc;

  if (column >= numExtraColumns())
    return false;

  ExtraColumn &extraColumn = this->extraColumn(column);

  if (role == Qt::DisplayRole) {
    extraColumn.variantMap[index.row()] = value;

    emit dataChanged(index, index);

    return true;
  }

  return false;
}

QVariant
CQExprModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QAbstractProxyModel::headerData(section, orientation, role);

  //--

  int nc = sourceModel()->columnCount(sourceModel()->index(0, section, QModelIndex()));

  if (section < nc)
    return sourceModel()->headerData(section, orientation, role);

  //---

  if (orientation != Qt::Horizontal)
    return QAbstractProxyModel::headerData(section, orientation, role);

  //---

  int column = section - nc;

  if (column >= numExtraColumns())
    return QVariant();

  const ExtraColumn &extraColumn = this->extraColumn(column);

  if      (role == Qt::DisplayRole) {
    if (extraColumn.header.length())
      return extraColumn.header;

    QString header = QString("%1").arg(section + 1);

    return header;
  }
  else if (role == Qt::ToolTipRole) {
    QString str = extraColumn.header;

    CQBaseModel::Type type = extraColumn.type;

    str += ":" + CQBaseModel::typeName(type);

    if (extraColumn.typeValues.length())
      str += ":" + extraColumn.typeValues;

    str += QString("\n(%1)").arg(extraColumn.expr);

    return str;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::Type)) {
    return CQBaseModel::typeToVariant(extraColumn.type);
  }
  else if (role == static_cast<int>(CQBaseModel::Role::TypeValues)) {
    return QVariant(extraColumn.typeValues);
  }

  return QVariant();
}

bool
CQExprModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (orientation != Qt::Horizontal)
    return false;

  //--

  int nc = sourceModel()->columnCount(sourceModel()->index(0, section, QModelIndex()));

  if (section < nc)
    return sourceModel()->setHeaderData(section, orientation, value, role);

  //---

  int column = section - nc;

  if (column >= numExtraColumns())
    return false;

  ExtraColumn &extraColumn = this->extraColumn(column);

  if      (role == Qt::DisplayRole) {
    extraColumn.header = value.toString();

    return true;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::Type)) {
    extraColumn.type = CQBaseModel::variantToType(value);

    return true;
  }
  else if (role == static_cast<int>(CQBaseModel::Role::TypeValues)) {
    extraColumn.typeValues = value.toString();

    return true;
  }

  return false;
}

Qt::ItemFlags
CQExprModel::
flags(const QModelIndex &index) const
{
  int nc = sourceModel()->columnCount(index);

  if (! index.isValid() || index.column() < nc)
    return sourceModel()->flags(mapToSource(index));

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex
CQExprModel::
mapToSource(const QModelIndex &index) const
{
  return model_->index(index.row(), index.column(), QModelIndex());
}

QModelIndex
CQExprModel::
mapFromSource(const QModelIndex &index) const
{
  return createIndex(index.row(), index.column(), index.internalPointer());
}

QString
CQExprModel::
replaceNumericColumns(const QString &expr) const
{
  CQStrParse parse(expr);

  QString expr1;

  while (! parse.eof()) {
    if (parse.isChar('@')) {
      parse.skipChar();

      if (parse.isDigit()) {
        int pos = parse.getPos();

        while (parse.isDigit())
          parse.skipChar();

        QString str = parse.getBefore(pos);

        int column = str.toInt();

        expr1 += QString("column(%1)").arg(column);
      }
      else
        expr1 += "@";
    }
    else
      expr1 += parse.getChar();
  }

  return expr1;
}
