#ifndef CQBaseModel_H
#define CQBaseModel_H

#include <CQBaseModelTypes.h>
#include <QAbstractItemModel>
#include <map>
#include <future>

/*!
 * \brief Wrapper class for QAbstractItemModel with extra features
 *
 * Supports extra features
 * . title
 * . max rows to determine type
 * . extra column values using roles
 *   . type
 *   . base type
 *   . type values (name values)
 *   . min, max
 *   . is key
 *   . is sorted
 *   . sort order
 *   . title
 *  extra row values
  *  . group
 */
class CQBaseModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(QString  title       READ title       WRITE setTitle      )
  Q_PROPERTY(int      maxTypeRows READ maxTypeRows WRITE setMaxTypeRows)
  Q_PROPERTY(DataType dataType    READ dataType)

  Q_ENUMS(DataType)

 public:
  enum DataType {
    DATA_TYPE_NONE,
    DATA_TYPE_CSV,
    DATA_TYPE_TSV,
    DATA_TYPE_XML,
    DATA_TYPE_JSON,
    DATA_TYPE_GNUPLOT,
    DATA_TYPE_PIVOT
  };

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

  //! get/set model title
  const QString &title() const { return title_; }
  void setTitle(const QString &v) { title_ = v; }

  //! get/set max rows to process to determine type
  int maxTypeRows() const { return maxTypeRows_; }
  void setMaxTypeRows(int i) { maxTypeRows_ = i; }

  //! get/set input data type
  const DataType &dataType() const { return dataType_; }
  void setDataType(const DataType &t) { dataType_ = t; }

  void setDataType(const CQBaseModelDataType &t) { dataType_ = (DataType) t; }

  //---

  //! get/set column type
  CQBaseModelType columnType(int column) const;
  bool setColumnType(int column, CQBaseModelType t);

  //! get/set column base type (original/calculated)
  CQBaseModelType columnBaseType(int column) const;
  bool setColumnBaseType(int column, CQBaseModelType t);

  //! get/set column values
  QString columnTypeValues(int column) const;
  bool setColumnTypeValues(int column, const QString &str);

  //! get/set column custom min value
  QVariant columnMin(int column) const;
  bool setColumnMin(int column, const QVariant &v);

  //! get/set column custom max value
  QVariant columnMax(int column) const;
  bool setColumnMax(int column, const QVariant &v);

  //! get/set column is key (unique values)
  bool isColumnKey(int column) const;
  bool setColumnKey(int column, bool b);

  //! get/set column is sorted
  bool isColumnSorted(int column) const;
  bool setColumnSorted(int column, bool b);

  //! get/set column sort order
  int columnSortOrder(int column) const;
  bool setColumnSortOrder(int column, int i);

  //! get/set column title
  QString columnTitle(int column) const;
  bool setColumnTitle(int column, const QString &v);

  //! get/set column name, value
  QVariant columnNameValue(int column, const QString &name) const;
  bool setColumnNameValue(int column, const QString &name, const QVariant &value);

  //! reset column type
  void resetColumnType(int column);
  void resetColumnTypes();

  //---

  //! get set row group
  QVariant rowGroup(int row) const;
  bool setRowGroup(int row, const QVariant &v);

  //---

  //! get/set header data
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  //---

  //! get data (no set data as read only)
  QVariant data(const QModelIndex &index, int role) const override;

  //---

  //! get value for name
  QVariant nameValue(const QString &name) const {
    auto p = nameValues_.find(name);
    if (p == nameValues_.end()) return QVariant();

    return (*p).second;
  }

  //! get value for name
  void setNameValue(const QString &name, const QVariant &value) {
    nameValues_[name] = value;
  }

  //---

  //! get column number for name
  int modelColumnNameToInd(const QString &name) const;

  static int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

  //---

  //! convert variant to type (integer or string)
  static CQBaseModelType variantToType(const QVariant &var, bool *ok=nullptr);
  static QVariant typeToVariant(CQBaseModelType type);

  //! get name for type, type for name
  static QString typeName(CQBaseModelType type);
  static CQBaseModelType nameType(const QString &name);

  //! compare variant and base model types are the same
  static bool isSameType(const QVariant &var, CQBaseModelType type);

  static QVariant typeStringToVariant(const QString &str, CQBaseModelType type);

  //! validate type value is a valid CQBaseModelType
  static bool isType(int type);

  //---

  //! convert string to real
  static double toReal(const QString &str, bool &ok);

  //! convert string to integer
  static long toInt(const QString &str, bool &ok);

  //! is real representable (exactly) as integer
  static bool isInteger(double r);

 signals:
  //! signals when data changed
  void columnTypeChanged     (int column);
  void columnBaseTypeChanged (int column);
  void columnRangeChanged    (int column);
  void columnKeyChanged      (int column);
  void columnSortedChanged   (int column);
  void columnSortOrderChanged(int column);
  void columnTitleChanged    (int column);

 protected:
  using RowValues     = std::map<int,QVariant>;
  using RoleRowValues = std::map<int,RowValues>;
  using NameValues    = std::map<QString,QVariant>;

  //---

  // column data
  struct ColumnData {
    ColumnData(int column=-1) :
     column(column) {
    }

    int             column        { -1 };                    //!< column
    CQBaseModelType type          { CQBaseModelType::NONE }; //!< auto or assigned type
    CQBaseModelType baseType      { CQBaseModelType::NONE }; //!< auto or assigned base type
    QString         typeValues;                              //!< type values
    QVariant        min;                                     //!< custom min value
    QVariant        max;                                     //!< custom max value
    bool            key           { false };                 //!< is key
    bool            sorted        { false };                 //!< is sorted
    bool            sortOrder     { Qt::AscendingOrder };    //!< sort role
    QString         title;                                   //!< title
    RoleRowValues   roleRowValues;                           //!< row role values
  };

  using ColumnDatas = std::map<int,ColumnData>;

  //---

  // row data
  struct RowData {
    RowData(int row=-1) :
     row(row) {
    }

    int      row { -1 }; //!< row
    QVariant group;      //!< group
  };

  using RowDatas = std::map<int,RowData>;

  //---

 protected:
  void genColumnTypes();

  void genColumnType(int c);

  void genColumnType(const ColumnData &columnData) const;

  ColumnData &getColumnData(int column);
  const ColumnData &getColumnData(int column) const;

  RowData &getRowData(int row);
  const RowData &getRowData(int row) const;

 private:
  void genColumnTypeI(ColumnData &columnData);

 protected:
  QString            title_;                          //!< model title
  ColumnDatas        columnDatas_;                    //!< column datas
  RowDatas           rowDatas_;                       //!< row datas
  int                maxTypeRows_ { -1 };             //!< max rows to determine type
  DataType           dataType_    { DATA_TYPE_NONE }; //!< input data type
  NameValues         nameValues_;                     //!< name values
  mutable std::mutex mutex_;                          //!< mutex
  mutable std::mutex typeMutex_;                      //!< type mutex
};

#endif
