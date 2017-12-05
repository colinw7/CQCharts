#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <QString>

class CQChartsRenderer;

namespace CQChartsDrawUtil {

void drawAlignedText(CQChartsRenderer *renderer, double x, double y, const QString &text,
                     Qt::Alignment align, double dx=0, double dy=0);

}

#endif
