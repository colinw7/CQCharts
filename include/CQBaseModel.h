#ifndef CQBaseModel_H
#define CQBaseModel_H

#include <QAbstractItemModel>
#include <map>

class CQBaseModel : public QAbstractItemModel {
  Q_OBJECT

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
    Type       = Qt::UserRole + 1,
    TypeValues = Type + 1,
    Min        = Type + 2,
    Max        = Type + 3
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
    POLYGON = QVariant::PolygonF,
    COLOR   = QVariant::Color,
    PEN     = QVariant::Pen,
    BRUSH   = QVariant::Brush,
    IMAGE   = QVariant::Image,
    TIME    = QVariant::Time,
    PATH    = QVariant::UserType + 101,
    STYLE   = QVariant::UserType + 102
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

  void resetColumnTypes();

  //---

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  QVariant data(const QModelIndex &index, int role) const override;

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

 protected:
  void genColumnTypes();

  Type genColumnType(int c);

  bool genColumnType(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData);

 protected:
  struct ColumnData {
    Type     type { Type::NONE };
    QString  typeValues;
    QVariant min;
    QVariant max;
  };

  using ColumnDatas = std::map<int,ColumnData>;

 protected:
  ColumnDatas columnDatas_;
};

#endif
