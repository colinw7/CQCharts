#ifndef CQChartsFile_H
#define CQChartsFile_H

#include <QString>

class CQChartsFile {
 public:
  CQChartsFile() {}

  explicit CQChartsFile(CQCharts *charts, const QString &name) :
   charts_(charts), name_(name) {
  }

  bool isValid() const { return (charts_ && name_ != ""); }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  QString resolve() const { return name_; }

 private:
  CQCharts *charts_ { nullptr };
  QString   name_;
};

#endif
