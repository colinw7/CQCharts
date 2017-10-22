#ifndef CQTsvModel_H
#define CQTsvModel_H

#include <CQDataModel.h>
#include <vector>

class CQTsvModel : public CQDataModel {
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

 protected:
  QString filename_;                  // input filename
  bool    commentHeader_   { false }; // first comment line has column names
  bool    firstLineHeader_ { false }; // first non-comment line has column names
};

#endif
