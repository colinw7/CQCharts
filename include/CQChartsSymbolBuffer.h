#ifndef CQChartsSymbolBuffer_H
#define CQChartsSymbolBuffer_H

#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <QImage>
#include <map>
#include <set>

#define CQChartsSymbolBufferInst CQChartsSymbolBuffer::instance()

class CQChartsSymbolBuffer {
 public:
  using Symbol = CQChartsSymbol;
  using Length = CQChartsLength;
  using Point  = CQChartsGeom::Point;

 public:
  static CQChartsSymbolBuffer *instance();

 ~CQChartsSymbolBuffer();

  QImage getImage(const Symbol &symbol, double size, const QPen &pen, const QBrush &brush);

 private:
  CQChartsSymbolBuffer();

 private:
  struct ImageBuffer {
    Symbol symbol;
    double size { 0.0 };
    int    isize { 0 };
    QPen   pen;
    QBrush brush;
    QImage image;
  };

  using ImageBufferList = std::vector<ImageBuffer *>;
  using ImageBufferMap  = std::map<Symbol, ImageBufferList>;

  ImageBufferMap imageBufferMap_;
};

#endif
