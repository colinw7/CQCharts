#ifndef CQChartsAnalyzeFile_H
#define CQChartsAnalyzeFile_H

#include <CQBaseModel.h>
#include <QString>

class CQChartsAnalyzeFile {
 public:
  CQChartsAnalyzeFile(const QString &filename);

  bool getDetails(CQBaseModel::DataType &dataType, bool &commentHeader,
                  bool &firstLineHeader, bool &firstColumnHeader);

 private:
  QString filename_;
};

#endif
