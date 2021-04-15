#include <CQChartsIconButton.h>
#include <CQPixmapCache.h>
#include <CQStyleMgr.h>

CQChartsIconButton::
CQChartsIconButton(QWidget *parent) :
 QToolButton(parent)
{
  connect(CQStyleMgrInst, SIGNAL(themeChanged()), this, SLOT(updateIcon()));
}

void
CQChartsIconButton::
setIcon(const QString &iconName)
{
  iconName_ = iconName;

  updateIcon();
}

void
CQChartsIconButton::
updateIcon()
{
  if (CQPixmapCacheInst->hasPixmap(iconName_ + "_LIGHT"))
    QToolButton::setIcon(CQPixmapCacheInst->getIcon(iconName_ + "_LIGHT", iconName_ + "_DARK"));
  else
    QToolButton::setIcon(CQPixmapCacheInst->getIcon(iconName_));
}
