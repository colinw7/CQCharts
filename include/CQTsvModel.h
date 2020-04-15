#ifndef CQTsvModel_H
#define CQTsvModel_H

#include <CQDataModel.h>
#include <QStringList>

/*!
 * \brief Model containing TSV file data
 */
class CQTsvModel : public CQDataModel {
  Q_OBJECT

  Q_PROPERTY(bool commentHeader     READ isCommentHeader     WRITE setCommentHeader    )
  Q_PROPERTY(bool firstLineHeader   READ isFirstLineHeader   WRITE setFirstLineHeader  )
  Q_PROPERTY(bool firstColumnHeader READ isFirstColumnHeader WRITE setFirstColumnHeader)

 public:
  CQTsvModel();

  //---

  //! get/set use first line comment for horizontal header
  bool isCommentHeader() const { return commentHeader_; }
  void setCommentHeader(bool b) { commentHeader_ = b; }

  //! get/set use first line as horizontal header
  bool isFirstLineHeader() const { return firstLineHeader_; }
  void setFirstLineHeader(bool b) { firstLineHeader_ = b; }

  //! get/set use column for vertical header
  bool isFirstColumnHeader() const { return firstColumnHeader_; }
  void setFirstColumnHeader(bool b) { firstColumnHeader_ = b; }

  //---

  //! get/set column names/numbers to read (also specifies order)
  const QStringList &columns() const { return columns_; }
  void setColumns(const QStringList &v) { columns_ = v; }

  //---

  //! load TSV from specified file
  bool load(const QString &filename);

  //! save model to TSV file
  void save(std::ostream &os);
  void save(QAbstractItemModel *model, std::ostream &os);

  //---

  //! encode string (suitable for TSV value)
  static QString encodeString(const QString &str);

 protected:
  //! encode variant (suitable for TSV value)
  std::string encodeVariant(const QVariant &var) const;

 protected:
  bool        commentHeader_     { false }; //!< first comment line has column names
  bool        firstLineHeader_   { false }; //!< first non-comment line has column names
  bool        firstColumnHeader_ { false }; //!< first column in each line is row name
  QStringList columns_;                     //!< specific columns (and order)
};

#endif
