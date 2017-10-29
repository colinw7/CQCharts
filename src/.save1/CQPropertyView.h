#ifndef CQPropertyView_H
#define CQPropertyView_H

#include <QString>
#include <map>

class CQPropertyViewType;
class CQPropertyViewEditorMgr;
class CQPropertyViewEditorFactory;

#define CQPropertyViewMgrInst CQPropertyViewMgr::instance()

class CQPropertyViewMgr {
 public:
  static CQPropertyViewMgr *instance();

 ~CQPropertyViewMgr();

  void addType(const QString &name, CQPropertyViewType *type);

  CQPropertyViewType *getType(const QString &name) const;

  CQPropertyViewEditorFactory *getEditor(const QString &name) const;

 private:
  CQPropertyViewMgr();

 private:
  typedef std::map<QString,CQPropertyViewType *> Types;

  Types                    types_;
  CQPropertyViewEditorMgr* editorMgr_ { nullptr };
};

#endif
