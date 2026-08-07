#include "TraceParser.h"

#include <QRegularExpression>
#include <QStringList>

QVector<TraceLine> TraceParser::parse(const QString &text)
{
    QVector<TraceLine> lines;
    const QStringList rawLines = text.split(QRegularExpression(QStringLiteral(R"(\r?\n)")), Qt::KeepEmptyParts);
    QRegularExpression expression(QStringLiteral(R"(^\s*([RrWw])\s*(?:0[xX])?([0-9A-Fa-f]+)\s*$)"));

    int lineNumber = 0;
    for (const QString &rawLine : rawLines) {
        ++lineNumber;
        const QString trimmed = rawLine.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }

        TraceLine line;
        line.lineNumber = lineNumber;
        line.rawText = trimmed;

        const QRegularExpressionMatch match = expression.match(trimmed);
        if (!match.hasMatch()) {
            line.valid = false;
            line.errorText = QStringLiteral("Invalid trace line");
            lines.push_back(line);
            continue;
        }

        bool ok = false;
        const QString op = match.captured(1).toUpper();
        const QString addressText = match.captured(2);
        const quint64 address = addressText.toULongLong(&ok, 16);
        if (!ok) {
            line.valid = false;
            line.errorText = QStringLiteral("Invalid hexadecimal address");
            lines.push_back(line);
            continue;
        }

        line.valid = true;
        line.type = op == QStringLiteral("W") ? AccessType::Write : AccessType::Read;
        line.address = address;
        lines.push_back(line);
    }

    return lines;
}

