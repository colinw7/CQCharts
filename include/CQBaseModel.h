#ifndef CQBaseModel_H
#define CQBaseModel_H

#include <CQBaseModelTypes.h>
#include <QAbstractItemModel>
#include <map>

class CQBaseModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(int maxTypeRows READ maxTypeRows WRITE setMaxTypeRows)

 protected:
  struct ColumnTypeData {
    CQBaseModelType type     { CQBaseModelType::NONE };
    CQBaseModelType baseType { CQBaseModelType::NONE };
    int             numInt   { 0 };
    int             numReal  { 0 };
  };

 public:
  CQBaseModel();

  virtual ~CQBaseModel() { }

  //---

  CQBaseModelType columnType(int column) const;
  bool setColumnType(int column, CQBaseModelType t);

  CQBaseModelType columnBaseType(int column) const;
  bool setColumnBaseType(int column, CQBaseModelType t);

  QString columnTypeValues(int column) const;
  bool setColumnTypeValues(int column, const QString &str);

  QVariant columnMin(int column) const;
  bool setColumnMin(int column, const QVariant &v);

  QVariant columnMax(int column) const;
  bool setColumnMax(int column, const QVariant &v);

  bool isColumnKey(int column) const;
  bool setColumnKey(int column, bool b);

  bool isColumnSorted(int column) const;
  bool setColumnSorted(int column, bool b);

  int columnSortOrder(int column) const;
  bool setColumnSortOrder(int column, int i);

  void resetColumnType(int column);
  void resetColumnTypes();

  //---

  QVariant rowGroup(int row) const;
  bool setRowGroup(int row, const QVariant &v);

  //---

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  QVariant data(const QModelIndex &index, int role) const override;

  //---

  QVariant nameValue(const QString &name) const {
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) return QVariant();

    return (*p).second;
  }

  void setNameValue(const QString &name, const QVariant &value) {
    nameValues_[name] = value;
  }

  //---

  int maxTypeRows() const { return maxTypeRows_; }
  void setMaxTypeRows(int i) { maxTypeRows_ = i; }

  //---

  int modelColumnNameToInd(const QString &name) const;

  static int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

  //---

  static CQBaseModelType variantToType(const QVariant &var, bool *ok=nullptr);
  static QVariant typeToVariant(CQBaseModelType type);

  static QString typeName(CQBaseModelType type);
  static CQBaseModelType nameType(const QString &name);

  static bool isSameType(const QVariant &var, CQBaseModelType type);

  static QVariant typeStringToVariant(const QString &str, CQBaseModelType type);

  static bool isType(int type);

  static double toReal(const QString &str, bool &ok);

  static long toInt(const QString &str, bool &ok);

  static bool isInteger(double r);

 signals:
  void columnTypeChanged     (int column);
  void columnBaseTypeChanged (int column);
  void columnRangeChanged    (int column);
  void columnKeyChanged      (int column);
  void columnSortedChanged   (int column);
  void columnSortOrderChanged(int column);

 protected:
  using RowValues     = std::map<int,QVariant>;
  using RoleRowValues = std::map<int,RowValues>;
  using NameValues    = std::map<QString,QVariant>;

  struct ColumnData {
    ColumnData(int column=-1) :
     column(column) {
    }

    int             column        { -1 };                    // column
    CQBaseModelType type          { CQBaseModelType::NONE }; // auto or assigned type
    CQBaseModelType baseType      { CQBaseModelType::NONE }; // auto or assigned base type
    QString         typeValues;                              // type values
    QVariant        min;                                     // custom min value
    QVariant        max;                                     // custom max value
    bool            key           { false };                 // is key
    bool            sorted        { false };                 // is sorted
    bool            sortOrder     { Qt::AscendingOrder };    // sort role
    RoleRowValues   roleRowValues;                           // row role values
  };

  using ColumnDatas = std::map<int,ColumnData>;

  struct RowData {
    RowData(int row=-1) :
     row(row) {
    }

    int      row { -1 }; // row
    QVariant group;
  };

  using RowDatas = std::map<int,RowData>;

 protected:
  void genColumnTypes();

  void genColumnType(int c);
  void genColumnType(ColumnData &columnData);
  bool genColumnType(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData);

  ColumnData &getColumnData(int column) const;

  RowData &getRowData(int column) const;

 protected:
  ColumnDatas columnDatas_;
  RowDatas    rowDatas_;
  int         maxTypeRows_ { 1000 };
  NameValues  nameValues_;
};

#endif
