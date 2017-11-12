#ifndef CQCsvModel_H
#define CQCsvModel_H

#include <CQDataModel.h>
#include <vector>

class CQCsvModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(bool    commentHeader   READ isCommentHeader   WRITE setCommentHeader  )
  Q_PROPERTY(bool    firstLineHeader READ isFirstLineHeader WRITE setFirstLineHeader)
  Q_PROPERTY(QString filter          READ filter            WRITE setFilter         )

 public:
  CQCsvModel();

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter) { filter_ = filter; }

  bool load(const QString &filename);

 private:
  void initFilter();

  bool acceptsRow(const std::vector<std::string> &cells) const;

 protected:
  struct FilterData {
    int     column { -1 };
    QRegExp regexp;
    bool    valid  { false };
  };

  typedef std::vector<FilterData> FilterDatas;

  QString     filename_;                  // input filename
  bool        commentHeader_   { false }; // first comment line has column names
  bool        firstLineHeader_ { false }; // first non-comment line has column names
  QString     filter_;                    // filter text
  FilterDatas filterDatas_;               // filter datas
};

#endif
