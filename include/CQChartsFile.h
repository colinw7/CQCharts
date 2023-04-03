#ifndef CQChartsFile_H
#define CQChartsFile_H

#include <QString>

class CQCharts;

class CQChartsFile {
 public:
  CQChartsFile() = default;

  explicit CQChartsFile(CQCharts *charts, const QString &name);

  bool isValid() const;

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  QString resolve() const;

 private:
  CQCharts *charts_ { nullptr };
  QString   name_;
};

#endif
