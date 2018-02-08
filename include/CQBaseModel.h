#ifndef CQBaseModel_H
#define CQBaseModel_H

#include <QAbstractItemModel>
#include <map>

class CQBaseModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  enum class Role {
    Type       = Qt::UserRole + 1,
    TypeValues = Type + 1
  };

  // use variant numbers where possible
  enum class Type {
    NONE    = QVariant::Invalid,
    BOOLEAN = QVariant::Bool,
    INTEGER = QVariant::Int,
    REAL    = QVariant::Double,
    STRING  = QVariant::String,
    TIME    = QVariant::Time,
    COLOR   = QVariant::Color
  };

 protected:
  struct ColumnTypeData {
    Type type    { Type::NONE };
    int  numInt  { 0 };
    int  numReal { 0 };
  };

 public:
  CQBaseModel();

  Type columnType(int column) const;
  void setColumnType(int column, Type t);

  QString columnTypeValues(int column) const;
  void setColumnTypeValues(int column, const QString &str);

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

 protected:
  void genColumnTypes();

  bool genColumnTypes(const QModelIndex &parent, int c, ColumnTypeData &columnTypeData);

 protected:
  struct ColumnData {
    Type    type { Type::NONE };
    QString typeValues;
  };

  using ColumnDatas = std::map<int,ColumnData>;

 protected:
  ColumnDatas columnDatas_;
};

#endif
