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
#include "c80_highlighter.h"

C80Highlighter::C80Highlighter(QTextDocument* parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;
  this->labels = NULL;
  this->variables = NULL;

  // Keywords
  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\bmov\\b"
                  << "\\bmvi\\b"
                  << "\\blxi\\b"
                  << "\\bsta\\b"
                  << "\\blda\\b"
                  << "\\blhld\\b"
                  << "\\bshld\\b"
                  << "\\bldax\\b"
                  << "\\bstax\\b"
                  << "\\bxchg\\b"
                  << "\\bana\\b"
                  << "\\bani\\b"
                  << "\\bxra\\b"
                  << "\\bxri\\b"
                  << "\\bora\\b"
                  << "\\bori\\b"
                  << "\\bcmp\\b"
                  << "\\bpush\\b"
                  << "\\bpsw\\b"
                  << "\\bdop\\b"
                  << "\\bpop\\b"
                  << "\\bxthl\\b"
                  << "\\bsphl\\b"
                  << "\\bin\\b"
                  << "\\bout\\b"
                  << "\\bei\\b"
                  << "\\bdi\\b"
                  << "\\bjz\\b"
                  << "\\bjnz\\b"
                  << "\\bjc\\b"
                  << "\\bjnc\\b"
                  << "\\bjmp\\b"
                  << "\\brlc\\b"
                  << "\\brrc\\b"
                  << "\\bral\\b"
                  << "\\brar\\b"
                  << "\\bcma\\b"
                  << "\\bcmc\\b"
                  << "\\bstc\\b"
                  << "\\bjp\\b"
                  << "\\bjm\\b"
                  << "\\bjpe\\b"
                  << "\\bjpo\\b"
                  << "\\bje\\b"
                  << "\\bjne\\b"
                  << "\\bcall\\b"
                  << "\\bret\\b"
                  << "\\brst\\b"
                  << "\\bpchl\\b"
                  << "\\badd\\b"
                  << "\\badi\\b"
                  << "\\badc\\b"
                  << "\\baci\\b"
                  << "\\bsub\\b"
                  << "\\bsui\\b"
                  << "\\bsbb\\b"
                  << "\\bsbi\\b"
                  << "\\binr\\b"
                  << "\\bdsr\\b"
                  << "\\binx\\b"
                  << "\\bcpi\\b"
                  << "\\bdcr\\b"
                  << "\\bhlt\\b"
                  << "\\bdcx\\b"
                  << "\\bdad\\b"
                  << "\\bcz\\b"
                  << "\\bcnz\\b"
                  << "\\bcp\\b"
                  << "\\bcm\\b"
                  << "\\bcc\\b"
                  << "\\bcnc\\b"
                  << "\\bcpe\\b"
                  << "\\bcpo\\b"
                  << "\\bdaa\\b"
                  << "\\bnop\\b"
                  << "\\brz\\b"
                  << "\\brnz\\b"
                  << "\\brp\\b"
                  << "\\brm\\b"
                  << "\\brc\\b"
                  << "\\brnc\\b"
                  << "\\brpe\\b"
                  << "\\brpo\\b";
  foreach (const QString& pattern, keywordPatterns) {
    rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  // DB and DW
  defineFormat.setForeground(Qt::darkMagenta);
  defineFormat.setFontWeight(QFont::Bold);
  keywordPatterns.clear();
  keywordPatterns << "\\bdb\\b"
                  << "\\bdw\\b";
  foreach (const QString& pattern, keywordPatterns) {
    rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
    rule.format = defineFormat;
    highlightingRules.append(rule);
  }

  // Translator directive
  this->translatorDirectiveFormat.setBackground(Qt::darkRed);
  this->translatorDirectiveFormat.setForeground(Qt::yellow);
  rule.pattern = QRegExp("^[\t\\ ]*#[\\w]+");
  rule.format = this->translatorDirectiveFormat;
  highlightingRules.append(rule);

  // Labels
  labelFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("^[\t\\ ]*[\\w]+:");
  rule.format = labelFormat;
  highlightingRules.append(rule);

  // Comments
  singleLineCommentFormat.setForeground(Qt::red);
  rule.pattern = QRegExp(";[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  // Register names
  this->registersFormat.setForeground(Qt::blue);
  this->registersFormat.setFontWeight(QFont::Bold);
  keywordPatterns.clear();
  keywordPatterns << "\\bA\\b"
                  << "\\bB\\b"
                  << "\\bC\\b"
                  << "\\bD\\b"
                  << "\\bE\\b"
                  << "\\bDE\\b"
                  << "\\bBC\\b"
                  << "\\bSP\\b"
                  << "\\bHL\\b"
                  << "\\bH\\b"
                  << "\\bL\\b"
                  << "\\bM\\b";

  foreach (const QString& pattern, keywordPatterns) {
    rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
    rule.format = registersFormat;
    highlightingRules.append(rule);
  }

  // Characters
  symbolFormat.setForeground(QColor("orange"));
  rule.pattern = QRegExp("\\'.\\'");
  rule.format = symbolFormat;
  highlightingRules.append(rule);

  // Variables
  this->variablesFormat.setForeground(Qt::yellow);
  this->variablesFormat.setBackground(Qt::darkGreen);
}

void
C80Highlighter::highlightBlock(const QString& text)
{
  foreach (const HighlightingRule& rule, highlightingRules) {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
}

void
C80Highlighter::setLabels(QStringList* labels)
{
  this->labels = labels;
}

void
C80Highlighter::setVariables(QStringList* variables)
{
  this->variables = variables;
}

void
C80Highlighter::highlightThis(const QString& text)
{
  this->highlightBlock(text);
}
