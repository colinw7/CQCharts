#ifndef CQChartsModelData_H
#define CQChartsModelData_H

#include <CQBaseModelTypes.h>
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
class CQSummaryModel;
class CQPropertyViewModel;

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
  Q_PROPERTY(bool    summaryEnabled READ isSummaryEnabled WRITE setSummaryEnabled)
  Q_PROPERTY(int     currentColumn  READ currentColumn    WRITE setCurrentColumn )

 public:
  using ModelP = QSharedPointer<QAbstractItemModel>;

#ifdef CQCHARTS_FOLDED_MODEL
  using FoldedModels = std::vector<CQFoldedModel *>;
#endif

 public:
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

  //---

  // get id string (name or index)
  QString id() const;

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

  // get details
  CQChartsModelDetails *details();
  const CQChartsModelDetails *details() const;

  //---

  ModelP currentModel() const;

  //---

  // add selection model
  void addSelectionModel(QItemSelectionModel *model);
  void removeSelectionModel(QItemSelectionModel *model);

  //---

  // add/get summary model
  CQSummaryModel *addSummaryModel();

  bool isSummaryEnabled() const { return summaryEnabled_; }
  void setSummaryEnabled(bool b);

  CQSummaryModel *summaryModel() const { return summaryModel_; }

  CQChartsModelData *summaryModelData() const { return summaryModelData_; }

  const ModelP &summaryModelP() const { return summaryModelP_; }

  //---

  CQPropertyViewModel *propertyViewModel();

  bool getPropertyData(const QString &name, QVariant &value) const;
  bool setPropertyData(const QString &name, const QVariant &value);

  struct NameAlias {
    QString name;
    QString alias;

    NameAlias(const char *name, const char *alias="") :
     name(name), alias(alias) {
    }
  };

  struct NameAliasArray {
    std::vector<NameAlias> data;

    NameAliasArray() { }

    NameAliasArray &operator<<(const NameAlias &nameAlias) {
      data.push_back(nameAlias);

      return *this;
    }
  };

  using ModelNames   = std::map<QObject*,NameAliasArray>;
  using IdModelNames = std::map<QString,ModelNames>;

  void getPropertyNameData(IdModelNames &names) const;

  void getPropertyNames(QStringList &names) const;

  //---

  bool write(std::ostream &os, const QString &varName="") const;

  bool exportModel(const QString &fileName, const CQBaseModelDataType &type,
                   bool hheader=true, bool vheader=false);
  bool exportModel(std::ostream &os, const CQBaseModelDataType &type,
                   bool hheader=true, bool vheader=false);

  bool writeCSV(const QString &fileName) const;
  bool writeCSV(std::ostream &os) const;

  //---

  QAbstractItemModel *copy();

 private:
  void updatePropertyModel();

  void connectModel();
  void disconnectModel();

 private slots:
  void modelDataChangedSlot(const QModelIndex &, const QModelIndex &);

  void modelLayoutChangedSlot();
  void modelResetSlot();

  void modelRowsInsertedSlot();
  void modelRowsRemovedSlot();
  void modelColumnsInsertedSlot();
  void modelColumnsRemovedSlot();

  void selectionSlot();

 signals:
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

 private:
  using SelectionModelP = QPointer<QItemSelectionModel>;
  using SelectionModels = std::vector<SelectionModelP>;

#ifdef CQCHARTS_FOLDED_MODEL
  using ModelPArray = std::vector<ModelP>;
#endif

  CQCharts*             charts_           { nullptr }; //!< parent charts
  ModelP                model_;                        //!< model
  int                   ind_              { -1 };      //!< model ind
  QString               name_;                         //!< model name
  QString               filename_;                     //!< model file name
  int                   currentColumn_    { -1 };      //!< current column

  // details
  CQChartsModelDetails* details_          { nullptr }; //!< model details

  // selection models data
  SelectionModels       selectionModels_;              //!< selection models

  // folded models data
#ifdef CQCHARTS_FOLDED_MODEL
  ModelP                foldProxyModel_;               //!< folded proxy model
  ModelPArray           foldedModels_;                 //!< folded models
#endif

  // hier sep model data
  ModelP                hierSepModel_;                 //!< hier sep model

  // summary model data
  bool                  summaryEnabled_   { false };   //!< summary model enabled
  CQSummaryModel*       summaryModel_     { nullptr }; //!< summary model
  ModelP                summaryModelP_;                //!< summary model (shared pointer)
  CQChartsModelData*    summaryModelData_ { nullptr }; //!< summary model data

  // property model
  CQPropertyViewModel*  propertyModel_    { nullptr }; //!< property model

  mutable std::mutex    mutex_;                        //!< thread mutex
};

#endif
