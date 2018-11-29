#ifndef CQChartsAnalyzeFile_H
#define CQChartsAnalyzeFile_H

#include <CQBaseModelTypes.h>
#include <QString>

class CQChartsAnalyzeFile {
 public:
  CQChartsAnalyzeFile(const QString &filename);

  bool getDetails(CQBaseModelDataType &dataType, bool &commentHeader,
                  bool &firstLineHeader, bool &firstColumnHeader);

 private:
  QString filename_;
};

#endif
