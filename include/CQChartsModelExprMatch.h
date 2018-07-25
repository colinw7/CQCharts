#ifndef CQChartsModelExprMatch_H
#define CQChartsModelExprMatch_H

#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <vector>

class CQChartsModelExprMatchFn;

#ifdef CQCharts_USE_TCL
class CQTcl;
#endif

class QAbstractItemModel;

//---

class CQChartsModelExprMatch {
 public:
  using Vars = std::vector<QVariant>;

 public:
  CQChartsModelExprMatch(QAbstractItemModel *model=0);

 ~CQChartsModelExprMatch();

  //---

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model) { model_ = model; }

  //---

  int debug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  //---

  void addFunction(const QString &name);

  //---

  void initMatch(const QString &expr);

  void initColumns();

  bool match(const QString &expr, const QModelIndex &ind, bool &ok);
  bool match(const QModelIndex &ind, bool &ok);

  QVariant processCmd(const QString &name, const Vars &vars);

  bool checkColumn(int col) const;
  bool checkIndex(int row, int col) const;

 private:
  using TclCmds = std::vector<CQChartsModelExprMatchFn *>;

  friend class CQChartsModelExprMatchFn;

 private:
  void addBuiltinFunctions();

#ifdef CQCharts_USE_TCL
  CQTcl *qtcl() const { return qtcl_; }
#endif

  QVariant columnCmd(const Vars &vars) const;
  QVariant rowCmd   (const Vars &vars) const;
  QVariant cellCmd  (const Vars &vars) const;
  QVariant headerCmd(const Vars &vars) const;

  int currentRow() const { return currentRow_; }
  int currentCol() const { return currentCol_; }

  bool evaluateExpression(const QString &expr, const QModelIndex &ind,
                          QVariant &value, bool replace);

  bool setTclResult(const QVariant &rc);
  bool getTclResult(QVariant &rc) const;

  QString replaceNumericColumns(const QString &expr, const QModelIndex &ind) const;

 private:
  using ColumnNames = std::map<int,QString>;

  QAbstractItemModel *model_      { nullptr };
#ifdef CQCharts_USE_TCL
  CQTcl*              qtcl_       { nullptr };
#endif
  TclCmds             tclCmds_;
  bool                debug_      { false };
  ColumnNames         columnNames_;
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  QString             matchExpr_;
};

#endif
