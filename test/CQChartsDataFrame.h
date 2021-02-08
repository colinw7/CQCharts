#ifndef CQChartsDataFrame_H
#define CQChartsDataFrame_H

#ifdef CQCHARTS_DATA_FRAME

#include <QStringList>

class CQChartsDataFrameCmd;

namespace CQDataFrame {

class Frame;

QStringList completeFile(const QString &file);

Frame *initFrame(CQChartsDataFrameCmd *cmd);

void showFrame(CQChartsDataFrameCmd *cmd);

void loadFrameFile(const QString &fileName);

}

#endif

#endif
