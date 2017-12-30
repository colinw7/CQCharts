#ifndef CQChartsModelP_H
#define CQChartsModelP_H

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QPointer>

struct CQChartsModelP {
  using ModelP = QSharedPointer<QAbstractItemModel>;
  using PModel = QPointer<QAbstractItemModel>;

  CQChartsModelP()              : type(Type::NONE   )                 { }
  CQChartsModelP(ModelP modelp) : type(Type::SHARED ), modelp(modelp) { }
  CQChartsModelP(PModel pmodel) : type(Type::POINTER), pmodel(pmodel) { }

  enum class Type {
    NONE,
    SHARED,
    POINTER
  };

  Type   type;
  ModelP modelp;
  PModel pmodel;

  QAbstractItemModel *data() const {
    if (type == Type::SHARED)
      return modelp.data();

    return pmodel.data();
  }
};

#endif
