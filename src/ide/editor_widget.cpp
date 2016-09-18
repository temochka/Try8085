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
#include "editor_widget.h"

EditorWidget::EditorWidget(QWidget* parent)
  : QPlainTextEdit(parent)
{
  this->highlighter = new C80Highlighter(this->document());
  this->variables = new QStringList;
  this->labels = new QStringList;

  lineNumberArea = new LineNumberAreaWidget(this);

  this->setWordWrapMode(QTextOption::NoWrap);

  connect(this, SIGNAL(blockCountChanged(int)), this,
          SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(const QRect&, int)), this,
          SLOT(updateLineNumberArea(const QRect&, int)));
  connect(this, SIGNAL(cursorPositionChanged()), this,
          SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

int
EditorWidget::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void
EditorWidget::updateLineNumberAreaWidth(int /* newBlockCount */)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void
EditorWidget::updateLineNumberArea(const QRect& rect, int dy)
{
  if (dy)
    lineNumberArea->scroll(0, dy);
  else
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void
EditorWidget::resizeEvent(QResizeEvent* e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea->setGeometry(
    QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void
EditorWidget::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::magenta).lighter(192);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

void
EditorWidget::lineNumberAreaPaintEvent(QPaintEvent* event)
{
  QPainter painter(lineNumberArea);
  painter.fillRect(event->rect(), Qt::lightGray);
  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int)blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                       Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}

void
EditorWidget::goToLine(int lineNumber)
{
  QTextCursor cursor(this->textCursor());
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor,
                      lineNumber - 1);
  this->setTextCursor(cursor);
  this->setFocus();
}

void
EditorWidget::parseSyntax()
{
  QStringList lines = this->toPlainText().split("\n");
  this->labels->clear();
  this->variables->clear();
  QRegExp regexpLabels("^([A-Za-z]{1}[A-Za-z\\_\\d]*)\\:.*$");
  QRegExp regexpVariables("^([A-Za-z]{1}[A-Za-z\\_\\d]*)[\\t ]*\\=.*$");
  for (int i = 0; i < lines.size(); i++) {
    if (regexpLabels.exactMatch(lines.at(i).trimmed())) {
      *labels << "\\b" + regexpLabels.capturedTexts().at(1) + "\\b";
    } else if (regexpVariables.exactMatch(lines.at(i).trimmed())) {
      *variables << "\\b" + regexpVariables.capturedTexts().at(1) + "\\b";
    }
  }
  this->highlighter->setLabels(labels->size() ? labels : NULL);
  this->highlighter->setVariables(variables->size() ? variables : NULL);
  emit this->parsingFinished();
}

void
EditorWidget::updateHighlighting()
{
  this->highlighter->rehighlight();
}
