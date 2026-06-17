#include "CommandItemWidget.hpp"

#include <QVBoxLayout>

CommandItemWidget::CommandItemWidget(const QString &command,
                                     const QString &remark, QWidget *parent)
    : QWidget(parent), commandLabelPtr(new QLabel(this)),
      remarkLabelPtr(new QLabel(this)) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(4, 4, 4, 4);
  layout->setSpacing(2);

  remarkLabelPtr->setStyleSheet("font-size: 12px; color: #757575;");

  commandLabelPtr->setWordWrap(false);
  remarkLabelPtr->setWordWrap(false);

  layout->addWidget(commandLabelPtr);
  layout->addWidget(remarkLabelPtr);

  setContent(command, remark);
}

void CommandItemWidget::setCommand(const QString &command) {
  _command = command;
  commandLabelPtr->setText(command);
}

void CommandItemWidget::setRemark(const QString &remark) {
  _remark = remark;
  remarkLabelPtr->setText(remark);
}

void CommandItemWidget::setContent(const QString &command,
                                   const QString &remark) {
  setCommand(command);
  setRemark(remark);
}

QString CommandItemWidget::command() const { return _command; }

QString CommandItemWidget::remark() const { return _remark; }

QSize CommandItemWidget::sizeHint() const {
  const QSize layoutSize = layout()->sizeHint();
  return layoutSize + QSize(4, 4);
}
