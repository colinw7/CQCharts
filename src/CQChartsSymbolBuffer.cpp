#include <CQChartsSymbolBuffer.h>
#include <CQChartsUtil.h>
#include <CQChartsPixelPaintDevice.h>

#include <QPainter>
#include <CMathRound.h>

CQChartsSymbolBuffer *
CQChartsSymbolBuffer::
instance()
{
  static CQChartsSymbolBuffer *instance;

  if (! instance)
    instance = new CQChartsSymbolBuffer;

  return instance;
}

CQChartsSymbolBuffer::
CQChartsSymbolBuffer()
{
}

CQChartsSymbolBuffer::
~CQChartsSymbolBuffer()
{
}

QImage
CQChartsSymbolBuffer::
getImage(const Symbol &symbol, double size, const QPen &pen, const QBrush &brush)
{
  auto cmpPen = [](const QPen &pen1, const QPen &pen2) {
    if (pen1.style () != pen2.style ()) return false;
    if (pen1.color () != pen2.color ()) return false;
    if (pen1.widthF() != pen2.widthF()) return false;
    return true;
  };

  auto cmpBrush = [](const QBrush &brush1, const QBrush &brush2) {
    if (brush1.style () != brush2.style ()) return false;
    if (brush1.color () != brush2.color ()) return false;
    return true;
  };

  auto ps = imageBufferMap_.find(symbol);

  if (ps == imageBufferMap_.end())
    ps = imageBufferMap_.insert(ps, ImageBufferMap::value_type(symbol, ImageBufferList()));

  for (const auto *imageBuffer : (*ps).second) {
    if (size == imageBuffer->size &&
        cmpPen  (pen  , imageBuffer->pen) &&
        cmpBrush(brush, imageBuffer->brush)) {
      return imageBuffer->image;
    }
  }

  double pw = std::max(pen.widthF(), 1.0);

  auto *imageBuffer = new ImageBuffer;

  imageBuffer->symbol = symbol;
  imageBuffer->size   = size;
  imageBuffer->isize  = CMathRound::RoundUp(2*(size + pw));
  imageBuffer->pen    = pen;
  imageBuffer->brush  = brush;
  imageBuffer->image  = CQChartsUtil::initImage(QSize(imageBuffer->isize, imageBuffer->isize));

  imageBuffer->image.fill(Qt::transparent);

  QPainter ipainter(&imageBuffer->image);

  ipainter.setRenderHints(QPainter::Antialiasing);

  ipainter.setPen  (imageBuffer->pen  );
  ipainter.setBrush(imageBuffer->brush);

  CQChartsPixelPaintDevice device(&ipainter);

  auto spos  = Point(size + pw, size + pw);
  auto ssize = Length::pixel(size);

  CQChartsDrawUtil::drawSymbol(&device, symbol, spos, ssize);

  (*ps).second.push_back(imageBuffer);

  //imageBuffer->image.save("symbol.png");

  return imageBuffer->image;
}
