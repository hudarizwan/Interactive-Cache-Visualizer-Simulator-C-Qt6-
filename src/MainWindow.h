#pragma once

#include "CacheEngine.h"
#include "TraceParser.h"

#include <QMainWindow>

class QButtonGroup;
class QColor;
class QComboBox;
class QFrame;
class QGroupBox;
class QLabel;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QResizeEvent;
class QSpinBox;
class QTextEdit;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void buildUi();
    void applyTheme();
    void refreshModeUi();
    void refreshSummary();
    void refreshStats();
    void refreshCacheTable();
    void refreshDashboard();
    void updateCacheTableSizing();
    void resetSimulation(bool keepTrace);
    bool captureConfig(CacheConfig &config, QString &errorText) const;
    bool ensureTraceLoaded(QString &errorText);
    void loadTraceText(const QString &text, bool resetCursor);
    void parseTraceFromEditor(bool resetCursor);
    void processNextLine();
    void processRange(int startIndex, int endIndex);
    void appendLog(const QString &line);
    void appendTimelineEntry(const QString &title, const QString &detail, const QColor &color);
    void updateAddressBreakdown(const CacheAccessResult &result, quint64 address);
    void updateMappingExplanation(const TraceLine &line, const CacheAccessResult &result);
    void updateModeButtons();
    QString locationText(const CacheAccessResult &result) const;
    void highlightCell(int row, int column, const QColor &color, const QString &tooltip);

private slots:
    void onModeChanged(int index);
    void onApplyConfiguration();
    void onLoadTrace();
    void onRunSimulation();
    void onStepSimulation();
    void onResetSimulation();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    CacheEngine m_engine;
    QVector<TraceLine> m_trace;
    QString m_lastTraceText;
    int m_cursor;

    QWidget *m_centralWidget;
    QButtonGroup *m_modeButtonGroup;
    QPushButton *m_directButton;
    QPushButton *m_fullyButton;
    QPushButton *m_kwayButton;
    QComboBox *m_modeCombo;
    QSpinBox *m_cacheSizeSpin;
    QSpinBox *m_blockSizeSpin;
    QSpinBox *m_assocSpin;
    QComboBox *m_policyCombo;
    QPushButton *m_applyButton;
    QPushButton *m_loadButton;
    QPushButton *m_runButton;
    QPushButton *m_stepButton;
    QPushButton *m_resetButton;
    QLabel *m_assocLabel;
    QLabel *m_summaryLabel;
    QLabel *m_statusLabel;
    QFrame *m_statsHitCard;
    QFrame *m_statsMissCard;
    QFrame *m_statsEvictCard;
    QFrame *m_statsAccessCard;
    QFrame *m_statsRateCard;
    QLabel *m_hitsValue;
    QLabel *m_missesValue;
    QLabel *m_evictionsValue;
    QLabel *m_accessesValue;
    QLabel *m_rateValue;
    QLabel *mModeHintLabel;
    QLabel *m_activeModeValue;
    QLabel *m_addressValue;
    QLabel *m_binaryValue;
    QLabel *m_blockValue;
    QLabel *m_setValue;
    QLabel *m_tagValue;
    QLabel *m_offsetValue;
    QTextEdit *m_explanationView;
    QListWidget *m_timelineList;
    QPlainTextEdit *m_traceEditor;
    QTextEdit *m_logView;
    QTableWidget *m_cacheTable;
};

