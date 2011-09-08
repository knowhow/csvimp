/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "logwindow.h"

#include <QPrinter>
#include <QPrintDialog>

LogWindow::LogWindow(QWidget *parent) : QMainWindow(parent)
{
  setupUi(this);

  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
}

LogWindow::~LogWindow()
{
}

void LogWindow::languageChange()
{
  retranslateUi(this);
}

void LogWindow::sPrint()
{
  QPrinter printer;
  QPrintDialog pdlg(&printer, this);
  if (pdlg.exec() == QDialog::Accepted)
    _log->print(&printer);
}
