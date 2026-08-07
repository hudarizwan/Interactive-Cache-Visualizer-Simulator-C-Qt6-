#include "MainWindow.h"

#include <QApplication>
#include <QPalette>

static void setBasicPalette(QApplication &app)
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(QStringLiteral("#f3f4f6")));
    palette.setColor(QPalette::WindowText, QColor(QStringLiteral("#111827")));
    palette.setColor(QPalette::Base, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::AlternateBase, QColor(QStringLiteral("#f9fafb")));
    palette.setColor(QPalette::ToolTipBase, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::ToolTipText, QColor(QStringLiteral("#111827")));
    palette.setColor(QPalette::Text, QColor(QStringLiteral("#111827")));
    palette.setColor(QPalette::Button, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::ButtonText, QColor(QStringLiteral("#111827")));
    palette.setColor(QPalette::BrightText, QColor(QStringLiteral("#000000")));
    palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#2563eb")));
    palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#ffffff")));
    app.setPalette(palette);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Cache Simulator Lab"));
    app.setOrganizationName(QStringLiteral("USTP"));
    setBasicPalette(app);

    MainWindow window;
    window.show();

    return app.exec();
}
