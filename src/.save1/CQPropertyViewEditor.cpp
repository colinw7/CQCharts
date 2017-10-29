#include <CQPropertyViewEditor.h>

CQPropertyViewEditorMgr::
CQPropertyViewEditorMgr()
{
}

CQPropertyViewEditorMgr::
~CQPropertyViewEditorMgr()
{
  for (auto &editor : editors_)
    delete editor.second;
}

void
CQPropertyViewEditorMgr::
setEditor(const QString &typeName, CQPropertyViewEditorFactory *editor)
{
  Editors::iterator p = editors_.find(typeName);

  if (p != editors_.end()) {
    std::swap((*p).second, editor);

    delete editor;
  }
  else
    (void) editors_.insert(p, Editors::value_type(typeName, editor));
}

CQPropertyViewEditorFactory *
CQPropertyViewEditorMgr::
getEditor(const QString &typeName) const
{
  Editors::const_iterator p = editors_.find(typeName);

  if (p != editors_.end())
    return (*p).second;
  else
    return 0;
}
