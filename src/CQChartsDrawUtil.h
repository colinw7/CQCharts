#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <QString>

class QPainter;

namespace CQChartsDrawUtil {

void drawAlignedText(QPainter *painter, double x, double y, const QString &text,
                     Qt::Alignment align, double dx=0, double dy=0);

}

#endif
