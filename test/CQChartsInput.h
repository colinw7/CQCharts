#ifndef CQChartsInput_H
#define CQChartsInput_H

#include <QFile>
#include <CQChartsReadLine.h>

namespace CQChartsInput {

template<typename IS_COMPLETE, typename PROCESS_LINE>
bool processFileLines(const QString &filename, IS_COMPLETE isComplete, PROCESS_LINE processLine) {
  // open file
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly))
    return false;

  // read lines
  QTextStream in(&file);

  while (! in.atEnd()) {
    QString line = in.readLine();

    bool join;

    while (! isComplete(line, join)) {
      if (in.atEnd())
        break;

      QString line1 = in.readLine();

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    processLine(line);
  }

  file.close();

  return true;
}

template<typename IS_COMPLETE>
QStringList stringToLines(const QString &str, IS_COMPLETE isComplete) {
  QStringList lines = str.split('\n', Qt::SkipEmptyParts);

  QStringList lines1;

  int i = 0;

  for ( ; i < lines.size(); ++i) {
    auto line = lines[i];

    bool join;

    while (! isComplete(line, join)) {
      ++i;

      if (i >= lines.size())
        break;

      const QString &line1 = lines[i];

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    lines1.push_back(std::move(line));
  }

  return lines1;
}

template<typename IS_COMPLETE, typename PROCESS_LINE>
void readLineLoop(CQChartsReadLine *readLine, IS_COMPLETE isComplete, PROCESS_LINE processLine) {
  for (;;) {
    readLine->setPrompt("> ");

    QString line = readLine->readLine().c_str();

    bool join;

    while (! isComplete(line, join)) {
      readLine->setPrompt("+> ");

      QString line1 = readLine->readLine().c_str();

      if (! join)
        line += "\n" + line1;
      else
        line += line1;
    }

    processLine(line);

    readLine->addHistory(line.toStdString());
  }
}

}

#endif
