#ifndef CQChartsModelExprMatch_H
#define CQChartsModelExprMatch_H

#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <vector>

class CQChartsModelExprMatchFn;
class CQChartsModelData;
class CQChartsExprTcl;
class CQChartsExprCmdValues;

class QAbstractItemModel;

//---

/*!
 * \brief Model Expression Match class
 * \ingroup Charts
 */
class CQChartsModelExprMatch {
 public:
  using Values = std::vector<QVariant>;

 public:
  CQChartsModelExprMatch(QAbstractItemModel *model=nullptr);
 ~CQChartsModelExprMatch();

  //---

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model);

  //---

  int isDebug() const { return debug_; }
  void setDebug(int i) { debug_ = i; }

  //---

  CQChartsExprTcl *qtcl() const { return qtcl_; }

  //---

  void addFunction(const QString &name);

  //---

  void initMatch(const QString &expr);

  void initColumns();

  bool match(const QString &expr, const QModelIndex &ind, bool &ok);
  bool match(const QModelIndex &ind, bool &ok);

  QVariant processCmd(const QString &name, const Values &values);

  bool checkColumn(int col) const;
  bool checkIndex(int row, int col) const;

 private:
  using TclCmds = std::vector<CQChartsModelExprMatchFn *>;

  friend class CQChartsModelExprMatchFn;

 private:
  void addBuiltinFunctions();

  //---

  // get model data
  QVariant columnCmd(const Values &values) const;
  QVariant rowCmd   (const Values &values) const;
  QVariant cellCmd  (const Values &values) const;

  // get header data
  QVariant headerCmd(const Values &values) const;

  // string
  QVariant matchCmd(const Values &values) const;

  // math
  QVariant isnanCmd(const Values &values) const;

  // details
  QVariant detailsCmd(const QString &name, const Values &values) const;

  //---

  int currentRow() const { return currentRow_; }
  int currentCol() const { return currentCol_; }

  bool getColumn(CQChartsExprCmdValues &cmdValues, int &col) const;

  bool evaluateExpression(const QString &expr, const QModelIndex &ind,
                          QVariant &value, bool replace);

  QString replaceExprColumns(const QString &expr, const QModelIndex &ind) const;

  QVariant getCmdData(int row, int col) const;

  QVariant getCmdData(const QModelIndex &ind) const;

 private:
  using ColumnNames = std::map<int,QString>;
  using NameColumns = std::map<QString,int>;

  CQChartsModelData*  modelData_  { nullptr };
  QAbstractItemModel* model_      { nullptr };
  CQChartsExprTcl*    qtcl_       { nullptr };
  TclCmds             tclCmds_;
  bool                detailsFns_ { false };
  bool                debug_      { false };
  QString             matchExpr_;
  int                 nr_         { 0 };
  int                 nc_         { 0 };
  mutable int         currentRow_ { 0 };
  mutable int         currentCol_ { 0 };
  ColumnNames         columnNames_;
  NameColumns         nameColumns_;
};

#endif
