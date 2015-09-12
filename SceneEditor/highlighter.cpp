/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    typeFormat.setFontItalic(true);
    typeFormat.setForeground(QColor(102,217,239));
    //typeFormat.setFontWeight(QFont::Bold);


    QStringList typePatterns;
    typePatterns << "\\bfloat\\b" << "\\bdouble\\b" << "\\bvec2\\b"  << "\\bbool\\b" << "\\bint\\b"
                    << "\\bvec3\\b" << "\\bvec4\\b" << "\\bmat2\\b"
                    << "\\bmat3\\b" << "\\bmat4\\b" << "\\bin\\b"
                    << "\\out\\b" << "\\binout\\b" << "\\bvoid\\b"
                    << "\\buniform\\b" << "\\bvarying\\b" << "\\battribute\\b" << "\\bstruct\\b"
                    << "\\bsampler1D\\b" << "\\bsampler2D\\b" << "\\bsampler3D\\b" << "\\bgl_FragColor\\b";
    foreach (const QString &pattern, typePatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    keywordFormat.setForeground(QColor(249,38,114));
    QStringList keywordPatterns;
    keywordPatterns << "\\bfor\\b" << "\\bdo\\b" << "\\bwhile\\b" <<
                       "\\bbreak\\b" << "\\breturn\\b" << "\\bif\\b" <<
                       "\\belse\\b" << "\\bconst\\b" << "\\bswitch\\b" << "\\bcase\\b"<< "\\bdiscard\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGray);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setForeground(QColor(166,226,46));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    //Number
    numberFormat.setForeground(QColor(174,129,255));
    rule.format = numberFormat;
    rule.pattern = QRegExp("(-?\\d+\\.\\d+)e-?\\d");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("(-?\\d+\\.)e-?\\d");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("(-?\\.\\d+)e-?\\d");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("(-?\\d+\\.\\d+)");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("(-?\\d+\\.)");
    highlightingRules.append(rule);
    rule.pattern = QRegExp("(-?\\.\\d+)");
    highlightingRules.append(rule);

}

void Highlighter::defineXMLRule()
{
    highlightingRules.clear();
    //XML
    HighlightingRule rule;

    XMLNameFormat.setForeground(QColor(249,38,114));
    rule.pattern = QRegExp("([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*");
    rule.format = XMLNameFormat;
    highlightingRules.append(rule);

    XMLAttributeFormat.setForeground(QColor(166,226,46));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\=)");
    rule.format = XMLAttributeFormat;
    highlightingRules.append(rule);

    XMLParamFormat.setForeground(QColor(230,219,116));
    rule.pattern = QRegExp("\"[^<\"]*\"|'[^<']*'");
    rule.format = XMLParamFormat;
    highlightingRules.append(rule);

}




void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
