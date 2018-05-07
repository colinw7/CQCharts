#include <CQFontListView.h>

CQFontListView::
CQFontListView(QWidget *parent) :
QListView(parent)
{
  setModel(new QStringListModel(parent));

  setEditTriggers(NoEditTriggers);
}
