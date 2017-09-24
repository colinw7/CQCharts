#ifndef CQTsvModel_H
#define CQTsvModel_H

#include <QAbstractItemModel>
#include <vector>

class CQTsvModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(bool commentHeader   READ isCommentHeader   WRITE setCommentHeader  )
  Q_PROPERTY(bool firstLineHeader READ isFirstLineHeader WRITE setFirstLineHeader)

 public:
  CQTsvModel();

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  bool load(const QString &filename);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

 protected:
  typedef std::vector<QString> Cells;
  typedef std::vector<Cells>   Data;

  QString filename_;
  bool    commentHeader_   { false };
  bool    firstLineHeader_ { false };
  int     numColumns_      { 0 };
  Cells   header_;
  Data    data_;
};

#endif
