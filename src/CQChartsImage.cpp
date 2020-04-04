#include <CQChartsImage.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsImage, toString, fromString)

int CQChartsImage::metaTypeId;

//---

void
CQChartsImage::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsImage);

  CQPropertyViewMgrInst->setUserName("CQChartsImage", "symbol");
}

CQChartsImage::
CQChartsImage(const QImage &image) :
 image_(image)
{
  fileName_ = image.text("filename");

  image_.setText("", fileName_);
}

CQChartsImage::
CQChartsImage(const QString &s)
{
  fromString(s);
}

QString
CQChartsImage::
toString() const
{
  return fileName_;
}

bool
CQChartsImage::
fromString(const QString &s)
{
  fileName_ = s;
  image_    = QImage(fileName_);

  image_.setText("", fileName_);

  return true;
}
