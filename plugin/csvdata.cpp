/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "csvdata.h"

#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QProgressDialog>
#include <QTextStream>
#include <QWidget>

#include "interactivemessagehandler.h"

CSVData::CSVData(QObject * parent, const char * name)
  : QObject(parent)
{
  setObjectName(name ? name : "_CSVData");
  _firstRowHeaders = FALSE;
  _numColumns = 0;
  _stopped    = false;
  _msghandler = new InteractiveMessageHandler(this);
}

CSVData::~CSVData() {
}

void CSVData::setFirstRowHeaders(bool y)
{
  if(_firstRowHeaders != y)
  {
    _firstRowHeaders = y;
  }
}

void CSVData::setMessageHandler(XAbstractMessageHandler *handler)
{
  _msghandler = handler;
}

unsigned int CSVData::rows()
{
  int n = _rows.count();
  if(_firstRowHeaders)
    --n;
  if(n < 0)
    n = 0;
  return n;
}

QString CSVData::header(int column)
{
  if(_firstRowHeaders)
  {
    if(_rows.count() > 0)
    {
      QStringList cols = _rows[0];
      if(column < cols.count())
      {
        return cols[column];
      }
    }
  }

  return QString::null;
}

QString CSVData::value(int row, int column)
{
  if(_firstRowHeaders)
    ++row;

  if(row < _rows.count())
  {
    QStringList cols = _rows[row];
    if(column < cols.count())
    {
      return cols[column];
    }
  }

  return QString::null;
}

bool CSVData::load(QString filename, QWidget * parent)
{
  QFile file;

  file.setFileName(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    _msghandler->message(QtWarningMsg, tr("Open Failed"),
                         tr("<p>Could not open %1 for reading: %2")
                         .arg(filename, file.errorString()));
    return FALSE;
  }

  QString progresstext(tr("Loading %1: %2 bytes out of %3, %4 records"));
  QProgressDialog *progress = 0;
  _stopped = false;
  int expected = file.size();
  if (parent)
  {
    progress = new QProgressDialog(progresstext
                                     .arg(filename).arg(0).arg(expected).arg(0),
                                   tr("Stop"), 0, expected, parent);
    progress->setWindowModality(Qt::WindowModal);
    connect(progress, SIGNAL(canceled()), this, SLOT(sUserCanceled()));
  }

  QTextStream in(&file);

  int  lines   = 0;
  bool inQuote = FALSE;
  bool haveText = FALSE;
  bool peeked = FALSE;
  QString field = QString::null;
  QChar c = QChar();
  QStringList row = QStringList();

  for (int actual = 0; ! in.atEnd() && ! _stopped; actual++)
  {
    if(peeked)
      peeked = FALSE;
    else
      in >> c;

    // If we are inside a quoted string we handle
    // everything differently
    if(inQuote)
    {
      if('"' == c)
      {
        in >> c;
        if('"' == c)
          field += c;
        else
        {
          peeked = TRUE;
          inQuote = FALSE;
        }
      }
      else
        field += c;
    }
    else
    {
      if(',' == c || '\r' == c || '\n' == c) 
      {
        // end of field processing
        if(!field.isNull() && haveText)
          field = field.trimmed();

        row.append(field);

        field = QString::null;
        inQuote = FALSE;
        haveText = FALSE;

        if('\r' == c || '\n' == c)
        {
          if('\r' == c)
          {
            in >> c;
            if('\n' != c)
              peeked = TRUE;
          }

          // end of line processing
          _numColumns = qMax(_numColumns, row.count());
          _rows.append(row);
          row = QStringList();
          if (progress)
          {
            progress->setValue(actual);
            progress->setLabelText(progresstext
                         .arg(filename).arg(actual).arg(expected).arg(++lines));
          }
        }
      }
      else if('"' == c)
      {
        inQuote = TRUE; 
        if(field.isNull())
          field = QString("");
      }
      else if(c.isSpace() && haveText)
        field += c;
      else if(!c.isSpace())
      {
        haveText = TRUE;
        field += c;
      }
    }
  }

  // Make sure any left over data is properly added to
  // the lists of information
  if(!field.isNull())
  {
    if(haveText)
      field = field.trimmed();
    row.append(field);
  }
  if(!row.isEmpty())
  {
    _numColumns = qMax(_numColumns, row.count());
    _rows.append(row);
  }

  if (progress)
    progress->setValue(expected);

  return TRUE;
}

XAbstractMessageHandler *CSVData::messageHandler() const
{
  return _msghandler;
}

void CSVData::sUserCanceled()
{
  _stopped = true;
}
