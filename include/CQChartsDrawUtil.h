#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <QString>
#include <QRectF>
#include <QFont>

class QPainter;

namespace CQChartsDrawUtil {

void drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                   const QPen &pen, const CQChartsTextOptions &options);

void drawRotatedTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                          const QPen &pen, const CQChartsTextOptions &options);

void drawTextAtPoint(QPainter *painter, const QPointF &point, const QString &text,
                     const QPen &pen, const CQChartsTextOptions &options=CQChartsTextOptions());

void drawAlignedText(QPainter *painter, double x, double y, const QString &text,
                     Qt::Alignment align, double dx=0, double dy=0);

QRectF calcAlignedTextRect(const QFont &font, double x, double y, const QString &text,
                           Qt::Alignment align, double dx, double dy);

void drawContrastText(QPainter *painter, double x, double y, const QString &text, const QPen &pen);

QSizeF calcTextSize    (const QString &text, const QFont &font);
QSizeF calcHtmlTextSize(const QString &text, const QFont &font);

void drawScaledHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
                        const CQChartsTextOptions &options);

void drawHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
                  const CQChartsTextOptions &options);

void drawCenteredText(QPainter *painter, const QPointF &pos, const QString &text);

void drawSimpleText(QPainter *painter, double x, double y, const QString &text);

void drawSimpleText(QPainter *painter, const QPointF &pos, const QString &text);

}

#endif
