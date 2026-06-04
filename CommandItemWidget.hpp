#ifndef COMMANDITEMWIDGET_HPP
#define COMMANDITEMWIDGET_HPP

#include <QLabel>
#include <QWidget>

class CommandItemWidget : public QWidget {
  Q_OBJECT

public:
  explicit CommandItemWidget(const QString &command = QString(),
                             const QString &remark = QString(),
                             QWidget *parent = nullptr);

  void setCommand(const QString &command);
  void setRemark(const QString &remark);
  void setContent(const QString &command, const QString &remark);

  QString command() const;
  QString remark() const;

  QSize sizeHint() const override;

private:
  QLabel *commandLabelPtr;
  QLabel *remarkLabelPtr;
  QString _command;
  QString _remark;
};

#endif // COMMANDITEMWIDGET_HPP
