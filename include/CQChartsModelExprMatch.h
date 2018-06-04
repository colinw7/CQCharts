#ifndef CQChartsModelExprMatch_H
#define CQChartsModelExprMatch_H

#include <QVariant>
#include <QString>

class CQChartsModelExprMatchExprFn;
class CQChartsModelExprMatchTclFn;

class CExpr;
#ifdef CQChartsModelExprMatch_USE_CEXPR
#include <CExpr.h>
#else
#include <CRefPtr.h>
class CExprValue;
class CExprVariable;
typedef CRefPtr<CExprValue> CExprValuePtr;
typedef CRefPtr<CExprVariable> CExprVariablePtr;
#endif

class CQTcl;

class QAbstractItemModel;

//---

class CQChartsModelExprMatch {
 public:
  enum class ExprType {
    NONE,
    EXPR,
    TCL
  };

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

  const ExprType &exprType() const { return exprType_; }
  void setExprType(const ExprType &t) { exprType_ = t; }

  //---

  void addFunction(const QString &name);

  void addExprFunction(const QString &name, CQChartsModelExprMatchExprFn *fn);
  void addTclFunction (const QString &name, CQChartsModelExprMatchTclFn *fn);

  //---

  void initMatch(const QString &expr);

  void initColumns();

  bool match(const QString &expr, const QModelIndex &ind, bool &ok);
  bool match(const QModelIndex &ind, bool &ok);

  QVariant processCmd(const QString &name, const Vars &vars);

  bool checkColumn(int col) const;
  bool checkIndex(int row, int col) const;

  static bool variantToValue(CExpr *expr, const QVariant &var, CExprValuePtr &value);
  static QVariant valueToVariant(CExpr *, const CExprValuePtr &value);

 private:
  using ExprCmds = std::vector<CQChartsModelExprMatchExprFn *>;
  using TclCmds  = std::vector<CQChartsModelExprMatchTclFn *>;

  friend class CQChartsModelExprMatchExprFn;
  friend class CQChartsModelExprMatchTclFn;
  friend class CQChartsModelExprMatchNameFn;

 private:
  void addBuiltinFunctions();

  CExpr *expr() const { return expr_; }

  CQTcl *qtcl() const { return qtcl_; }

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
#ifdef CQChartsModelExprMatch_USE_CEXPR
  using Variables = std::map<int,CExprVariablePtr>;
#endif

  using ColumnNames = std::map<int,QString>;

  QAbstractItemModel *model_      { nullptr };
  ExprType            exprType_   { ExprType::NONE };
  CExpr*              expr_       { nullptr };
  CQTcl*              qtcl_       { nullptr };
  ExprCmds            exprCmds_;
  TclCmds             tclCmds_;
  bool                debug_      { false };
#ifdef CQChartsModelExprMatch_USE_CEXPR
  CExprValuePtr       rowValue_;
  CExprValuePtr       colValue_;
  CExprVariablePtr    rowVar_;
  CExprVariablePtr    colVar_;
  Variables           columnVars_;
#endif
  ColumnNames         columnNames_;
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  QString             matchExpr_;
};

#endif
