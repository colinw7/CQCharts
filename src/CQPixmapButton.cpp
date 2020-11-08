#include <CQPixmapButton.h>

CQPixmapButton::
CQPixmapButton(const QString &fileName)
{
  QPixmap pixmap(fileName);

  init(pixmap);
}

CQPixmapButton::
CQPixmapButton(uchar *data, uint len)
{
  QPixmap pixmap;

  pixmap.loadFromData(data, len);

  init(pixmap);
}

CQPixmapButton::
CQPixmapButton(const QPixmap &pixmap)
{
  init(pixmap);
}

CQPixmapButton::
CQPixmapButton(const QIcon &icon)
{
  init(icon);
}

void
CQPixmapButton::
init(const QPixmap &pixmap)
{
  size_ = pixmap.size();

  QIcon icon(pixmap);

  setIcon(icon);

  setFixedSize(sizeHint());

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
CQPixmapButton::
init(const QIcon &icon)
{
  size_ = iconSize();

  setIcon(icon);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
CQPixmapButton::
setMargin(int i)
{
  margin_ = i;

  setFixedSize(sizeHint());
}

QSize
CQPixmapButton::
sizeHint() const
{
  return size_ + QSize(margin(), margin());
}

QSize
CQPixmapButton::
minimumSizeHint () const
{
  return size_ + QSize(margin(), margin());
}
