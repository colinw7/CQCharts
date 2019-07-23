#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <CQChartsSymbol.h>
#include <QString>
#include <QRectF>
#include <QFont>

class QPainter;

namespace CQChartsDrawUtil {

void drawTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                   const CQChartsTextOptions &options);

void drawRotatedTextInBox(QPainter *painter, const QRectF &rect, const QString &text,
                          const QPen &pen, const CQChartsTextOptions &options);

void drawTextAtPoint(QPainter *painter, const QPointF &point, const QString &text,
                     const CQChartsTextOptions &options=CQChartsTextOptions());

void drawAlignedText(QPainter *painter, double x, double y, const QString &text,
                     Qt::Alignment align, double dx=0, double dy=0);

QRectF calcAlignedTextRect(const QFont &font, double x, double y, const QString &text,
                           Qt::Alignment align, double dx, double dy);

void drawContrastText(QPainter *painter, double x, double y, const QString &text);

QSizeF calcTextSize(const QString &text, const QFont &font, const CQChartsTextOptions &options);

void drawCenteredText(QPainter *painter, const QPointF &pos, const QString &text);

void drawSimpleText(QPainter *painter, double x, double y, const QString &text);

void drawSimpleText(QPainter *painter, const QPointF &pos, const QString &text);

void drawSymbol(QPainter *painter, const CQChartsSymbol &symbol, const QRectF &rect);
void drawSymbol(QPainter *painter, const CQChartsSymbol &symbol,
                const QPointF &c, const QSizeF &size);
void drawSymbol(QPainter *painter, const CQChartsSymbol &symbol,
                double cx, double cy, double sw, double sh);

}

//---

namespace CQChartsDrawPrivate {

// private
QSizeF calcHtmlTextSize(const QString &text, const QFont &font, int margin=0);

void drawScaledHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
                        const CQChartsTextOptions &options);

void drawHtmlText(QPainter *painter, const QRectF &trect, const QString &text,
                  const CQChartsTextOptions &options);

}

#endif
