#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <CQChartsSides.h>
#include <CQChartsGeom.h>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QRectF>
#include <QString>

class CQChartsPaintDevice;

struct CQChartsPenBrush {
  QPen   pen;
  QBrush brush;
};

//---

namespace CQChartsDrawUtil {

void setPenBrush(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush);

void drawRoundedPolygon(CQChartsPaintDevice *device, const QRectF &qrect,
                        const CQChartsLength &xsize=CQChartsLength(),
                        const CQChartsLength &ysize=CQChartsLength(),
                        const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));
void drawRoundedPolygon(CQChartsPaintDevice *device, const QPolygonF &poly,
                        const CQChartsLength &xsize=CQChartsLength(),
                        const CQChartsLength &ysize=CQChartsLength());

void drawTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
                   const CQChartsTextOptions &options);

void drawRotatedTextInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
                          const QPen &pen, const CQChartsTextOptions &options);

void drawTextAtPoint(CQChartsPaintDevice *device, const QPointF &point, const QString &text,
                     const CQChartsTextOptions &options=CQChartsTextOptions());

void drawAlignedText(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
                     Qt::Alignment align, double dx=0, double dy=0);

QRectF calcAlignedTextRect(const QFont &font, const QPointF &p, const QString &text,
                           Qt::Alignment align, double dx, double dy);

void drawContrastText(CQChartsPaintDevice *device, const QPointF &p, const QString &text);

QSizeF calcTextSize(const QString &text, const QFont &font, const CQChartsTextOptions &options);

void drawCenteredText(CQChartsPaintDevice *device, const QPointF &pos, const QString &text);

void drawSimpleText(CQChartsPaintDevice *device, const QPointF &pos, const QString &text);

void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const QPointF &c, const CQChartsLength &size);
void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const QRectF &rect);

void drawPieSlice(CQChartsPaintDevice *device, const CQChartsGeom::Point &c,
                  double ri, double ro, double a1, double a2,
                  bool isInvertX=false, bool isInvertY=false);

}

//---

namespace CQChartsDrawPrivate {

// private
QSizeF calcHtmlTextSize(const QString &text, const QFont &font, int margin=0);

void drawScaledHtmlText(CQChartsPaintDevice *device, const QRectF &trect, const QString &text,
                        const CQChartsTextOptions &options);

void drawHtmlText(CQChartsPaintDevice *device, const QRectF &trect, const QString &text,
                  const CQChartsTextOptions &options);

}

#endif
