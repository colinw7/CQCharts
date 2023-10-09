#include <CQHtmlTextPainter.h>

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QPainterPath>

#include <iostream>

CQHtmlTextPainter::
CQHtmlTextPainter()
{
  setObjectName("htmlTextPainter");
}

void
CQHtmlTextPainter::
drawScaledInRect(QPainter *painter, const QRect &rect)
{
  auto origFont    = font();
  auto currentFont = origFont;

  int iter = 0;

  while (iter < 20) {
    font_ = currentFont;

    auto psize = textSize();

    auto pw = psize.width ();
    auto ph = psize.height();
    if (pw <= 0.0 || ph <= 0) return;

    auto xs = rect.width ()/pw;
    auto ys = rect.height()/ph;

    auto s1 = std::min(xs, ys);

    if (std::abs(s1 - 1.0) < 1E-5)
      break;

    auto ps = currentFont.pointSizeF();
    currentFont.setPointSizeF(s1*ps);

    ++iter;
  }

  //---

  bool align = false;

  font_ = currentFont;

  std::swap(align, align_);

  drawInRect(painter, rect);

  std::swap(align, align_);

  font_ = origFont;
}

void
CQHtmlTextPainter::
drawInRect(QPainter *painter, const QRect &rect)
{
  drawInRect(painter, rect.center(), rect);
}

void
CQHtmlTextPainter::
drawInRect(QPainter *painter, const QPoint &center, const QRect &rect)
{
  painter->save();

  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (angle() != 0.0) {
    painter->translate(center);
    painter->rotate(-angle());
    painter->translate(-center);
  }

  QTextDocument doc;

  doc.setDocumentMargin(margin());
  doc.setHtml(text());
  doc.setDefaultFont(font());

  painter->translate(rect.left(), rect.top());

  if (isClip()) {
    QRect clip(0, 0, rect.width(), rect.height());
    painter->setClipRect(clip, Qt::IntersectClip);
  }

  if (isAlign()) {
    doc.setTextWidth(rect.width());
    doc.setPageSize(rect.size());

    QTextCursor cursor(&doc);

    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    auto pos = cursor.position();
    while (true) {
      //std::cerr << pos << "\n";
      QTextBlockFormat f;
      f.setAlignment(alignment());
      cursor.setBlockFormat(f);

      cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 1);
      auto pos1 = cursor.position();
      if (pos1 == pos) break;
      pos = pos1;
    }

    //cursor.select(QTextCursor::Document);
    //QTextBlockFormat f;
    //f.setAlignment(alignment());
    //cursor.setBlockFormat(f);
  }

  QAbstractTextDocumentLayout::PaintContext ctx;

  auto *layout = doc.documentLayout();

  layout->setPaintDevice(painter->device());

  if (isContrast()) {
    auto bwColor = [](const QColor &c) {
      int g = qGray(c.red(), c.green(), c.blue());

      return (g > 128 ? QColor(Qt::black) : QColor(Qt::white));
    };

    auto ipc = bwColor(textColor());

    ctx.palette.setColor(QPalette::Text, ipc);

    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        if (dx != 0 || dy != 0) {
          painter->translate(dx, dy);

          layout->draw(painter, ctx);

          painter->translate(-dx, -dy);
        }
      }
    }

    ctx.palette.setColor(QPalette::Text, textColor());

    layout->draw(painter, ctx);
  }
  else {
    ctx.palette.setColor(QPalette::Text, textColor());

    layout->draw(painter, ctx);
  }

  painter->restore();
}

void
CQHtmlTextPainter::
drawTextBg(QPainter *painter, const QRect &rect)
{
  auto size = textSize();

  painter->setBrush(Qt::red);

  painter->drawRect(QRect(rect.x(), rect.y(), size.width(), size.height()));
}

QSizeF
CQHtmlTextPainter::
textSize() const
{
  QTextDocument doc;

  doc.setDocumentMargin(margin());
  doc.setHtml(text());
  doc.setDefaultFont(font());

  auto *layout = doc.documentLayout();

  return layout->documentSize();
}

QSizeF
CQHtmlTextPainter::
textWidthSize(int width) const
{
  QTextDocument doc;

  doc.setDocumentMargin(margin());
  doc.setHtml(text());
  doc.setDefaultFont(font());

  doc.setTextWidth(width);

  auto *layout = doc.documentLayout();

  return layout->documentSize();
}
