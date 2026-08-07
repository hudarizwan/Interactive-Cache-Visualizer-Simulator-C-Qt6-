#include "MainWindow.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QColor>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QStringList>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

#include <algorithm>
namespace
{
QString formatBinary(quint64 value, int width)
{
    QString bits = QString::number(value, 2);
    if (width > 0) {
        bits = bits.rightJustified(width, QLatin1Char('0'));
    }

    QString grouped;
    grouped.reserve(bits.size() + bits.size() / 4);
    for (int i = 0; i < bits.size(); ++i) {
        if (i > 0 && ((bits.size() - i) % 4 == 0)) {
            grouped.append(QLatin1Char(' '));
        }
        grouped.append(bits.at(i));
    }
    return grouped;
}

void repolishWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_cursor(0),
      m_centralWidget(nullptr),
      m_modeButtonGroup(nullptr),
      m_directButton(nullptr),
      m_fullyButton(nullptr),
      m_kwayButton(nullptr),
      m_modeCombo(nullptr),
      m_cacheSizeSpin(nullptr),
      m_blockSizeSpin(nullptr),
      m_assocSpin(nullptr),
      m_policyCombo(nullptr),
      m_applyButton(nullptr),
      m_loadButton(nullptr),
      m_runButton(nullptr),
      m_stepButton(nullptr),
      m_resetButton(nullptr),
      m_assocLabel(nullptr),
      m_summaryLabel(nullptr),
      m_statusLabel(nullptr),
      m_statsHitCard(nullptr),
      m_statsMissCard(nullptr),
      m_statsEvictCard(nullptr),
      m_statsAccessCard(nullptr),
      m_statsRateCard(nullptr),
      m_hitsValue(nullptr),
      m_missesValue(nullptr),
      m_evictionsValue(nullptr),
      m_accessesValue(nullptr),
      m_rateValue(nullptr),
      mModeHintLabel(nullptr),
      m_activeModeValue(nullptr),
      m_addressValue(nullptr),
      m_binaryValue(nullptr),
      m_blockValue(nullptr),
      m_setValue(nullptr),
      m_tagValue(nullptr),
      m_offsetValue(nullptr),
      m_explanationView(nullptr),
      m_timelineList(nullptr),
      m_traceEditor(nullptr),
      m_logView(nullptr),
      m_cacheTable(nullptr)
{
    buildUi();
    applyTheme();
    refreshModeUi();
    refreshSummary();
    refreshStats();
    statusBar()->showMessage(QStringLiteral("Ready"));
}

void MainWindow::buildUi()
{
    setWindowTitle(QStringLiteral("Cache Simulator Lab"));
    resize(1550, 980);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    auto *rootLayout = new QVBoxLayout(m_centralWidget);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(14);

    auto *headerFrame = new QFrame(m_centralWidget);
    headerFrame->setObjectName(QStringLiteral("HeaderFrame"));
    auto *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 18, 20, 18);
    headerLayout->setSpacing(12);

    auto *headerTop = new QHBoxLayout();
    headerTop->setSpacing(18);

    auto *brandBlock = new QHBoxLayout();
    brandBlock->setSpacing(14);
    auto *brandBadge = new QLabel(QStringLiteral("CS"), headerFrame);
    brandBadge->setObjectName(QStringLiteral("BrandBadge"));
    brandBadge->setAlignment(Qt::AlignCenter);
    auto *brandText = new QVBoxLayout();
    brandText->setSpacing(4);
    auto *title = new QLabel(QStringLiteral("Cache Simulator Lab"), headerFrame);
    title->setObjectName(QStringLiteral("HeaderTitle"));
    auto *subtitle = new QLabel(QStringLiteral("Interactive visualization of cache memory mapping techniques"), headerFrame);
    subtitle->setObjectName(QStringLiteral("HeaderSubtitle"));
    brandText->addWidget(title);
    brandText->addWidget(subtitle);
    brandBlock->addWidget(brandBadge);
    brandBlock->addLayout(brandText);
    brandBlock->addStretch();

    auto *modePanel = new QWidget(headerFrame);
    auto *modePanelLayout = new QVBoxLayout(modePanel);
    modePanelLayout->setContentsMargins(0, 0, 0, 0);
    modePanelLayout->setSpacing(8);
    auto *modeRow = new QHBoxLayout();
    modeRow->setSpacing(8);

    m_modeCombo = new QComboBox(headerFrame);
    m_modeCombo->addItem(QStringLiteral("Direct Mapped"));
    m_modeCombo->addItem(QStringLiteral("Fully Associative"));
    m_modeCombo->addItem(QStringLiteral("K-Way Set Associative"));
    m_modeCombo->hide();

    m_modeButtonGroup = new QButtonGroup(this);
    m_modeButtonGroup->setExclusive(true);

    m_directButton = new QPushButton(QStringLiteral("Direct"), headerFrame);
    m_directButton->setObjectName(QStringLiteral("ModeChip"));
    m_directButton->setCheckable(true);
    m_fullyButton = new QPushButton(QStringLiteral("Fully Associative"), headerFrame);
    m_fullyButton->setObjectName(QStringLiteral("ModeChip"));
    m_fullyButton->setCheckable(true);
    m_kwayButton = new QPushButton(QStringLiteral("K-Way"), headerFrame);
    m_kwayButton->setObjectName(QStringLiteral("ModeChip"));
    m_kwayButton->setCheckable(true);

    m_modeButtonGroup->addButton(m_directButton, 0);
    m_modeButtonGroup->addButton(m_fullyButton, 1);
    m_modeButtonGroup->addButton(m_kwayButton, 2);
    modeRow->addWidget(m_directButton);
    modeRow->addWidget(m_fullyButton);
    modeRow->addWidget(m_kwayButton);
    modePanelLayout->addLayout(modeRow);

    m_activeModeValue = new QLabel(QStringLiteral("Direct Mapped"), headerFrame);
    m_activeModeValue->setObjectName(QStringLiteral("ModeBadge"));
    m_activeModeValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    modePanelLayout->addWidget(m_activeModeValue);

    headerTop->addLayout(brandBlock, 1);
    headerTop->addWidget(modePanel, 0, Qt::AlignRight | Qt::AlignTop);
    headerLayout->addLayout(headerTop);

    rootLayout->addWidget(headerFrame);

    auto *contentSplitter = new QSplitter(Qt::Horizontal, m_centralWidget);
    contentSplitter->setChildrenCollapsible(false);

    auto *leftPanel = new QWidget(contentSplitter);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(14);

    auto *configCard = new QGroupBox(QStringLiteral("Configuration Control"), leftPanel);
    auto *configLayout = new QVBoxLayout(configCard);
    configLayout->setSpacing(12);

    auto *configForm = new QFormLayout();
    configForm->setLabelAlignment(Qt::AlignLeft);
    configForm->setFormAlignment(Qt::AlignTop);
    configForm->setHorizontalSpacing(14);
    configForm->setVerticalSpacing(10);

    m_cacheSizeSpin = new QSpinBox(configCard);
    m_cacheSizeSpin->setRange(4, 1048576);
    m_cacheSizeSpin->setSingleStep(4);
    m_cacheSizeSpin->setValue(64);
    m_cacheSizeSpin->setSuffix(QStringLiteral(" bytes"));

    m_blockSizeSpin = new QSpinBox(configCard);
    m_blockSizeSpin->setRange(1, 65536);
    m_blockSizeSpin->setSingleStep(1);
    m_blockSizeSpin->setValue(8);
    m_blockSizeSpin->setSuffix(QStringLiteral(" bytes"));

    m_assocSpin = new QSpinBox(configCard);
    m_assocSpin->setRange(1, 1024);
    m_assocSpin->setSingleStep(1);
    m_assocSpin->setValue(2);

    m_policyCombo = new QComboBox(configCard);
    m_policyCombo->addItem(QStringLiteral("LRU"));
    m_policyCombo->addItem(QStringLiteral("FIFO"));
    m_policyCombo->addItem(QStringLiteral("Random"));

    m_assocLabel = new QLabel(QStringLiteral("🧩 Ways per Set"), configCard);
    m_assocLabel->setToolTip(QStringLiteral("For k-way set associative caches, this is the number of ways in each set."));

    configForm->addRow(new QLabel(QStringLiteral("💾 Cache Size"), configCard), m_cacheSizeSpin);
    configForm->addRow(new QLabel(QStringLiteral("📦 Block Size"), configCard), m_blockSizeSpin);
    configForm->addRow(m_assocLabel, m_assocSpin);
    configForm->addRow(new QLabel(QStringLiteral("🔁 Replacement Policy"), configCard), m_policyCombo);
    configLayout->addLayout(configForm);

    mModeHintLabel = new QLabel(configCard);
    mModeHintLabel->setWordWrap(true);
    mModeHintLabel->setObjectName(QStringLiteral("HintLabel"));
    configLayout->addWidget(mModeHintLabel);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);
    m_applyButton = new QPushButton(QStringLiteral("Apply Configuration"), configCard);
    m_applyButton->setObjectName(QStringLiteral("PrimaryAction"));
    m_loadButton = new QPushButton(QStringLiteral("Load Trace..."), configCard);
    m_loadButton->setObjectName(QStringLiteral("LoadAction"));
    m_runButton = new QPushButton(QStringLiteral("Run Simulation"), configCard);
    m_runButton->setObjectName(QStringLiteral("RunAction"));
    m_stepButton = new QPushButton(QStringLiteral("Step"), configCard);
    m_stepButton->setObjectName(QStringLiteral("StepAction"));
    m_resetButton = new QPushButton(QStringLiteral("Reset"), configCard);
    m_resetButton->setObjectName(QStringLiteral("ResetAction"));
    for (QPushButton *button : {m_applyButton, m_loadButton, m_runButton, m_stepButton, m_resetButton}) {
        button->setMinimumHeight(42);
        buttonRow->addWidget(button);
    }
    configLayout->addLayout(buttonRow);

    leftLayout->addWidget(configCard);

    auto *traceCard = new QGroupBox(QStringLiteral("Trace Editor"), leftPanel);
    auto *traceLayout = new QVBoxLayout(traceCard);
    traceLayout->setSpacing(10);
    m_traceEditor = new QPlainTextEdit(traceCard);
    m_traceEditor->setPlaceholderText(QStringLiteral("1. R 0x1000\n2. W 0x1004\n3. R 0x2000"));
    m_traceEditor->setTabChangesFocus(false);
    traceLayout->addWidget(m_traceEditor);
    leftLayout->addWidget(traceCard, 1);

    auto *rightPanel = new QWidget(contentSplitter);
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(14);

    auto *statsCard = new QGroupBox(QStringLiteral("Statistics Dashboard"), rightPanel);
    auto *statsLayout = new QVBoxLayout(statsCard);
    statsLayout->setSpacing(12);

    auto createStatCard = [&](const QString &titleText, const QString &iconText, QLabel *&valueRef, QFrame *&cardRef, const QString &accentColor) {
        auto *card = new QFrame(statsCard);
        card->setMinimumHeight(104);
        card->setObjectName(QStringLiteral("StatCard"));
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 12, 14, 12);
        cardLayout->setSpacing(8);
        auto *accent = new QFrame(card);
        accent->setFixedHeight(4);
        accent->setStyleSheet(QStringLiteral("background: %1; border: none; border-radius: 2px;").arg(accentColor));
        auto *icon = new QLabel(iconText, card);
        icon->setObjectName(QStringLiteral("StatIcon"));
        auto *label = new QLabel(titleText, card);
        label->setObjectName(QStringLiteral("StatLabel"));
        valueRef = new QLabel(QStringLiteral("0"), card);
        valueRef->setMinimumHeight(34);
        valueRef->setObjectName(QStringLiteral("StatValue"));
        cardLayout->addWidget(accent);
        cardLayout->addWidget(icon);
        cardLayout->addWidget(label);
        cardLayout->addWidget(valueRef);
        cardLayout->addStretch();
        cardRef = card;
        return card;
    };

    auto *statsGrid = new QGridLayout();
    statsGrid->setHorizontalSpacing(12);
    statsGrid->setVerticalSpacing(12);
    statsGrid->addWidget(createStatCard(QStringLiteral("Hits"), QStringLiteral("🟢"), m_hitsValue, m_statsHitCard, QStringLiteral("#22c55e")), 0, 0);
    statsGrid->addWidget(createStatCard(QStringLiteral("Misses"), QStringLiteral("🔴"), m_missesValue, m_statsMissCard, QStringLiteral("#ef4444")), 0, 1);
    statsGrid->addWidget(createStatCard(QStringLiteral("Hit Rate"), QStringLiteral("📈"), m_rateValue, m_statsRateCard, QStringLiteral("#3b82f6")), 0, 2);
    statsGrid->addWidget(createStatCard(QStringLiteral("Evictions"), QStringLiteral("🗑️"), m_evictionsValue, m_statsEvictCard, QStringLiteral("#f59e0b")), 1, 0);
    statsGrid->addWidget(createStatCard(QStringLiteral("Accesses"), QStringLiteral("📊"), m_accessesValue, m_statsAccessCard, QStringLiteral("#7c3aed")), 1, 1);
    statsGrid->setColumnStretch(0, 1);
    statsGrid->setColumnStretch(1, 1);
    statsGrid->setColumnStretch(2, 1);
    statsLayout->addLayout(statsGrid);

    m_summaryLabel = new QLabel(statsCard);
    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setObjectName(QStringLiteral("SummaryLabel"));
    statsLayout->addWidget(m_summaryLabel);
    rightLayout->addWidget(statsCard);

    auto *visualCard = new QGroupBox(QStringLiteral("Cache Visualizer"), rightPanel);
    auto *visualLayout = new QVBoxLayout(visualCard);
    visualLayout->setSpacing(10);
    m_cacheTable = new QTableWidget(visualCard);
    m_cacheTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_cacheTable->setMinimumHeight(300);
    m_cacheTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cacheTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_cacheTable->setFocusPolicy(Qt::NoFocus);
    m_cacheTable->setWordWrap(true);
    m_cacheTable->setAlternatingRowColors(true);
    m_cacheTable->setShowGrid(false);
    m_cacheTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_cacheTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_cacheTable->horizontalHeader()->setStretchLastSection(false);
    m_cacheTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_cacheTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_cacheTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_cacheTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    visualLayout->addWidget(m_cacheTable, 1);

    auto *legendRow = new QHBoxLayout();
    legendRow->setSpacing(8);
    auto addLegend = [&](const QString &text, const QString &color) {
        auto *chip = new QLabel(text, visualCard);
        chip->setObjectName(QStringLiteral("LegendChip"));
        chip->setProperty("legendColor", color);
        legendRow->addWidget(chip);
    };
    addLegend(QStringLiteral("🟢 Valid"), QStringLiteral("#22c55e"));
    addLegend(QStringLiteral("⚪ Invalid"), QStringLiteral("#94a3b8"));
    addLegend(QStringLiteral("🟠 Current"), QStringLiteral("#f59e0b"));
    addLegend(QStringLiteral("🔴 Evicted"), QStringLiteral("#ef4444"));
    addLegend(QStringLiteral("🔵 Recent"), QStringLiteral("#3b82f6"));
    legendRow->addStretch();
    visualLayout->addLayout(legendRow);
    rightLayout->addWidget(visualCard, 3);

    auto *detailSplitter = new QSplitter(Qt::Horizontal, rightPanel);
    detailSplitter->setChildrenCollapsible(false);

    auto *detailLeft = new QWidget(detailSplitter);
    auto *detailLeftLayout = new QVBoxLayout(detailLeft);
    detailLeftLayout->setContentsMargins(0, 0, 0, 0);
    detailLeftLayout->setSpacing(12);

    auto *addressCard = new QGroupBox(QStringLiteral("Address Breakdown"), detailLeft);
    auto *addressLayout = new QGridLayout(addressCard);
    addressLayout->setHorizontalSpacing(12);
    addressLayout->setVerticalSpacing(10);
    addressLayout->setColumnStretch(1, 1);
    auto addField = [&](int row, const QString &labelText, QLabel *&valueRef) {
        auto *label = new QLabel(labelText, addressCard);
        label->setObjectName(QStringLiteral("DetailLabel"));
        valueRef = new QLabel(QStringLiteral("-"), addressCard);
        valueRef->setObjectName(QStringLiteral("DetailValue"));
        valueRef->setWordWrap(true);
        addressLayout->addWidget(label, row, 0);
        addressLayout->addWidget(valueRef, row, 1);
    };
    addField(0, QStringLiteral("Address"), m_addressValue);
    addField(1, QStringLiteral("Binary"), m_binaryValue);
    addField(2, QStringLiteral("Block"), m_blockValue);
    addField(3, QStringLiteral("Set"), m_setValue);
    addField(4, QStringLiteral("Tag"), m_tagValue);
    addField(5, QStringLiteral("Offset"), m_offsetValue);
    detailLeftLayout->addWidget(addressCard);

    auto *explainCard = new QGroupBox(QStringLiteral("Mapping Explanation"), detailLeft);
    auto *explainLayout = new QVBoxLayout(explainCard);
    m_explanationView = new QTextEdit(explainCard);
    m_explanationView->setReadOnly(true);
    m_explanationView->setAcceptRichText(true);
    m_explanationView->setObjectName(QStringLiteral("ExplanationView"));
    m_explanationView->setMinimumHeight(180);
    explainLayout->addWidget(m_explanationView);
    detailLeftLayout->addWidget(explainCard, 1);

    auto *timelineCard = new QGroupBox(QStringLiteral("Simulation Timeline"), detailSplitter);
    auto *timelineLayout = new QVBoxLayout(timelineCard);
    m_timelineList = new QListWidget(timelineCard);
    m_timelineList->setObjectName(QStringLiteral("TimelineList"));
    m_timelineList->setMinimumHeight(260);
    timelineLayout->addWidget(m_timelineList);

    detailSplitter->addWidget(detailLeft);
    detailSplitter->addWidget(timelineCard);
    detailSplitter->setStretchFactor(0, 2);
    detailSplitter->setStretchFactor(1, 1);
    detailSplitter->setSizes({700, 280});
    rightLayout->addWidget(detailSplitter, 2);

    auto *logCard = new QGroupBox(QStringLiteral("Execution Log"), rightPanel);
    auto *logLayout = new QVBoxLayout(logCard);
    m_logView = new QTextEdit(logCard);
    m_logView->setReadOnly(true);
    m_logView->setAcceptRichText(true);
    m_logView->setObjectName(QStringLiteral("LogView"));
    logLayout->addWidget(m_logView);
    rightLayout->addWidget(logCard, 1);

    contentSplitter->addWidget(leftPanel);
    contentSplitter->addWidget(rightPanel);
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 2);

    rootLayout->addWidget(contentSplitter, 1);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("StatusBadge"));
    statusBar()->addPermanentWidget(m_statusLabel, 1);

    connect(m_directButton, &QPushButton::clicked, this, [this]() { m_modeCombo->setCurrentIndex(0); });
    connect(m_fullyButton, &QPushButton::clicked, this, [this]() { m_modeCombo->setCurrentIndex(1); });
    connect(m_kwayButton, &QPushButton::clicked, this, [this]() { m_modeCombo->setCurrentIndex(2); });
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);
    connect(m_cacheSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) { refreshModeUi(); refreshSummary(); });
    connect(m_blockSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) { refreshModeUi(); refreshSummary(); });
    connect(m_policyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { refreshSummary(); });
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::onApplyConfiguration);
    connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::onLoadTrace);
    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::onRunSimulation);
    connect(m_stepButton, &QPushButton::clicked, this, &MainWindow::onStepSimulation);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::onResetSimulation);

    updateModeButtons();
    refreshModeUi();
    refreshSummary();
    refreshStats();
    refreshCacheTable();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateCacheTableSizing();
}

void MainWindow::refreshModeUi()
{
    const CacheMode mode = static_cast<CacheMode>(m_modeCombo->currentIndex());
    const int lines = std::max(1, m_cacheSizeSpin->value() / std::max(1, m_blockSizeSpin->value()));

    if (mode == CacheMode::DirectMapped) {
        m_assocLabel->setText(QStringLiteral("Ways per Set"));
        m_assocSpin->setValue(1);
        m_assocSpin->setEnabled(false);
        mModeHintLabel->setText(QStringLiteral("Direct mapped cache uses one line per set. Each block maps to exactly one line."));
    } else if (mode == CacheMode::FullyAssociative) {
        m_assocLabel->setText(QStringLiteral("Ways in the Single Set"));
        m_assocSpin->setValue(lines);
        m_assocSpin->setEnabled(false);
        mModeHintLabel->setText(QStringLiteral("Fully associative cache uses one set containing every line. The lookup searches every line in that single set."));
    } else {
        m_assocLabel->setText(QStringLiteral("K-Way / Ways per Set"));
        m_assocSpin->setEnabled(true);
        if (m_assocSpin->value() > lines) {
            m_assocSpin->setValue(std::max(1, lines));
        }
        mModeHintLabel->setText(QStringLiteral("Choose how many ways each set should have. Common values are 2, 4, or 8."));
    }

    refreshSummary();
}

void MainWindow::refreshSummary()
{
    CacheConfig config;
    QString errorText;
    if (!captureConfig(config, errorText)) {
        const int lines = std::max(1, m_cacheSizeSpin->value() / std::max(1, m_blockSizeSpin->value()));
        const int ways = m_modeCombo->currentIndex() == 0 ? 1 : (m_modeCombo->currentIndex() == 1 ? lines : m_assocSpin->value());
        const int sets = ways == 0 ? 0 : lines / ways;
        m_summaryLabel->setText(QStringLiteral("Mode: %1 | Cache: %2 bytes | Block: %3 bytes | Lines: %4 | Sets: %5 | Ways: %6 | Policy: %7")
                                    .arg(modeName(static_cast<CacheMode>(m_modeCombo->currentIndex())))
                                    .arg(m_cacheSizeSpin->value())
                                    .arg(m_blockSizeSpin->value())
                                    .arg(lines)
                                    .arg(sets)
                                    .arg(ways)
                                    .arg(policyName(static_cast<ReplacementPolicyType>(m_policyCombo->currentIndex()))));
        return;
    }

    m_summaryLabel->setText(QStringLiteral("Mode: %1 | Cache: %2 bytes | Block: %3 bytes | Lines: %4 | Sets: %5 | Ways: %6 | Policy: %7")
                                .arg(modeName(config.mode))
                                .arg(config.cacheSizeBytes)
                                .arg(config.blockSizeBytes)
                                .arg(config.totalLines())
                                .arg(config.setCount())
                                .arg(config.waysPerSet())
                                .arg(policyName(config.policy)));
}
void MainWindow::applyTheme()
{
    QApplication::setStyle(QStringLiteral("Fusion"));
    QFont font(QStringLiteral("Segoe UI"));
    font.setPointSize(10);
    qApp->setFont(font);

    setStyleSheet(QStringLiteral(
        "QMainWindow { background: #0b1220; }"
        "QWidget { color: #e2e8f0; background: transparent; }"
        "#HeaderFrame { background: #111827; border: 1px solid #2c3e50; border-radius: 16px; }"
        "#BrandBadge { background: #7c3aed; color: #ffffff; border-radius: 18px; min-width: 48px; min-height: 48px; max-width: 48px; max-height: 48px; font-size: 18px; font-weight: 700; }"
        "#HeaderTitle { color: #ffffff; font-size: 28px; font-weight: 700; }"
        "#HeaderSubtitle { color: #94a3b8; font-size: 11px; }"
        "#ModeChip { background: #1a2335; color: #cbd5e1; border: 1px solid #2c3e50; border-radius: 14px; padding: 8px 16px; font-weight: 600; }"
        "#ModeChip:hover { background: #24314a; border-color: #3b82f6; color: #ffffff; }"
        "#ModeChip:checked { background: #3b82f6; border-color: #7c3aed; color: #ffffff; }"
        "#ModeBadge { color: #ffffff; background: #1a2335; border: 1px solid #7c3aed; border-radius: 12px; padding: 8px 12px; font-weight: 600; }"
        "QGroupBox { color: #ffffff; border: 1px solid #2c3e50; border-radius: 16px; margin-top: 14px; background: #111827; font-weight: 600; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 8px; color: #cbd5e1; background: #111827; }"
        "QLabel { color: #e2e8f0; }"
        "#HintLabel, #SummaryLabel { color: #94a3b8; }"
        "#StatCard { background: #1a2335; border: 1px solid #2c3e50; border-radius: 16px; }"
        "#StatIcon { color: #94a3b8; font-size: 18px; font-weight: 600; }"
        "#StatLabel { color: #cbd5e1; font-size: 13px; font-weight: 600; }"
        "#StatValue { color: #ffffff; font-size: 24px; font-weight: 700; }"
        "#DetailLabel { color: #94a3b8; font-size: 13px; }"
        "#DetailValue { color: #ffffff; font-size: 13px; font-weight: 600; }"
        "#LegendChip { background: #1a2335; color: #e2e8f0; border: 1px solid #2c3e50; border-radius: 12px; padding: 5px 10px; }"
        "#StatusBadge { color: #e2e8f0; background: #111827; border: 1px solid #2c3e50; border-radius: 12px; padding: 6px 12px; }"
        "#PrimaryAction { background: #7c3aed; color: #ffffff; border: 1px solid #8b5cf6; border-radius: 12px; padding: 10px 14px; font-weight: 700; }"
        "#PrimaryAction:hover { background: #8b5cf6; }"
        "#LoadAction { background: #1a2335; color: #93c5fd; border: 1px solid #3b82f6; border-radius: 12px; padding: 10px 14px; font-weight: 700; }"
        "#LoadAction:hover { background: #24314a; }"
        "#RunAction { background: #166534; color: #ffffff; border: 1px solid #22c55e; border-radius: 12px; padding: 10px 14px; font-weight: 700; }"
        "#RunAction:hover { background: #1f7a42; }"
        "#StepAction { background: #b45309; color: #ffffff; border: 1px solid #f59e0b; border-radius: 12px; padding: 10px 14px; font-weight: 700; }"
        "#StepAction:hover { background: #d97706; }"
        "#ResetAction { background: #7f1d1d; color: #ffffff; border: 1px solid #ef4444; border-radius: 12px; padding: 10px 14px; font-weight: 700; }"
        "#ResetAction:hover { background: #991b1b; }"
        "QLineEdit, QSpinBox, QComboBox, QPlainTextEdit, QTextEdit, QTableWidget { background: #0b1220; color: #e2e8f0; border: 1px solid #2c3e50; border-radius: 12px; selection-background-color: #7c3aed; selection-color: #ffffff; }"
        "QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QPlainTextEdit:focus, QTextEdit:focus, QTableWidget:focus { border: 1px solid #7c3aed; }"
        "QPlainTextEdit, QTextEdit { padding: 10px; font-family: Consolas, monospace; }"
        "QAbstractSpinBox { padding-right: 24px; }"
        "QSpinBox::up-button, QSpinBox::down-button { width: 18px; background: #111827; border-left: 1px solid #2c3e50; }"
        "QComboBox { padding: 8px 12px; }"
        "QComboBox::drop-down { width: 24px; background: #111827; border-left: 1px solid #2c3e50; }"
        "QComboBox QAbstractItemView { background: #111827; selection-background-color: #24314a; color: #e2e8f0; outline: 0; }"
        "QHeaderView::section { background: #111827; color: #e2e8f0; border: none; border-right: 1px solid #2c3e50; border-bottom: 1px solid #2c3e50; padding: 8px; font-weight: 600; }"
        "QTableWidget { gridline-color: #2c3e50; alternate-background-color: #0f172a; }"
        "QTableWidget::item { padding: 8px; }"
        "QTableCornerButton::section { background: #111827; border: none; }"
        "#CacheCell { background: #111827; border: 1px solid #2c3e50; border-radius: 14px; }"
        "#CacheCell[state='valid'] { background: #122035; }"
        "#CacheCell[state='invalid'] { background: #0b1220; }"
        "#CacheCell[state='current'] { background: #16263f; border: 1px solid #f59e0b; }"
        "#CacheCell[state='evicted'] { background: #2b1720; border: 1px solid #ef4444; }"
        "#CellTitle { color: #ffffff; font-size: 12px; font-weight: 700; }"
        "#CellStatus { color: #cbd5e1; font-size: 11px; font-weight: 600; }"
        "#CellMeta { color: #94a3b8; font-size: 11px; }"
        "#CellWords { color: #e2e8f0; font-size: 11px; }"
        "#ExplanationView, #LogView, #TimelineList { background: #0b1220; }"
        "#TimelineList::item { background: #1a2335; border: 1px solid #2c3e50; border-radius: 12px; padding: 8px 10px; margin: 4px 2px; }"
        "#TimelineList::item:selected { background: #24314a; }"
        "QScrollBar:vertical { background: #0b1220; width: 10px; margin: 2px; }"
        "QScrollBar::handle:vertical { background: #334155; min-height: 24px; border-radius: 5px; }"
        "QScrollBar:horizontal { background: #0b1220; height: 10px; margin: 2px; }"
        "QScrollBar::handle:horizontal { background: #334155; min-width: 24px; border-radius: 5px; }"
        "QScrollBar::add-line, QScrollBar::sub-line { background: none; border: none; }"
        "QStatusBar { color: #cbd5e1; background: #111827; border-top: 1px solid #2c3e50; }"
        "QToolTip { color: #ffffff; background: #111827; border: 1px solid #3b82f6; padding: 6px; border-radius: 8px; }"
    ));
}

void MainWindow::refreshStats()
{
    const CacheStats &stats = m_engine.stats();
    m_hitsValue->setText(QString::number(stats.hits));
    m_missesValue->setText(QString::number(stats.misses));
    m_evictionsValue->setText(QString::number(stats.evictions));
    m_accessesValue->setText(QString::number(stats.accesses));
    m_rateValue->setText(QStringLiteral("%1%").arg(stats.hitRate() * 100.0, 0, 'f', 2));
    m_activeModeValue->setText(modeName(static_cast<CacheMode>(m_modeCombo->currentIndex())));

    if (m_engine.configured()) {
        m_statusLabel->setText(QStringLiteral("Mode: %1 | Accesses: %2 | Hits: %3 | Misses: %4 | Evictions: %5")
                                   .arg(modeName(m_engine.config().mode))
                                   .arg(stats.accesses)
                                   .arg(stats.hits)
                                   .arg(stats.misses)
                                   .arg(stats.evictions));
    } else {
        m_statusLabel->setText(QStringLiteral("Ready"));
    }
}

void MainWindow::refreshDashboard()
{
    updateModeButtons();
    refreshModeUi();
    refreshSummary();
    refreshStats();
    refreshCacheTable();
}

void MainWindow::refreshCacheTable()
{
    if (!m_engine.configured()) {
        m_cacheTable->clear();
        m_cacheTable->setRowCount(0);
        m_cacheTable->setColumnCount(0);
        return;
    }

    const CacheConfig &config = m_engine.config();
    const int sets = config.setCount();
    const int ways = config.waysPerSet();

    m_cacheTable->clear();
    m_cacheTable->setRowCount(sets);
    m_cacheTable->setColumnCount(ways);

    QStringList horizontalHeaders;
    QStringList verticalHeaders;

    if (config.mode == CacheMode::FullyAssociative) {
        for (int way = 0; way < ways; ++way) {
            horizontalHeaders << QStringLiteral("Line %1").arg(way);
        }
        verticalHeaders << QStringLiteral("All Lines");
    } else if (config.mode == CacheMode::DirectMapped) {
        horizontalHeaders << QStringLiteral("Line");
        for (int set = 0; set < sets; ++set) {
            verticalHeaders << QStringLiteral("Line %1").arg(set);
        }
    } else {
        for (int way = 0; way < ways; ++way) {
            horizontalHeaders << QStringLiteral("Way %1").arg(way);
        }
        for (int set = 0; set < sets; ++set) {
            verticalHeaders << QStringLiteral("Set %1").arg(set);
        }
    }

    m_cacheTable->setHorizontalHeaderLabels(horizontalHeaders);
    m_cacheTable->setVerticalHeaderLabels(verticalHeaders);

    for (int row = 0; row < sets; ++row) {
        for (int column = 0; column < ways; ++column) {
            const CacheLine &line = m_engine.sets().at(row).lineAt(column);
            auto *cell = new QFrame(m_cacheTable);
            cell->setObjectName(QStringLiteral("CacheCell"));
            cell->setProperty("state", line.valid() ? QStringLiteral("valid") : QStringLiteral("invalid"));

            auto *cellLayout = new QVBoxLayout(cell);
            cellLayout->setContentsMargins(8, 8, 8, 8);
            cellLayout->setSpacing(5);

            const QString cellTitle = config.mode == CacheMode::DirectMapped
                                          ? QStringLiteral("Line %1").arg(row)
                                          : (config.mode == CacheMode::FullyAssociative ? QStringLiteral("Line %1").arg(column) : QStringLiteral("Way %1").arg(column));

            auto *title = new QLabel(cellTitle, cell);
            title->setWordWrap(true);
            title->setObjectName(QStringLiteral("CellTitle"));
            title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            auto *status = new QLabel(line.valid() ? QStringLiteral("Valid  ✓") : QStringLiteral("Invalid"), cell);
            status->setObjectName(QStringLiteral("CellStatus"));

            auto *meta = new QLabel(cell);
            meta->setObjectName(QStringLiteral("CellMeta"));
            meta->setWordWrap(true);
            if (line.valid()) {
                meta->setText(QStringLiteral("Tag %1  |  Block %2\nBase %3  |  Dirty %4")
                                   .arg(hexString(line.tag()))
                                   .arg(line.blockNumber())
                                   .arg(hexString(line.blockBaseAddress()))
                                   .arg(line.dirty() ? QStringLiteral("yes") : QStringLiteral("no")));
            } else {
                meta->setText(QStringLiteral("Empty line\nWaiting for a block"));
            }

            auto *words = new QLabel(cell);
            words->setObjectName(QStringLiteral("CellWords"));
            words->setWordWrap(true);
            QStringList preview;
            const QVector<quint64> &blockWords = line.words();
            const int previewCount = std::min(4, static_cast<int>(blockWords.size()));
            for (int i = 0; i < previewCount; ++i) {
                preview << hexString(blockWords.at(i));
            }
            if (blockWords.size() > previewCount) {
                preview << QStringLiteral("...");
            }
            words->setText(line.valid() ? preview.join(QStringLiteral("  ")) : QStringLiteral("No block loaded"));

            cellLayout->addWidget(title);
            cellLayout->addWidget(status);
            cellLayout->addWidget(meta);
            cellLayout->addWidget(words);
            cellLayout->addStretch();

            cell->setToolTip(QStringLiteral("Set: %1\nWay: %2\nTag: %3\nBlock: %4\nBase: %5\nLoaded at: %6\nLast used: %7")
                                 .arg(row)
                                 .arg(column)
                                 .arg(line.valid() ? hexString(line.tag()) : QStringLiteral("-"))
                                 .arg(line.valid() ? QString::number(line.blockNumber()) : QStringLiteral("-"))
                                 .arg(line.valid() ? hexString(line.blockBaseAddress()) : QStringLiteral("-"))
                                 .arg(line.loadedAt())
                                 .arg(line.lastUsedAt()));
            repolishWidget(cell);
            m_cacheTable->setCellWidget(row, column, cell);
        }
    }

    updateCacheTableSizing();
}

void MainWindow::updateCacheTableSizing()
{
    if (!m_cacheTable || !m_engine.configured()) {
        return;
    }

    const int rows = m_cacheTable->rowCount();
    const int cols = m_cacheTable->columnCount();
    if (rows <= 0 || cols <= 0) {
        return;
    }

    const int minColumnWidth = 96;
    const int minRowHeight = 90;
    const int widthBudget = std::max(0, m_cacheTable->viewport()->width());
    const int heightBudget = std::max(0, m_cacheTable->viewport()->height());

    const int columnWidth = std::max(minColumnWidth, widthBudget / cols);
    const int rowHeight = std::max(minRowHeight, heightBudget / rows);

    for (int column = 0; column < cols; ++column) {
        m_cacheTable->setColumnWidth(column, columnWidth);
    }
    for (int row = 0; row < rows; ++row) {
        m_cacheTable->setRowHeight(row, rowHeight);
    }
}

void MainWindow::resetSimulation(bool keepTrace)
{
    if (m_engine.configured()) {
        m_engine.reset();
    }
    m_cursor = 0;
    if (!keepTrace) {
        m_trace.clear();
        m_lastTraceText.clear();
        m_traceEditor->clear();
    }
    m_logView->clear();
    m_timelineList->clear();
    m_explanationView->clear();
    m_addressValue->setText(QStringLiteral("-"));
    m_binaryValue->setText(QStringLiteral("-"));
    m_blockValue->setText(QStringLiteral("-"));
    m_setValue->setText(QStringLiteral("-"));
    m_tagValue->setText(QStringLiteral("-"));
    m_offsetValue->setText(QStringLiteral("-"));
    refreshStats();
    refreshCacheTable();
}

bool MainWindow::captureConfig(CacheConfig &config, QString &errorText) const
{
    config.mode = static_cast<CacheMode>(m_modeCombo->currentIndex());
    config.policy = static_cast<ReplacementPolicyType>(m_policyCombo->currentIndex());
    config.cacheSizeBytes = static_cast<quint64>(m_cacheSizeSpin->value());
    config.blockSizeBytes = static_cast<quint64>(m_blockSizeSpin->value());
    config.associativity = m_assocSpin->value();

    if (config.mode == CacheMode::DirectMapped) {
        config.associativity = 1;
    } else if (config.mode == CacheMode::FullyAssociative) {
        const quint64 lines = config.blockSizeBytes == 0 ? 0 : config.cacheSizeBytes / config.blockSizeBytes;
        config.associativity = static_cast<int>(lines);
    }

    return config.isValid(&errorText);
}

bool MainWindow::ensureTraceLoaded(QString &errorText)
{
    const QString text = m_traceEditor->toPlainText();
    if (text.trimmed().isEmpty()) {
        errorText = QStringLiteral("Error: Please load a trace file first.");
        return false;
    }

    if (text != m_lastTraceText) {
        parseTraceFromEditor(true);
    } else if (m_trace.isEmpty()) {
        parseTraceFromEditor(false);
    }

    if (m_trace.isEmpty()) {
        errorText = QStringLiteral("Error: Please load a trace file first.");
        return false;
    }

    return true;
}

void MainWindow::loadTraceText(const QString &text, bool resetCursor)
{
    m_traceEditor->setPlainText(text);
    const QString normalized = m_traceEditor->toPlainText();
    m_lastTraceText = normalized;
    m_trace = TraceParser::parse(normalized);
    if (resetCursor) {
        m_cursor = 0;
    }
    if (m_engine.configured()) {
        m_engine.reset();
    }
    m_logView->clear();
    m_timelineList->clear();
    m_explanationView->clear();
    m_addressValue->setText(QStringLiteral("-"));
    m_binaryValue->setText(QStringLiteral("-"));
    m_blockValue->setText(QStringLiteral("-"));
    m_setValue->setText(QStringLiteral("-"));
    m_tagValue->setText(QStringLiteral("-"));
    m_offsetValue->setText(QStringLiteral("-"));
    refreshStats();
    refreshCacheTable();
}

void MainWindow::parseTraceFromEditor(bool resetCursor)
{
    const QString text = m_traceEditor->toPlainText();
    if (text == m_lastTraceText && !m_trace.isEmpty()) {
        if (resetCursor) {
            m_cursor = 0;
        }
        return;
    }

    m_lastTraceText = text;
    m_trace = TraceParser::parse(text);
    if (resetCursor) {
        m_cursor = 0;
    }
    if (m_engine.configured()) {
        m_engine.reset();
    }
    m_logView->clear();
    m_timelineList->clear();
    m_explanationView->clear();
    m_addressValue->setText(QStringLiteral("-"));
    m_binaryValue->setText(QStringLiteral("-"));
    m_blockValue->setText(QStringLiteral("-"));
    m_setValue->setText(QStringLiteral("-"));
    m_tagValue->setText(QStringLiteral("-"));
    m_offsetValue->setText(QStringLiteral("-"));
    refreshStats();
    refreshCacheTable();
}

QString MainWindow::locationText(const CacheAccessResult &result) const
{
    if (!m_engine.configured()) {
        return QString();
    }

    const CacheConfig &config = m_engine.config();
    if (config.mode == CacheMode::DirectMapped) {
        return QStringLiteral("Line %1").arg(result.setIndex);
    }

    if (config.mode == CacheMode::FullyAssociative) {
        return QStringLiteral("Line %1").arg(result.wayIndex);
    }

    return QStringLiteral("Set %1, Way %2").arg(result.setIndex).arg(result.wayIndex);
}

void MainWindow::highlightCell(int row, int column, const QColor &color, const QString &tooltip)
{
    QWidget *cell = m_cacheTable->cellWidget(row, column);
    if (!cell) {
        return;
    }
    cell->setToolTip(tooltip);
    cell->setStyleSheet(QStringLiteral(
        "QFrame#CacheCell { background: #16263f; border: 1px solid %1; border-radius: 14px; }"
        "QLabel#CellTitle { color: #ffffff; }"
        "QLabel#CellStatus { color: #ffffff; }"
        "QLabel#CellMeta { color: #dbeafe; }"
        "QLabel#CellWords { color: #ffffff; }").arg(color.name()));
    repolishWidget(cell);
}

void MainWindow::processNextLine()
{
    if (m_cursor < 0 || m_cursor >= m_trace.size()) {
        appendLog(QStringLiteral("<span style='color:#22c55e; font-weight:600;'>Simulation complete.</span>"));
        return;
    }

    const TraceLine line = m_trace.at(m_cursor);
    ++m_cursor;
    const int accessNumber = m_cursor;

    if (!line.valid) {
        appendLog(QStringLiteral("<span style='color:#f59e0b; font-weight:600;'>Request %1 skipped:</span> %2")
                      .arg(accessNumber)
                      .arg(line.errorText.toHtmlEscaped()));
        appendTimelineEntry(QStringLiteral("Access %1").arg(accessNumber), QStringLiteral("Skipped invalid request"), QColor(QStringLiteral("#94a3b8")));
        return;
    }

    const CacheAccessResult result = m_engine.access(line.type, line.address);
    updateAddressBreakdown(result, line.address);
    updateMappingExplanation(line, result);

    const QString actionColor = line.type == AccessType::Read ? QStringLiteral("#3b82f6") : QStringLiteral("#a855f7");
    const QString outcomeColor = result.hit ? QStringLiteral("#22c55e") : QStringLiteral("#ef4444");
    const QString reason = result.hit
                               ? QStringLiteral("matched an existing tag")
                               : (result.eviction ? QStringLiteral("the set was full, so the policy evicted a victim") : QStringLiteral("an empty way accepted the block"));
    const QString location = locationText(result);
    const QString requestLabel = line.type == AccessType::Read ? QStringLiteral("READ") : QStringLiteral("WRITE");
    const QString victimText = result.eviction ? QStringLiteral(" | Victim Way %1").arg(result.victimWay) : QString();

    appendLog(QStringLiteral(
        "<div style='margin-bottom:4px;'>"
        "<span style='color:%1; font-weight:700;'>%2</span> "
        "<span style='color:#22d3ee;'>%3</span> "
        "<span style='color:#94a3b8;'>Request %4</span>"
        "</div>"
        "<div style='margin-bottom:6px; color:#cbd5e1;'>Block <b>%5</b> -> %6, Tag <b>%7</b>, Set <b>%8</b>, Offset <b>%9</b>%10</div>"
        "<div style='color:%11; font-weight:700;'>%12</div>"
        "<div style='color:#94a3b8;'>%13</div>")
                      .arg(actionColor)
                      .arg(requestLabel)
                      .arg(hexString(line.address))
                      .arg(accessNumber)
                      .arg(result.blockNumber)
                      .arg(location)
                      .arg(hexString(result.tag))
                      .arg(result.setIndex)
                      .arg(hexString(result.offset))
                      .arg(victimText.toHtmlEscaped())
                      .arg(outcomeColor)
                      .arg(result.hit ? QStringLiteral("CACHE HIT") : QStringLiteral("CACHE MISS"))
                      .arg(reason.toHtmlEscaped()));

    refreshStats();
    refreshCacheTable();

    if (result.valid) {
        if (result.setIndex >= 0 && result.wayIndex >= 0) {
            const QColor color = result.hit ? QColor(QStringLiteral("#22c55e")) : (result.eviction ? QColor(QStringLiteral("#ef4444")) : QColor(QStringLiteral("#f59e0b")));
            const QString tooltip = result.hit ? QStringLiteral("Hit: the cache line already contained the requested block")
                                               : (result.eviction ? QStringLiteral("Miss with eviction: a victim line was replaced") : QStringLiteral("Miss: the block was loaded into a free way"));
            highlightCell(result.setIndex, result.wayIndex, color, tooltip);
        }
    }

    appendTimelineEntry(QStringLiteral("Access %1").arg(accessNumber),
                        QStringLiteral("%1 %2 -> %3")
                            .arg(line.type == AccessType::Read ? QStringLiteral("READ") : QStringLiteral("WRITE"))
                            .arg(hexString(line.address))
                            .arg(result.hit ? QStringLiteral("HIT") : (result.eviction ? QStringLiteral("MISS + EVICT") : QStringLiteral("MISS"))),
                        result.hit ? QColor(QStringLiteral("#166534")) : (result.eviction ? QColor(QStringLiteral("#b45309")) : QColor(QStringLiteral("#7f1d1d"))));
}

void MainWindow::processRange(int startIndex, int endIndex)
{
    if (m_trace.isEmpty()) {
        return;
    }

    const int upper = std::min(endIndex, static_cast<int>(m_trace.size()));
    m_cursor = std::max(0, startIndex);
    for (; m_cursor < upper; ) {
        processNextLine();
    }
}

void MainWindow::appendLog(const QString &line)
{
    if (line.trimmed().startsWith(QLatin1Char('<'))) {
        m_logView->append(line);
    } else if (line.startsWith(QStringLiteral("Error:"))) {
        m_logView->append(QStringLiteral("<span style='color:#ef4444; font-weight:600;'>%1</span>").arg(line.toHtmlEscaped()));
    } else if (line.contains(QStringLiteral("complete"), Qt::CaseInsensitive)) {
        m_logView->append(QStringLiteral("<span style='color:#22c55e; font-weight:600;'>%1</span>").arg(line.toHtmlEscaped()));
    } else {
        m_logView->append(QStringLiteral("<span style='color:#cbd5e1;'>%1</span>").arg(line.toHtmlEscaped()));
    }
    m_logView->verticalScrollBar()->setValue(m_logView->verticalScrollBar()->maximum());
}

void MainWindow::appendTimelineEntry(const QString &title, const QString &detail, const QColor &color)
{
    auto *item = new QListWidgetItem(QStringLiteral("%1\n%2").arg(title, detail));
    item->setForeground(Qt::white);
    item->setBackground(color.darker(170));
    item->setToolTip(detail);
    item->setSizeHint(QSize(0, 54));
    m_timelineList->addItem(item);
    m_timelineList->scrollToBottom();
}

void MainWindow::updateAddressBreakdown(const CacheAccessResult &result, quint64 address)
{
    if (!m_engine.configured() || !result.valid) {
        m_addressValue->setText(QStringLiteral("-"));
        m_binaryValue->setText(QStringLiteral("-"));
        m_blockValue->setText(QStringLiteral("-"));
        m_setValue->setText(QStringLiteral("-"));
        m_tagValue->setText(QStringLiteral("-"));
        m_offsetValue->setText(QStringLiteral("-"));
        return;
    }

    const int width = std::max(16, integerLog2(address) + 1);
    m_addressValue->setText(hexString(address));
    m_binaryValue->setText(formatBinary(address, width));
    m_blockValue->setText(QStringLiteral("%1 (%2)").arg(result.blockNumber).arg(hexString(result.blockBaseAddress)));
    m_setValue->setText(QString::number(result.setIndex));
    m_tagValue->setText(hexString(result.tag));
    m_offsetValue->setText(hexString(result.offset));
}

void MainWindow::updateMappingExplanation(const TraceLine &line, const CacheAccessResult &result)
{
    if (!m_engine.configured()) {
        m_explanationView->setHtml(QStringLiteral(
            "<div style='color:#94a3b8;'>Configure the cache and load a trace to see the mapping explanation here.</div>"));
        return;
    }

    const CacheConfig &config = m_engine.config();
    if (!line.valid || !result.valid) {
        m_explanationView->setHtml(QStringLiteral(
            "<div style='color:#94a3b8;'>Run or step through a request to see the mapping explanation here.</div>"));
        return;
    }

    const QString mode = modeName(config.mode);
    const QString accessWord = line.type == AccessType::Read ? QStringLiteral("READ") : QStringLiteral("WRITE");
    const QString outcomeColor = result.hit ? QStringLiteral("#22c55e") : QStringLiteral("#ef4444");
    const QString outcomeText = result.hit ? QStringLiteral("CACHE HIT") : QStringLiteral("CACHE MISS");
    const QString scope = config.mode == CacheMode::DirectMapped
                              ? QStringLiteral("the single mapped line")
                              : (config.mode == CacheMode::FullyAssociative ? QStringLiteral("every line in the single set") : QStringLiteral("the %1 ways in Set %2").arg(config.waysPerSet()).arg(result.setIndex));
    const QString comparison = result.hit ? QStringLiteral("The tag matched an existing cache line.") : QStringLiteral("No matching tag was found.");
    const QString action = result.hit ? QStringLiteral("The existing line was reused.") : (result.eviction ? QStringLiteral("A victim line was replaced because the set was full.") : QStringLiteral("The block was loaded into an empty way."));

    m_explanationView->setHtml(QStringLiteral(
        "<div style='line-height:1.6;'>"
        "<div><span style='color:#93c5fd; font-weight:700;'>%1</span> address <b>%2</b> maps to block <b>%3</b>.</div>"
        "<div>In <b>%4</b>, the cache searched <b>%5</b>.</div>"
        "<div>The selected location was <b>%6</b>.</div>"
        "<div style='color:#cbd5e1;'>%7</div>"
        "<div style='margin-top:8px; color:%8; font-weight:700;'>%9</div>"
        "<div style='margin-top:4px; color:#e2e8f0;'>%10</div>"
        "</div>")
                               .arg(accessWord)
                               .arg(hexString(line.address))
                               .arg(result.blockNumber)
                               .arg(mode)
                               .arg(scope.toHtmlEscaped())
                               .arg(locationText(result).toHtmlEscaped())
                               .arg(comparison.toHtmlEscaped())
                               .arg(outcomeColor)
                               .arg(outcomeText)
                               .arg(action.toHtmlEscaped()));
}

void MainWindow::updateModeButtons()
{
    QSignalBlocker directBlock(m_directButton);
    QSignalBlocker fullyBlock(m_fullyButton);
    QSignalBlocker kwayBlock(m_kwayButton);

    const int mode = m_modeCombo->currentIndex();
    m_directButton->setChecked(mode == 0);
    m_fullyButton->setChecked(mode == 1);
    m_kwayButton->setChecked(mode == 2);
}

void MainWindow::onModeChanged(int)
{
    refreshModeUi();
    refreshSummary();
    updateModeButtons();
    refreshStats();
}

void MainWindow::onApplyConfiguration()
{
    CacheConfig config;
    QString errorText;
    if (!captureConfig(config, errorText)) {
        QMessageBox::warning(this, QStringLiteral("Configuration Error"), errorText);
        return;
    }

    if (!m_engine.configure(config, &errorText)) {
        QMessageBox::warning(this, QStringLiteral("Configuration Error"), errorText);
        return;
    }

    m_cursor = 0;
    m_logView->clear();
    m_timelineList->clear();
    m_explanationView->clear();
    refreshDashboard();
    statusBar()->showMessage(QStringLiteral("Cache configured"));
}

void MainWindow::onLoadTrace()
{
    const QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Open Trace File"), QString(), QStringLiteral("Trace Files (*.txt *.trace *.log);;All Files (*.*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("Open File"), QStringLiteral("Unable to open the selected trace file."));
        return;
    }

    const QString text = QString::fromUtf8(file.readAll());
    loadTraceText(text, true);
    statusBar()->showMessage(QStringLiteral("Trace loaded: %1").arg(fileName), 4000);
}

void MainWindow::onRunSimulation()
{
    QString errorText;
    if (!m_engine.configured()) {
        errorText = QStringLiteral("Error: Please configure cache parameters first.");
        QMessageBox::warning(this, QStringLiteral("Run Simulation"), errorText);
        appendLog(errorText);
        return;
    }

    if (!ensureTraceLoaded(errorText)) {
        QMessageBox::warning(this, QStringLiteral("Run Simulation"), errorText);
        appendLog(errorText);
        return;
    }

    processRange(m_cursor, m_trace.size());
    if (m_cursor >= m_trace.size()) {
        appendLog(QStringLiteral("Simulation complete. Final cache state is shown above."));
    }
}

void MainWindow::onStepSimulation()
{
    QString errorText;
    if (!m_engine.configured()) {
        errorText = QStringLiteral("Error: Please configure cache parameters first.");
        QMessageBox::warning(this, QStringLiteral("Step"), errorText);
        appendLog(errorText);
        return;
    }

    if (!ensureTraceLoaded(errorText)) {
        QMessageBox::warning(this, QStringLiteral("Step"), errorText);
        appendLog(errorText);
        return;
    }

    processNextLine();
}

void MainWindow::onResetSimulation()
{
    resetSimulation(true);
    appendLog(QStringLiteral("Cache and counters reset. Load a trace and run again."));
    refreshDashboard();
    statusBar()->showMessage(QStringLiteral("Reset complete"), 3000);
}




