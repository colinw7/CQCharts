#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CUnixFile.h>
#include <CStrUtil.h>
#include <COSNaN.h>
#include <QAbstractItemModel>
#include <QVariant>
#include <QStringList>

namespace CQChartsUtil {

inline double toReal(const QString &str, bool &ok) {
  ok = true;

  double r = 0.0;

  try {
    r = std::stod(str.toStdString());
  }
  catch (...) {
    ok = false;
  }

  return r;
}

inline double toReal(const QVariant &var, bool &ok) {
  return toReal(var.toString(), ok);
}

inline bool toReal(const QString &str, double &r) {
  bool ok;

  r = toReal(str, ok);

  return ok;
}

inline long toInt(const QString &str, bool &ok) {
  ok = true;

  long i = 0.0;

  try {
    i = std::stol(str.toStdString());
  }
  catch (...) {
    ok = false;
  }

  return i;
}

inline long toInt(const QVariant &var, bool &ok) {
  return toInt(var.toString(), ok);
}

inline bool toInt(const QString &str, long &i) {
  bool ok;

  i = toInt(str, ok);

  return ok;
}

inline QString toString(double r) {
  return CStrUtil::toString(r).c_str();
}

inline QString toString(long i) {
  return CStrUtil::toString(i).c_str();
}

//------

inline bool isNaN(double r) {
  return COSNaN::is_nan(r) && ! COSNaN::is_inf(r);
}

//------

inline bool fileToLines(const QString &filename, QStringList &lines) {
  // open file
  CUnixFile file(filename.toStdString());

  if (! file.open())
    return false;

  // read lines
  std::string line;

  while (file.readLine(line))
    lines.push_back(line.c_str());

  return true;
}

inline QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  ok = var.isValid();

  return var.toString();
}

inline QString modelString(QAbstractItemModel *model, int row, int col, bool &ok) {
  QModelIndex ind = model->index(row, col);

  return modelString(model, ind, ok);
}

inline double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  ok = true;

  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  if (var.type() == QVariant::Double)
    return var.toReal();

  return toReal(var, ok);
}

inline double modelReal(QAbstractItemModel *model, int row, int col, bool &ok) {
  ok = true;

  QModelIndex ind = model->index(row, col);

  return modelReal(model, ind, ok);
}

inline long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  ok = true;

  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  if (var.type() == QVariant::Int)
    return var.toInt();

  return toInt(var, ok);
}

inline long modelInteger(QAbstractItemModel *model, int row, int col, bool &ok) {
  ok = true;

  QModelIndex ind = model->index(row, col);

  return modelInteger(model, ind, ok);
}

}

#endif
