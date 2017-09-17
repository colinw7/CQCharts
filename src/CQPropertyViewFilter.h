#ifndef CQPropertyViewFilter_H
#define CQPropertyViewFilter_H

#include <QSortFilterProxyModel>
#include <set>

class CQPropertyView;

class CQPropertyViewFilter : public QSortFilterProxyModel {
  // model indices are from source mode (propertyModel)
 public:
  CQPropertyViewFilter(CQPropertyView *view);

  bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

  bool acceptsItem(const QModelIndex &ind) const;

  bool anyChildMatch(const QModelIndex &parent) const;

#if 0
  void expandMatches();
#endif

  void setFilter(const QString &filter);

 private:
  typedef std::map<QModelIndex,bool> IndexMatches;
  typedef std::set<QModelIndex>      ExpandInds;

  CQPropertyView*      view_ { nullptr };
  QString              filter_;
  QRegExp              regexp_;
  mutable IndexMatches matches_;
  mutable ExpandInds   expand_;
};

#endif
