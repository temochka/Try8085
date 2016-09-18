/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Ilya Beda <ir4y.ix@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>

  This file is part of Try8085.

  Try8085 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Try8085 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Try8085.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TRY8085_EDITOR_WIDGET_H
#define TRY8085_EDITOR_WIDGET_H
#include "c80_highlighter.h"
#include <QDebug>
#include <QPainter>
#include <QPlainTextEdit>

class EditorWidget : public QPlainTextEdit
{
  Q_OBJECT
public:
  EditorWidget(QWidget* parent = 0);

  void lineNumberAreaPaintEvent(QPaintEvent* event);
  int lineNumberAreaWidth();

protected:
  void resizeEvent(QResizeEvent* event);

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect&, int);
  void parseSyntax();
public slots:
  void goToLine(int lineNumber);
  void updateHighlighting();

private:
  QStringList* labels;
  QStringList* variables;
  C80Highlighter* highlighter;
  QWidget* lineNumberArea;
signals:
  void parsingFinished();
};

class LineNumberAreaWidget : public QWidget
{
public:
  LineNumberAreaWidget(EditorWidget* editor)
    : QWidget(editor)
  {
    codeEditor = editor;
  }

  QSize sizeHint() const { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
  void paintEvent(QPaintEvent* event)
  {
    codeEditor->lineNumberAreaPaintEvent(event);
  }

private:
  EditorWidget* codeEditor;
};

#endif // TRY8085_EDITOR_WIDGET_H
