#ifndef CQChartsFileType_H
#define CQChartsFileType_H

#include <QString>
#include <QStringList>

enum class CQChartsFileType {
  NONE,
  CSV,
  TSV,
  JSON,
  DATA,
  EXPR,
  VARS
};

namespace CQChartsFileTypeUtil {

inline QStringList fileTypeNames() {
  return QStringList() << "CSV" << "TSV" << "Json" << "Data" << "Expr" << "Vars";
}

inline CQChartsFileType stringToFileType(const QString &str) {
  QString lstr = str.toLower();

  if      (lstr == "csv" ) return CQChartsFileType::CSV;
  else if (lstr == "tsv" ) return CQChartsFileType::TSV;
  else if (lstr == "json") return CQChartsFileType::JSON;
  else if (lstr == "data") return CQChartsFileType::DATA;
  else if (lstr == "expr") return CQChartsFileType::EXPR;
  else if (lstr == "vars") return CQChartsFileType::VARS;
  else                     return CQChartsFileType::NONE;
}

inline QString fileTypeToString(CQChartsFileType type) {
  if      (type == CQChartsFileType::CSV ) return "csv";
  else if (type == CQChartsFileType::TSV ) return "tsv";
  else if (type == CQChartsFileType::JSON) return "json";
  else if (type == CQChartsFileType::DATA) return "data";
  else if (type == CQChartsFileType::EXPR) return "expr";
  else if (type == CQChartsFileType::VARS) return "vars";
  else                                     return "";
}

}

#endif
