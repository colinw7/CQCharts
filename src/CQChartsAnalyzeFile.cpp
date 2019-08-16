#include <CQChartsAnalyzeFile.h>
#include <CQChartsUtil.h>
#include <CQStrParse.h>

CQChartsAnalyzeFile::
CQChartsAnalyzeFile(const QString &filename) :
 filename_(filename)
{
}

bool
CQChartsAnalyzeFile::
getDetails(CQBaseModelDataType &dataType, bool &commentHeader, bool &firstLineHeader,
           bool &firstColumnHeader)
{
  dataType = CQBaseModelDataType::NONE;

  commentHeader     = false;
  firstLineHeader   = false;
  firstColumnHeader = false;

  //---

  int maxLines = 10;

  QStringList lines;

  if (! CQChartsUtil::fileToLines(filename_, lines, maxLines))
    return false;

  //---

  int lineNum = 0;

  // check for comment first line
  if (lineNum < lines.length()) {
    QString line = lines[lineNum];

    CQStrParse parse(line);

    parse.skipSpace();

    if (parse.getChar() == '#') {
      commentHeader = true;

      ++lineNum;

      // skip subsequent comment lines
      while (lineNum < lines.length()) {
        QString line = lines[lineNum];

        CQStrParse parse(line);

        parse.skipSpace();

        if (parse.getChar() != '#')
          break;

        ++lineNum;
      }
    }
  }

  // check line for comma, tab, space separators
  // TODO: combine multiple lines
  if (lineNum < lines.length()) {
    QString line = lines[lineNum];

    int commaPos = line.indexOf(',');
    int tabPos   = line.indexOf('\t');
    int spacePos = line.indexOf(' ');

    CQStrParse parse(line);

    QStringList commaStrs, tabStrs, spaceStrs;

    if (commaPos >= 0) commaStrs = line.split(',' , QString::KeepEmptyParts);
    if (tabPos   >= 0) tabStrs   = line.split('\t', QString::KeepEmptyParts);
    if (spacePos >= 0) spaceStrs = line.split(' ' , QString::SkipEmptyParts);

    int nc = commaStrs.length();
    int nt = tabStrs  .length();
    int ns = spaceStrs.length();

    if      (nc > 0 && nc > nt)
      dataType = CQBaseModelDataType::CSV;
    else if (nt > 0 && nt > nc)
      dataType = CQBaseModelDataType::TSV;
    else if (ns > 0)
      dataType = CQBaseModelDataType::GNUPLOT;
  }

  // TODO: auto determine column type from first few lines ?

  // TODO: if no header then check first line again (using data type) for column heade like
  // line (string and different type from other lines)

  return true;
}
