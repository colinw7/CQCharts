#ifndef CQExprModelFnImpl_H
#define CQExprModelFnImpl_H

#include <CQExprModelFn.h>
#include <COSRand.h>

// column(), column(col) : get column value
class CQExprModelColumnFn : public CQExprModelFn {
 public:
  CQExprModelColumnFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    if (values.size() == 0) {
      return expr->createIntegerValue(col);
    }

    if (! values[0]->getIntegerValue(col))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 1)
      extraInd = 1;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    return variantToValue(expr, var);
  }
};

//---

// row(), row(row) : get row value
class CQExprModelRowFn : public CQExprModelFn {
 public:
  CQExprModelRowFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    if (values.size() == 0) {
      return expr->createIntegerValue(row);
    }

    if (! values[0]->getIntegerValue(row))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 1)
      extraInd = 1;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    return variantToValue(expr, var);
  }
};

//---

// cell(), cell(row,column) : get cell value
class CQExprModelCellFn : public CQExprModelFn {
 public:
  CQExprModelCellFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    if (values.size() < 2) {
      // TODO: row and column string value ?
      return CExprValuePtr();
    }

    if (! values[0]->getIntegerValue(row))
      return CExprValuePtr();

    if (! values[1]->getIntegerValue(col))
      return CExprValuePtr();

    int extraInd = -1;

    if (values.size() > 2)
      extraInd = 2;

    //---

    if (! checkIndex(row, col)) {
      if (extraInd >= 0)
        return values[extraInd];

      return CExprValuePtr();
    }

    //---

    QModelIndex ind = model_->index(row, col, QModelIndex());

    QVariant var = model_->data(ind, Qt::DisplayRole);

    return variantToValue(expr, var);
  }
};

//---

// setColumn(value), setColumn(col,value) : set column value
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

// setCell(value), setCell(row,col,value) : set row value
class CQExprModelSetCellFn : public CQExprModelFn {
 public:
  CQExprModelSetCellFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    long row = model_->currentRow(), col = model_->currentCol();

    QVariant var;

    if      (values.size() == 1) {
      var = valueToVariant(expr, values[0]);
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

// map(), map(max), map(min,max)
class CQExprModelMapFn : public CQExprModelFn {
 public:
  CQExprModelMapFn(CQExprModel *model) :
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

// norm(col), norm(col,scale)
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

    double scale = 1.0;

    if (values.size() > 1) {
      if (! values[1]->getRealValue(scale))
        scale = 1.0;
    }

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
        s = scale*(value - minVal)/d;

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
        s = scale*double(value - minVal)/d;

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

class CQExprModelRandFn : public CQExprModelFn {
 public:
  CQExprModelRandFn(CQExprModel *model) :
   CQExprModelFn(model) {
  }

  CExprValuePtr operator()(CExpr *expr, const CExprValueArray &values) {
    double min = 0.0;
    double max = 1.0;

    if (values.size() > 0)
      (void) values[0]->getRealValue(max);

    if (values.size() > 1)
      (void) values[1]->getRealValue(min);

    double r = COSRand::randIn(min, max);

    return expr->createRealValue(r);
  }
};

#endif
