#ifndef CQChartsUtil_H
#define CQChartsUtil_H

#include <CUnixFile.h>
#include <QAbstractItemModel>
#include <QVariant>

namespace CQChartsUtil {

inline double toReal(const QString &str) {
  double r = 0.0;

  try {
    r = std::stod(str.toStdString());
  }
  catch (...) {
  }

  return r;
}

inline double toReal(const QVariant &var) {
  return toReal(var.toString());
}

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

inline double modelReal(QAbstractItemModel *model, int row, int col) {
  QModelIndex ind = model->index(row, col);

  QVariant var = model->data(ind, Qt::UserRole);

  if (! var.isValid())
    var = model->data(ind, Qt::DisplayRole);

  if (var.type() == QVariant::Double)
    return var.toReal();

  return toReal(var);
}

}

#endif
