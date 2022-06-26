#ifndef CQChartsMetaObj_H
#define CQChartsMetaObj_H

#include <CQChartsColumn.h>

#include <QObject>

#include <iostream>

class CQChartsMetaObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  CQChartsMetaObj() { }

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) { column_ = c; }

  bool printType(std::ostream &os, const QString &type) {
    if (type == "column")
      os << column_.toString().toStdString() << "\n";
    else
      return false;

    return true;
  }

 private:
  CQChartsColumn column_;
};

#endif
