#ifndef CQBaseModel_H
#define CQBaseModel_H

#include <QAbstractItemModel>
#include <map>

class CQBaseModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(int maxTypeRows READ maxTypeRows WRITE setMaxTypeRows)

 public:
  enum class DataType {
    NONE,
    CSV,
    TSV,
    XML,
    JSON,
    GNUPLOT
  };

  enum class Role {
    Type              = Qt::UserRole + 1, // column type role
    BaseType          = Type + 1,         // column base type role
    TypeValues        = Type + 2,         // column type values role
    Min               = Type + 3,         // column user min role
    Max               = Type + 4,         // column user max role
    Sorted            = Type + 5,         // sorted role
    SortOrder         = Type + 6,         // sort order role
    Key               = Type + 7,         // is key role
    RawValue          = Type + 8,         // raw value by role
    IntermediateValue = Type + 9,         // intermediate value role
    CachedValue       = Type + 10,        // cached value role
    OutputValue       = Type + 11,        // output value role
    Group             = Type + 12         // group role
  };

  // use variant numbers where possible
  enum class Type {
    NONE    = QVariant::Invalid,
    BOOLEAN = QVariant::Bool,
    INTEGER = QVariant::Int,
    REAL    = QVariant::Double,
    STRING  = QVariant::String,
    STRINGS = QVariant::StringList,
    POINT   = QVariant::PointF,
    LINE    = QVariant::LineF,
    RECT    = QVariant::RectF,
    SIZE    = QVariant::SizeF,
#if QT_VERSION >= 0x050000
    POLYGON = QVariant::PolygonF,
#else
    POLYGON = QVariant::UserType + 199,
#endif
    COLOR   = QVariant::Color,
    PEN     = QVariant::Pen,
    BRUSH   = QVariant::Brush,
    IMAGE   = QVariant::Image,
    TIME    = QVariant::Time,

    SYMBOL          = QVariant::UserType + 101,
    SYMBOL_SIZE     = QVariant::UserType + 102,
    FONT_SIZE       = QVariant::UserType + 103,
    PATH            = QVariant::UserType + 104,
    STYLE           = QVariant::UserType + 105,
    POLYGON_LIST    = QVariant::UserType + 106,
    CONNECTION_LIST = QVariant::UserType + 107,
    NAME_PAIR       = QVariant::UserType + 108,
    COLUMN          = QVariant::UserType + 109,
    COLUMN_LIST     = QVariant::UserType + 110,
    ENUM            = QVariant::UserType + 111
  };

 protected:
  struct ColumnTypeData {
    Type type     { Type::NONE };
    Type baseType { Type::NONE };
    int  numInt   { 0 };
    int  numReal  { 0 };
  };

 public:
  CQBaseModel();

  virtual ~CQBaseModel() { }

  //---

  Type columnType(int column) const;
  bool setColumnType(int column, Type t);

  Type columnBaseType(int column) const;
  bool setColumnBaseType(int column, Type t);

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

  static Type variantToType(const QVariant &var, bool *ok=nullptr);
  static QVariant typeToVariant(Type type);

  static QString typeName(Type type);
  static Type nameType(const QString &name);

  static bool isSameType(const QVariant &var, Type type);

  static QVariant typeStringToVariant(const QString &str, Type type);

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

    int           column        { -1 };                 // column
    Type          type          { Type::NONE };         // auto or assigned type
    Type          baseType      { Type::NONE };         // auto or assigned base type
    QString       typeValues;                           // type values
    QVariant      min;                                  // custom min value
    QVariant      max;                                  // custom max value
    bool          key           { false };              // is key
    bool          sorted        { false };              // is sorted
    bool          sortOrder     { Qt::AscendingOrder }; // sort role
    RoleRowValues roleRowValues;                        // row role values
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
