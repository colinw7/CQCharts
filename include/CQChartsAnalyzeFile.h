#ifndef CQChartsAnalyzeFile_H
#define CQChartsAnalyzeFile_H

#include <CQChartsModelTypes.h>
#include <QString>

/*!
 * \brief analyze a file to auto determine file type and format
 * \ingroup Charts
 */
class CQChartsAnalyzeFile {
 public:
  using DataType = CQBaseModelDataType;

 public:
  explicit CQChartsAnalyzeFile(const QString &filename);

  int maxLines() const { return maxLines_; }
  void setMaxLines(int i) { maxLines_ = i; }

  bool getDetails(DataType &dataType, bool &commentHeader,
                  bool &firstLineHeader, bool &firstColumnHeader);

 private:
  QString filename_;         //!< file name
  int     maxLines_ { 100 }; //!< max lines to analyze
};

#endif
