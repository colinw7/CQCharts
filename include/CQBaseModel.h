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
    TypeValues        = Type + 1,         // column type values role
    Min               = Type + 2,         // column user min role
    Max               = Type + 3,         // column user max role
    RawValue          = Type + 4,         // raw value by role
    IntermediateValue = Type + 5,         // intermediate value role
    CachedValue       = Type + 6,         // cached value role
    OutputValue       = Type + 7,         // output value role
    Group             = Type + 8,         // group role
    Key               = Type + 9          // is key role
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

    PATH            = QVariant::UserType + 101,
    STYLE           = QVariant::UserType + 102,
    POLYGON_LIST    = QVariant::UserType + 103,
    CONNECTION_LIST = QVariant::UserType + 104,
    NAME_PAIR       = QVariant::UserType + 105
  };

 protected:
  struct ColumnTypeData {
    Type type    { Type::NONE };
    int  numInt  { 0 };
    int  numReal { 0 };
  };

 public:
  CQBaseModel();

  virtual ~CQBaseModel() { }

  //---

  Type columnType(int column) const;
  bool setColumnType(int column, Type t);

  QString columnTypeValues(int column) const;
  bool setColumnTypeValues(int column, const QString &str);

  QVariant columnMin(int column) const;
  bool setColumnMin(int column, const QVariant &v);

  QVariant columnMax(int column) const;
  bool setColumnMax(int column, const QVariant &v);

  bool isColumnKey(int column) const;
  bool setColumnKey(int column, bool b);

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

  int maxTypeRows() const { return maxTypeRows_; }
  void setMaxTypeRows(int i) { maxTypeRows_ = i; }

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
  void columnTypeChanged(int column);

  void columnRangeChanged(int column);

  void columnKeyChanged(int column);

 protected:
  using RowValues     = std::map<int,QVariant>;
  using RoleRowValues = std::map<int,RowValues>;

  struct ColumnData {
    ColumnData(int column=-1) :
     column(column) {
    }

    int           column { -1 };       // column
    Type          type { Type::NONE }; // auto or assigned type
    QString       typeValues;          // type values
    QVariant      min;                 // custom min value
    QVariant      max;                 // custom max value
    bool          key { false };       // is key
    RoleRowValues roleRowValues;       // row role values
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
};

#endif
