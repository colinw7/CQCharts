#include <CQChartsIconButton.h>
#include <CQPixmapCache.h>

CQChartsIconButton::
CQChartsIconButton(QWidget *parent) :
 QToolButton(parent)
{
}

void
CQChartsIconButton::
setIcon(const QString &iconName)
{
  if (CQPixmapCacheInst->hasPixmap(iconName + "_LIGHT"))
    QToolButton::setIcon(CQPixmapCacheInst->getIcon(iconName + "_LIGHT", iconName + "_DARK"));
  else
    QToolButton::setIcon(CQPixmapCacheInst->getIcon(iconName));
}
