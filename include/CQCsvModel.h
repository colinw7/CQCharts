#ifndef CQCsvModel_H
#define CQCsvModel_H

#include <CQDataModel.h>

class CQCsvModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(bool commentHeader     READ isCommentHeader     WRITE setCommentHeader    )
  Q_PROPERTY(bool firstLineHeader   READ isFirstLineHeader   WRITE setFirstLineHeader  )
  Q_PROPERTY(bool firstColumnHeader READ isFirstColumnHeader WRITE setFirstColumnHeader)
  Q_PROPERTY(char separator         READ separator           WRITE setSeparator        )

 public:
  CQCsvModel();

  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  bool isFirstColumnHeader() const { return firstColumnHeader_; }
  void setFirstColumnHeader(bool b) { firstColumnHeader_ = b; }

  const char &separator() const { return separator_; }
  void setSeparator(const char &v) { separator_ = v; }

  bool load(const QString &filename);

  void save(std::ostream &os);
  void save(QAbstractItemModel *model, std::ostream &os);

 protected:
  std::string encodeVariant(const QVariant &var) const;

  QString encodeString(const QString &str) const;

 protected:
  QString filename_;                    // input filename
  bool    commentHeader_     { false }; // first comment line has column names
  bool    firstLineHeader_   { false }; // first non-comment line has column names
  bool    firstColumnHeader_ { false }; // first column in each line is row name
  char    separator_         { ',' };   // field separator
};

#endif
