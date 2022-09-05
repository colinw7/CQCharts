#ifndef CQChartsModelData_H
#define CQChartsModelData_H

#include <CQChartsModelTypes.h>
#include <CQChartsColumn.h>
#include <QObject>
#include <QSharedPointer>
#include <QModelIndex>
#include <QItemSelection>
#include <QPointer>
#include <future>

class CQChartsModelDetails;
class CQCharts;
#ifdef CQCHARTS_FOLDED_MODEL
class CQFoldedModel;
#endif

class CQBucketModel;
class CQHierSepModel;
class CQPivotModel;
class CQSummaryModel;
class CQTransposeModel;

class CQPropertyViewModel;

class CQFileWatcher;

class QAbstractItemModel;
class QItemSelectionModel;

/*!
 * \brief Model Data Object
 * \ingroup Charts
 */
class CQChartsModelData : public QObject {
  Q_OBJECT

  Q_PROPERTY(int     ind            READ ind              WRITE setInd           )
  Q_PROPERTY(QString id             READ id                                      )
  Q_PROPERTY(QString name           READ name             WRITE setName          )
  Q_PROPERTY(QString filename       READ filename         WRITE setFilename      )
  Q_PROPERTY(QString desc           READ desc)
  Q_PROPERTY(bool    summaryEnabled READ isSummaryEnabled WRITE setSummaryEnabled)
  Q_PROPERTY(int     currentColumn  READ currentColumn    WRITE setCurrentColumn )

 public:
  using ModelP        = QSharedPointer<QAbstractItemModel>;
  using Columns       = std::vector<CQChartsColumn>;
  using ModelDetails  = CQChartsModelDetails;
  using PropertyModel = CQPropertyViewModel;

#ifdef CQCHARTS_FOLDED_MODEL
  using FoldedModels = std::vector<CQFoldedModel *>;
#endif

 public:
  //! fold data
  struct FoldData {
    enum class FoldType {
      BUCKET,
      SEPARATOR
    };

    QString  columnsStr;
    FoldType foldType  { FoldType::BUCKET };
    bool     isAuto    { true };
    double   delta     { 1.0 };
    int      count     { 20 };
    QString  separator { "/" };
  };

  enum JoinType {
    NONE,
    LEFT,
    RIGHT,
    OUTER,
    INNER,
    CROSS,
    CONCAT
  };

  using ModelData = CQChartsModelData;

  template<typename T>
  struct ProxyModelData {
    bool       enabled   { false };
    T*         model     { nullptr };
    ModelData* modelData { nullptr };
    ModelP     modelP;
  };

 public:
  static QString description();
  static QString typeDescription(const QString &type);

 public:
  CQChartsModelData(CQCharts *charts, ModelP &model);

 ~CQChartsModelData();

  CQCharts *charts() const { return charts_; }

  //---

  // get model
  ModelP &model() { return model_; }
  const ModelP &model() const { return model_; }

  //---

  // get (unique) index
  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  bool isInd(int ind) const;

  //---

  // get id string (name or index)
  QString id() const;

  // get default id string (from index)
  QString defId() const;

  //---

  void select(const QItemSelection &sel);

  //---

  // get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s);

  //---

  // get/set file name
  QString filename() const;
  void setFilename(const QString &s);

  //---

  // get description
  QString desc() const;

  //---

  int currentColumn() const { return currentColumn_; }
  void setCurrentColumn(int i);

  //---

#ifdef CQCHARTS_FOLDED_MODEL
  // get associated fold models
  FoldedModels foldedModels() const;

  // fold model
  bool foldModel(const FoldData &data);

  void foldClear(bool notify=true);
#endif

  //---

  bool isHierarchical() const;

  //---

  // get details
  ModelDetails *details();
  const ModelDetails *details() const;

  void resetDetails();

  //---

  ModelP currentModel(bool proxy=true) const;

  //---

  // add selection model
  void addSelectionModel(QItemSelectionModel *model);
  void removeSelectionModel(QItemSelectionModel *model);

  //---

  void emitDeleted();
  void emitModelChanged();
  void emitDataChanged();

  //---

  // add/get bucket model
  CQBucketModel *addBucketModel();

  bool isBucketEnabled() const { return bucketModelData_.enabled; }
  void setBucketEnabled(bool b);

  CQBucketModel *bucketModel    () const { return bucketModelData_.model; }
  ModelData     *bucketModelData() const { return bucketModelData_.modelData; }
  const ModelP  &bucketModelP   () const { return bucketModelData_.modelP; }

  //---

  // add/get hierSep model
  CQHierSepModel *addHierSepModel();

  bool isHierSepEnabled() const { return hierSepModelData_.enabled; }
  void setHierSepEnabled(bool b);

  CQHierSepModel *hierSepModel    () const { return hierSepModelData_.model; }
  ModelData      *hierSepModelData() const { return hierSepModelData_.modelData; }
  const ModelP   &hierSepModelP   () const { return hierSepModelData_.modelP; }

  //---

  // add/get pivot model
  CQPivotModel *addPivotModel();

  bool isPivotEnabled() const { return pivotModelData_.enabled; }
  void setPivotEnabled(bool b);

  CQPivotModel *pivotModel    () const { return pivotModelData_.model; }
  ModelData    *pivotModelData() const { return pivotModelData_.modelData; }
  const ModelP &pivotModelP   () const { return pivotModelData_.modelP; }

  //---

  // add/get summary model
  CQSummaryModel *addSummaryModel();

  bool isSummaryEnabled() const { return summaryModelData_.enabled; }
  void setSummaryEnabled(bool b);

  CQSummaryModel *summaryModel    () const { return summaryModelData_.model; }
  ModelData      *summaryModelData() const { return summaryModelData_.modelData; }
  const ModelP   &summaryModelP   () const { return summaryModelData_.modelP; }

  //---

  // add/get transponse model
  CQTransposeModel *addTransposeModel();

  bool isTransposeEnabled() const { return transposeModelData_.enabled; }
  void setTransposeEnabled(bool b);

  CQTransposeModel *transposeModel    () const { return transposeModelData_.model; }
  ModelData        *transposeModelData() const { return transposeModelData_.modelData; }
  const ModelP     &transposeModelP   () const { return transposeModelData_.modelP; }

  //---

  PropertyModel *propertyViewModel();

  bool getPropertyData(const QString &name, QVariant &value) const;
  bool setPropertyData(const QString &name, const QVariant &value);

  //! name alias
  struct NameAlias {
    QString name;
    QString alias;

    NameAlias(const char *name, const char *alias="") :
     name(name), alias(alias) {
    }
  };

  //! name alias array
  struct NameAliasArray {
    std::vector<NameAlias> data;

    NameAliasArray() { }

    NameAliasArray &operator<<(const NameAlias &nameAlias) {
      data.push_back(nameAlias);

      return *this;
    }
  };

  using ModelNames   = std::map<QObject*, NameAliasArray>;
  using IdModelNames = std::map<QString, ModelNames>;

  void getPropertyNameData(IdModelNames &names) const;

  void getPropertyNames(QStringList &names) const;

  //---

  bool write(std::ostream &os, const QString &varName="") const;

  bool exportModel(const QString &filename, const CQBaseModelDataType &type,
                   bool hheader=true, bool vheader=false);
  bool exportModel(std::ostream &os, const CQBaseModelDataType &type,
                   bool hheader=true, bool vheader=false);

  bool writeCSV(const QString &filename) const;
  bool writeCSV(std::ostream &os) const;

  //---

  int replaceValue(const CQChartsColumn &column, const QVariant &oldValue,
                   const QVariant &newValue);
  int replaceNullValues(const CQChartsColumn &column, const QVariant &newValue);

  //---

  //! copy model data
  struct CopyData {
    using RowErrors = std::map<int, QStringList>;

    QString   filter;
    int       nr        { -1 };
    bool      debug     { false };
    int       numErrors { 0 };
    RowErrors rowErrors;
  };

  QAbstractItemModel *copy(CopyData &copyData);

  QAbstractItemModel *join(ModelData *joinModel, const Columns &lColumns,
                           const Columns &rColumns, JoinType joinType=JoinType::NONE);
  QAbstractItemModel *join(ModelData *joinModel, const Columns &joinCColumns,
                           JoinType joinType=JoinType::NONE);

  static QAbstractItemModel *cross(const std::vector<ModelData *> &models);
  QAbstractItemModel *cross(ModelData *joinModel);

  static QAbstractItemModel *concat(const std::vector<ModelData *> &models,
                                    const QStringList &keys=QStringList(),
                                    Qt::Orientation orient=Qt::Vertical);

  QAbstractItemModel *groupColumns(const Columns &groupColumns);

  bool copyHeaderRoles(QAbstractItemModel *toModel) const;

  bool copyColumnHeaderRoles(QAbstractItemModel *toModel, int c1, int c2) const;

 private:
  void updatePropertyModel();

  void connectModel(bool b);

  static QVariant modelIndData(QAbstractItemModel *model, const QModelIndex &ind);

 private Q_SLOTS:
  void modelDataChangedSlot(const QModelIndex &, const QModelIndex &);
  void modelHeaderDataChangedSlot(Qt::Orientation, int, int);

  void modelLayoutChangedSlot();
  void modelResetSlot();

  void modelRowsInsertedSlot();
  void modelRowsRemovedSlot();
  void modelColumnsInsertedSlot();
  void modelColumnsRemovedSlot();

  void selectionSlot();

  void fileChangedSlot(const QString &);

 Q_SIGNALS:
  // data changed
  void dataChanged();

  // model changed
  void modelChanged();

  // current model of model data changed
  void currentModelChanged();

  // current model column changed
  void currentColumnChanged(int);

  // selection changed
  void selectionChanged(QItemSelectionModel *sm);

  // model data deleted
  void deleted();

 private:
  using SelectionModelP = QPointer<QItemSelectionModel>;
  using SelectionModels = std::vector<SelectionModelP>;

#ifdef CQCHARTS_FOLDED_MODEL
  using ModelPArray = std::vector<ModelP>;
#endif

  CQCharts* charts_        { nullptr }; //!< parent charts
  ModelP    model_;                     //!< model
  int       ind_           { -1 };      //!< model ind
  QString   name_;                      //!< model name
  QString   filename_;                  //!< model file name
  int       currentColumn_ { -1 };      //!< current column

  // details
  ModelDetails* details_ { nullptr }; //!< model details

  // selection models data
  SelectionModels selectionModels_; //!< selection models

  // folded models data
#ifdef CQCHARTS_FOLDED_MODEL
  ModelP      foldProxyModel_; //!< folded proxy model
  ModelPArray foldedModels_;   //!< folded models
#endif

  // hier sep model data
  ModelP hierSepModel_; //!< hier sep model

  // proxt model datas
  ProxyModelData<CQBucketModel>    bucketModelData_;
  ProxyModelData<CQHierSepModel>   hierSepModelData_;
  ProxyModelData<CQPivotModel>     pivotModelData_;
  ProxyModelData<CQSummaryModel>   summaryModelData_;
  ProxyModelData<CQTransposeModel> transposeModelData_;

  // property model
  PropertyModel* propertyModel_ { nullptr }; //!< property model

  CQFileWatcher* fileWatcher_ { nullptr }; //!< model file watcher

  mutable std::mutex mutex_; //!< thread mutex
};

#endif
