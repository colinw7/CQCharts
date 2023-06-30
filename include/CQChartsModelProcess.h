#ifndef CQChartsModelProcess_H
#define CQChartsModelProcess_H

#include <CQChartsColumn.h>

#include <QString>
#include <QVariant>
#include <QAbstractItemModel>

#include <map>

class CQCharts;

class CQChartsModelProcessData {
 public:
  enum class Type {
    NONE,
    ADD,
    DELETE,
    MODIFY,
    REPLACE,
    REPLACE_NULL
  };

  using Column     = CQChartsColumn;
  using NameValues = std::map<QString, QVariant>;

  struct AddData {
    enum class Type {
      NONE,
      EXPR,
      TCL
    };

    Type        type { Type::NONE };
    Column      column; // added column number;
    QString     header;
    QString     expr; // for Type::EXPR
    QStringList strs; // for Type::TCL
    NameValues  varNameValues;
    QString     columnType;
  };

  struct DeleteData {
    Column column;
  };

  struct ModifyData {
    Column  column;
    QString header;
    QString expr;
    QString columnType;
  };

  struct ReplaceData {
    Column   column;
    QVariant oldValue;
    QVariant newValue;
  };

  struct ReplaceNullData {
    Column   column;
    QVariant value;
  };

  //---

 public:
  CQChartsModelProcessData() = default;

 ~CQChartsModelProcessData() { setType(Type::NONE); }

  Type type() const { return type_; }

  AddData         *addData        () { setType(Type::ADD         ); return addData_        ; }
  DeleteData      *deleteData     () { setType(Type::DELETE      ); return deleteData_     ; }
  ModifyData      *modifyData     () { setType(Type::MODIFY      ); return modifyData_     ; }
  ReplaceData     *replaceData    () { setType(Type::REPLACE     ); return replaceData_    ; }
  ReplaceNullData *replaceNullData() { setType(Type::REPLACE_NULL); return replaceNullData_; }

 private:
  void setType(Type type) {
    if (type != Type::ADD         ) { delete addData_        ; addData_         = nullptr; }
    if (type != Type::DELETE      ) { delete deleteData_     ; deleteData_      = nullptr; }
    if (type != Type::MODIFY      ) { delete modifyData_     ; modifyData_      = nullptr; }
    if (type != Type::REPLACE     ) { delete replaceData_    ; replaceData_     = nullptr; }
    if (type != Type::REPLACE_NULL) { delete replaceNullData_; replaceNullData_ = nullptr; }

    if      (type == Type::ADD         ) {
      if (! addData_        ) addData_         = new AddData; }
    else if (type == Type::DELETE      ) {
      if (! deleteData_     ) deleteData_      = new DeleteData; }
    else if (type == Type::MODIFY      ) {
      if (! modifyData_     ) modifyData_      = new ModifyData; }
    else if (type == Type::REPLACE     ) {
      if (! replaceData_    ) replaceData_     = new ReplaceData; }
    else if (type == Type::REPLACE_NULL) {
      if (! replaceNullData_) replaceNullData_ = new ReplaceNullData; }
  }

 private:
  Type type_ { Type::NONE };

  AddData*         addData_         { nullptr };
  DeleteData*      deleteData_      { nullptr };
  ModifyData*      modifyData_      { nullptr };
  ReplaceData*     replaceData_     { nullptr };
  ReplaceNullData* replaceNullData_ { nullptr };
};

//---

namespace CQChartsModelProcess {

using ModelP = QSharedPointer<QAbstractItemModel>;

enum class ErrorType {
  NONE,
  ADD_FAILED,
  DELETE_FAILED,
  MODIFY_FAILED,
  REPLACE_FAILED,
  REPLACE_NULL_FAILED,
  SET_TYPE_FAILED
};

ErrorType processColumn(ModelP model, CQChartsModelProcessData &data);

ErrorType addColumn(ModelP model, CQChartsModelProcessData::AddData &addData);

bool deleteColumn(ModelP model, const CQChartsModelProcessData::DeleteData &deleteData);

ErrorType modifyColumn(ModelP model, const CQChartsModelProcessData::ModifyData &modifyData);

int replaceColumn(CQCharts *charts, ModelP model,
                  const CQChartsModelProcessData::ReplaceData &replaceData);

int replaceNullColumn(CQCharts *charts, ModelP model,
                      const CQChartsModelProcessData::ReplaceNullData &replaceData);

}

#endif
