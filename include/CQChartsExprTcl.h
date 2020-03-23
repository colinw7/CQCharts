#ifndef CQChartsExprTcl_H
#define CQChartsExprTcl_H

#include <CQTclUtil.h>
#include <CMathUtil.h>

#include <QAbstractItemModel>

class CQChartsExprTcl : public CQTcl {
 public:
  CQChartsExprTcl(QAbstractItemModel *model=nullptr) :
   model_(model) {
  }

  const QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *p) { model_ = p; }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  const QVariant &lastValue() const { return lastValue_; }
  void setLastValue(const QVariant &v) { lastValue_ = v; }
  void resetLastValue() { lastValue_ = QVariant(); }
  bool hasLastValue() const { return lastValue_.isValid(); }

  int nameColumn(const QString &name) const {
    auto p = nameColumns_.find(name);

    return (p != nameColumns_.end() ? (*p).second : -1);
  }
  void setNameColumn(const QString &name, int column) {
    nameColumns_[name] = column;

    traceVar(name);
  }

  QString encodeColumnName(const QString &name) {
    assert(name.length());

    QString name1;

    for (int i = 0; i < name.length(); ++i) {
      const auto &c = name[i];

      if (i == 0) {
        if (c.isLetter() || c == '_')
          name1 += c;
        else
          name1 += '_';
      }
      else {
        if (c.isLetter() || c.isNumber() || c == '_')
          name1 += c;
        else
          name1 += '_';
      }
    }

    return name1;
  }

  void setColumnRole(int column, int role) {
    columnRoles_[column] = role;
  }

  void resetColumns() { nameColumns_.clear(); columnRoles_.clear(); }

  void handleTrace(const char *name, int flags) override {
    if (flags & TCL_TRACE_READS)
      setVar(name, row(), column());
  }

  void initVars() {
    // TODO: x optional ?
    traceVar("row"   );
    traceVar("x"     );
    traceVar("column");
    traceVar("col"   );
    traceVar("PI"    );
    traceVar("NaN"   );
    traceVar("_"     );
  }

  void defineProc(const QString &name, const QString &args, const QString &body) {
    eval(QString("proc ::tcl::mathfunc::%1 {%2} {%3}").arg(name).arg(args).arg(body));
  }

  bool evaluateExpression(const QString &expr, QVariant &value, bool showError=false) const {
    auto *th = const_cast<CQChartsExprTcl *>(this);

    int rc = th->evalExpr(expr, showError);

    if (rc != TCL_OK) {
      if (isDomainError(rc)) {
        double x = CMathUtil::getNaN();

        value = QVariant(x);

        th->setLastValue(value);

        return true;
      }

      if (showError)
        std::cerr << errorInfo(rc).toStdString() << std::endl;

      return false;
    }

    value = getResult();

    th->setLastValue(value);

    return true;
  }

 private:
  void setVar(const QString &name, int row, int column) {
    int nameCol = (model_ ? nameColumn(name) : -1);

    if      (nameCol >= 0) {
      // get model value
      QModelIndex parent; // TODO

      QModelIndex ind = model_->index(row, nameCol, parent);

      QVariant var;

      auto pr = columnRoles_.find(column);

      if (pr != columnRoles_.end())
        var = model_->data(ind, (*pr).second);

      if (! var.isValid())
        var = model_->data(ind, Qt::EditRole);

      if (! var.isValid())
        var = model_->data(ind, Qt::DisplayRole);

      // store value in column variable
      createVar(name, var);
    }
    else if (name == "row" || name == "x") {
      createVar(name, row);
    }
    else if (name == "column" || name == "col") {
      createVar(name, column);
    }
    else if (name == "PI") {
      createVar(name, QVariant(M_PI));
    }
    else if (name == "NaN") {
      createVar(name, QVariant(CMathUtil::getNaN()));
    }
    else if (name == "_") {
      if (hasLastValue())
        createVar(name, QVariant(lastValue()));
      else
        createVar(name, QVariant(0.0));
    }
  }

 private:
  using NameColumns = std::map<QString,int>;
  using ColumnRoles = std::map<int,int>;

  QAbstractItemModel *model_  { nullptr };
  int                 row_    { -1 };
  int                 column_ { -1 };
  QVariant            lastValue_;
  NameColumns         nameColumns_;
  ColumnRoles         columnRoles_;
};

#endif
