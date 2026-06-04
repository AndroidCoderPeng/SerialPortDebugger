//
// Created by Administrator on 2025/5/5.
//

#ifndef SAVECOMMANDDIALOG_HPP
#define SAVECOMMANDDIALOG_HPP

#include <QDialog>

#include "GlobalDefinition.hpp"

QT_BEGIN_NAMESPACE

namespace Ui {
class SaveCommandDialog;
}

QT_END_NAMESPACE

class SaveCommandDialog : public QDialog {
  Q_OBJECT

public:
  explicit SaveCommandDialog(QWidget *parent = nullptr,
                             const QString &defaultValue = "",
                             const QString &defaultRemark = "");

  ~SaveCommandDialog() override;

  void onSaveCommandButtonClicked();

  void onCancelButtonClicked();

  DatabaseCommand getInputValue() const;

private:
  Ui::SaveCommandDialog *ui;
};

#endif // SAVECOMMANDDIALOG_HPP
