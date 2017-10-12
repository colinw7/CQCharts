#include <CQPropertyView.h>
#include <CQPropertyViewEditor.h>
#include <CQPropertyViewIntegerType.h>
#include <CQPropertyViewRealType.h>
#include <CQPropertyViewColorType.h>
#include <CQPropertyViewFontType.h>
#include <CQPropertyViewPointFType.h>
#include <CQPropertyViewRectFType.h>
#include <CQPropertyViewSizeFType.h>
//#include <CQPropertyViewPaletteType.h>
#include <CQPropertyViewAlignType.h>
//#include <CQPropertyViewLineDashType.h>
//#include <CQPropertyViewAngleType.h>
#include <cassert>

CQPropertyViewMgr *
CQPropertyViewMgr::
instance()
{
  static CQPropertyViewMgr *instance;

  if (! instance)
    instance = new CQPropertyViewMgr;

  return instance;
}

CQPropertyViewMgr::
CQPropertyViewMgr()
{
  editorMgr_ = new CQPropertyViewEditorMgr;

  addType("int"          , new CQPropertyViewIntegerType );
  addType("double"       , new CQPropertyViewRealType    );
  addType("QColor"       , new CQPropertyViewColorType   );
  addType("QFont"        , new CQPropertyViewFontType    );
  addType("QPointF"      , new CQPropertyViewPointFType  );
  addType("QRectF"       , new CQPropertyViewRectFType   );
  addType("QSizeF"       , new CQPropertyViewSizeFType   );
//addType("QPalette"     , new CQPropertyViewPaletteType );
  addType("Qt::Alignment", new CQPropertyViewAlignType   );
//addType("CLineDash"    , new CQPropertyViewLineDashType);
//addType("CAngle"       , new CQPropertyViewAngleType   );
}

CQPropertyViewMgr::
~CQPropertyViewMgr()
{
  delete editorMgr_;

  for (auto &type : types_)
    delete type.second;
}

void
CQPropertyViewMgr::
addType(const QString &name, CQPropertyViewType *type)
{
  assert(! getType(name));

  types_[name] = type;

  editorMgr_->setEditor(name, type->getEditor());
}

CQPropertyViewType *
CQPropertyViewMgr::
getType(const QString &name) const
{
  auto p = types_.find(name);

  if (p == types_.end())
    return nullptr;

  return (*p).second;
}

CQPropertyViewEditorFactory *
CQPropertyViewMgr::
getEditor(const QString &name) const
{
  CQPropertyViewType *type = getType(name);

  if (! type)
    return nullptr;

  return type->getEditor();
}
