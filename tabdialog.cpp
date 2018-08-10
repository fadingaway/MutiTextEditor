#include "QTabWidget"
#include "QVBoxLayout"
#include "QPushButton"
#include "QDebug"
#include "QRect"
#include "QHBoxLayout"
#include "QLabel"
#include "QLineEdit"
#include "QCheckBox"
#include "QGroupBox"
#include "QSlider"
#include "QPixmap"
#include "QComboBox"
#include "QRadioButton"
#include "tabdialog.h"
#include "QDebug"
#include "QThread"
#include "QApplication"
#include "QButtonGroup"

TabDialog::TabDialog(QString searchString, int tabIndex, QWidget *parent):QDialog(parent)
{
    qDebug()<<"TabDialog::TabDialog";
    tabWidget = new QTabWidget;
    findtab = new FindTab(searchString);
    replaceTab = new ReplaceTab(searchString);
    documentTab = new DocumentSearchTab(searchString);
    tabWidget->addTab(findtab, tr("Find"));
    tabWidget->addTab(replaceTab, tr("Replace"));
    tabWidget->addTab(documentTab, tr("File Search"));

    connect(tabWidget,SIGNAL(currentChanged(int)),this, SLOT(updateWindowTitle(int)));

    connect(findtab, SIGNAL(notifyTabWidget(int)), this,SLOT(setTabAlpha(int)));
    connect(findtab, SIGNAL(notifyTabWidget(int)), this,SLOT(setTabAlpha(int)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->setMargin(1);
    setLayout(mainLayout);
    updateWindowTitle(tabIndex);

}

void TabDialog::updateWindowTitle(int tab_Id = 0)
{
    qDebug()<<"TabDialog::updateWindowTitle";
    if(tab_Id == 0)
    {
        setWindowTitle("Find");
    }
    else if(tab_Id == 1)
    {
        setWindowTitle("Search");
    }
    else if(tab_Id == 2)
    {
        setWindowTitle("File Research");
    }
}

void TabDialog::setTabAlpha(int value)
{
    qDebug()<<"TabDialog::setTabAlpha";
    double alpha = (double)value/100;
    if(alpha < 0.1)
    {
        alpha = 0.1;
    }
    currentAlpa = alpha;
    this->setWindowOpacity(1-currentAlpa);
}

void FindTab::valueChange(int value)
{
    qDebug()<<"FindTab::valueChange";
    emit notifyTabWidget(value);
}

void FindTab::ClearMarks()
{
    qDebug()<<"FindTab::ClearMarks";
    emit ClearMarkClicked();
}

void FindTab::SearchAll()
{
    qDebug()<<"FindTab::SearchAll";
    emit notifySearchAllClicked(getSearchString(),
                                checkBoxMarkLine->isChecked(),
                                checkBoxHighlightSearchResult->isChecked(),
                                checkBoxClearLastMark->isChecked());
}

void FindTab::SearchNext()
{
    qDebug()<<"FindTab::SearchNext";
    emit notifySearchNextClicked(getSearchString(),
                                 checkBoxMatchWholeWord->isChecked(),
                                 checkBoxMatchUpperLower->isChecked(),
                                 checkBoxSearchLoop->isChecked(),
                                 buttonUp->isChecked());
}

QString FindTab::getSearchString()
{
    qDebug()<<"FindTab::getSearchString";
    qDebug()<<comboBox->currentText();
    return comboBox->currentText();
}

void FindTab::CountClicked()
{
    qDebug()<<"FindTab::CountClicked";
    if(checkBoxMatchWholeWord->isChecked()&& checkBoxMatchUpperLower->isChecked())
    {
        emit notifyCountClicked(true, true);
    }
    else if(checkBoxMatchUpperLower->isChecked())
    {
        emit notifyCountClicked(false, true);
    }
    else
    {
        emit notifyCountClicked(true, false);
    }
}

void FindTab::SearchAllFile()
{
    qDebug()<<"FindTab::SearchAllFile";
    if(checkBoxMatchWholeWord->isChecked()&& checkBoxMatchUpperLower->isChecked())
    {
        emit notifySearchAllOpenedFileClicked(true, true);
    }
    else if(checkBoxMatchUpperLower->isChecked())
    {
        emit notifySearchAllOpenedFileClicked(false, true);
    }
    else
    {
        emit notifySearchAllOpenedFileClicked(true, false);
    }
}

void FindTab::SearchCurrentFile()
{
    qDebug()<<"FindTab::SearchCurrentFile";
    if(checkBoxMatchWholeWord->isChecked()&& checkBoxMatchUpperLower->isChecked())
    {
        emit notifySearchCurrentOpenedFileClicked(true, true);
    }
    else if(checkBoxMatchUpperLower->isChecked())
    {
        emit notifySearchCurrentOpenedFileClicked(false, true);
    }
    else
    {
        emit notifySearchCurrentOpenedFileClicked(true, false);
    }
}

void FindTab::cancel()
{
    qDebug()<<"FindTab::cancel";
    emit notifyCancelClicked();
}

void TabDialog::focusInEvent(QFocusEvent *e)
{
    qDebug()<<"TabDialog::focusInEvent";
    this->setWindowOpacity(1 - (double)currentAlpa/100);
    QDialog::focusInEvent(e);
}

void TabDialog::focusOutEvent(QFocusEvent *e)
{
    qDebug()<<"TabDialog::focusOutEvent";
    this->setWindowOpacity((double)currentAlpa/100);
    QDialog::focusOutEvent(e);
}

FindTab::FindTab(QString searchString)
{
    qDebug()<<"FindTab::FindTab";
    QVBoxLayout *wholeScreenLayout = new QVBoxLayout();

    QHBoxLayout *wholeScreenLayout_up = new QHBoxLayout();
    QVBoxLayout *wholeScreenLayout_up_left = new QVBoxLayout;

    QHBoxLayout *wholeScreenLayout_down = new QHBoxLayout();

    //0
    QHBoxLayout *searchTargetLayout = new QHBoxLayout();
    QLabel *labelSearchTarget = new QLabel(tr("Search Target:"));
    QLineEdit *LineEditSearchTarget = new QLineEdit();
    comboBox = new QComboBox;
    comboBox->setLineEdit(LineEditSearchTarget);
    comboBox->setEditText(searchString);
    comboBox->setFixedWidth(190);
    searchTargetLayout->addWidget(labelSearchTarget);
    searchTargetLayout->addWidget(comboBox);
    searchTargetLayout->setAlignment(Qt::AlignRight|Qt::AlignBottom);
    searchTargetLayout->setContentsMargins(0,1,0,0);

    wholeScreenLayout_up_left->addLayout(searchTargetLayout);
    //0

    //1
    QHBoxLayout *searchCritriaLayout = new QHBoxLayout();
    QGroupBox *markGroup = new QGroupBox();
    //markGroup->setStyle(QStyle::);
    QVBoxLayout *searchCritriaLayout_left = new QVBoxLayout();
    checkBoxMarkLine = new QCheckBox(tr("Mark Whole Line"));
    checkBoxHighlightSearchResult = new QCheckBox(tr("Highlight Search Result"));
    checkBoxClearLastMark = new QCheckBox(tr("Clear Last Mark"));
    searchCritriaLayout_left->addWidget(checkBoxMarkLine);
    searchCritriaLayout_left->addWidget(checkBoxHighlightSearchResult);
    searchCritriaLayout_left->addWidget(checkBoxClearLastMark);

    QVBoxLayout *searchCritriaLayout_right = new QVBoxLayout();
    QPushButton *buttonSearchAll = new QPushButton(tr("Search All"));
    buttonSearchAll->setFixedWidth(85);
    connect(buttonSearchAll, &QPushButton::clicked, this, &FindTab::SearchAll);
    checkBoxPickRange = new QCheckBox(tr("Picked Area"));
    checkBoxPickRange->setFixedWidth(85);
    QPushButton *buttonClear = new QPushButton(tr("Clear"));
    buttonClear->setFixedWidth(85);
    connect(buttonClear, &QPushButton::clicked, this, &FindTab::ClearMarks);
    searchCritriaLayout_right->addWidget(buttonSearchAll);
    searchCritriaLayout_right->addWidget(checkBoxPickRange);
    searchCritriaLayout_right->addWidget(buttonClear);

    searchCritriaLayout->addLayout(searchCritriaLayout_left);
    searchCritriaLayout->addLayout(searchCritriaLayout_right);

    markGroup->setLayout(searchCritriaLayout);

    wholeScreenLayout_up_left->addWidget(markGroup);
    //1

    //2
    QVBoxLayout *matchLayout = new QVBoxLayout();
    checkBoxMatchWholeWord = new QCheckBox(tr("Match Whole Word"));
    checkBoxMatchUpperLower = new QCheckBox(tr("Match upper or lower"));
    checkBoxSearchLoop = new QCheckBox(tr("Loop Search"));
    matchLayout->addWidget(checkBoxMatchWholeWord);
    matchLayout->addWidget(checkBoxMatchUpperLower);
    matchLayout->addWidget(checkBoxSearchLoop);

    wholeScreenLayout_up_left->addLayout(matchLayout);
    wholeScreenLayout_up->addLayout(wholeScreenLayout_up_left);
    //2

    //3
    QHBoxLayout *othersLayout = new QHBoxLayout();

    QGroupBox *searchModeGroup = new QGroupBox(tr("Search Mode"));
    QVBoxLayout *searchModeLayout = new QVBoxLayout();
    QRadioButton *buttonNormal = new QRadioButton(tr("Normal"));
    buttonNormal->setChecked(true);
    QRadioButton *buttonExtension = new QRadioButton(tr("Extension"));
    QRadioButton *buttonRegExp = new QRadioButton(tr("Reg Exp"));
    searchModeLayout->addWidget(buttonNormal);
    searchModeLayout->addWidget(buttonExtension);
    searchModeLayout->addWidget(buttonRegExp);
    searchModeGroup->setLayout(searchModeLayout);
    searchModeGroup->setFixedSize(190,85);
    othersLayout->addWidget(searchModeGroup);

    QGroupBox *directionGroup = new QGroupBox(tr("Drection"));
    QVBoxLayout *directionLayout = new QVBoxLayout();
    buttonUp = new QRadioButton(tr("Up"));
    QRadioButton *buttonDown = new QRadioButton(tr("Down"));
    buttonDown->setChecked(true);
    directionLayout->addWidget(buttonUp);
    directionLayout->addWidget(buttonDown);
    directionGroup->setLayout(directionLayout);
    directionGroup->setFixedSize(100,85);
    othersLayout->addWidget(directionGroup);


    QGroupBox *AlphaGroup = new QGroupBox(tr("Alpha"));
    AlphaGroup->setChecked(true);
    QVBoxLayout *alphaLayout = new QVBoxLayout();
    QRadioButton *buttonAlphaLoseFocus = new QRadioButton(tr("After lose focus"));
    QRadioButton *buttonAlphaAlways = new QRadioButton(tr("Alaways"));
    buttonAlphaAlways->setChecked(true);
    QSlider *alphaSlider = new QSlider(Qt::Horizontal);
    connect(alphaSlider, SIGNAL(valueChanged(int)),this, SLOT(valueChange(int)));
    alphaSlider->setMaximum(80);
    alphaLayout->addWidget(buttonAlphaLoseFocus);
    alphaLayout->addWidget(buttonAlphaAlways);
    alphaLayout->addWidget(alphaSlider);
    AlphaGroup->setLayout(alphaLayout);
    AlphaGroup->setCheckable(true);
    AlphaGroup->setFixedSize(130,85);
    othersLayout->addWidget(AlphaGroup);

    wholeScreenLayout_down->addLayout(othersLayout);
    wholeScreenLayout_down->setAlignment(Qt::AlignBottom);
    wholeScreenLayout_down->setSizeConstraint(QLayout::SetFixedSize);
    //3

    //4
    QVBoxLayout *buttonListLayout = new QVBoxLayout;
    QPushButton *buttonNext = new QPushButton(tr("Search next"));
    buttonNext->setFixedWidth(130);
    connect(buttonNext, &QPushButton::clicked, this, &FindTab::SearchNext);
    QPushButton *buttonCount = new QPushButton(tr("Count"));
    buttonCount->setFixedWidth(130);
    connect(buttonCount, &QPushButton::clicked, this, &FindTab::CountClicked);
    QPushButton *buttonSearchAllOpenFile = new QPushButton(tr("Search All Opened File"));
    buttonSearchAllOpenFile->setFixedWidth(130);
    connect(buttonSearchAllOpenFile, &QPushButton::clicked, this, &FindTab::SearchAllFile);
    QPushButton *buttonSearchCurrentFile = new QPushButton(tr("Search Current File"));
    buttonSearchCurrentFile->setFixedWidth(130);
    connect(buttonSearchCurrentFile, &QPushButton::clicked, this, &FindTab::SearchCurrentFile);
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    connect(buttonCancel, &QPushButton::clicked, this, &FindTab::cancel);
    buttonCancel->setFixedWidth(130);

    buttonListLayout->addWidget(buttonNext);
    buttonListLayout->addWidget(buttonCount);
    buttonListLayout->addWidget(buttonSearchAllOpenFile);
    buttonListLayout->addWidget(buttonSearchCurrentFile);
    buttonListLayout->addWidget(buttonCancel);
    buttonListLayout->setAlignment(Qt::AlignTop);
    wholeScreenLayout_up->addLayout(buttonListLayout);

    wholeScreenLayout->addLayout(wholeScreenLayout_up);
    wholeScreenLayout->addLayout(wholeScreenLayout_down);
    wholeScreenLayout->addStretch(1);
    setLayout(wholeScreenLayout);
}

ReplaceTab::ReplaceTab(QString searchString)
{
    qDebug()<<"ReplaceTab::ReplaceTab";
    QVBoxLayout *wholeScreenLayout = new QVBoxLayout();

    QHBoxLayout *wholeScreenLayout_middle = new QHBoxLayout();
    QHBoxLayout *wholeScreenLayout_down = new QHBoxLayout();

    //0
    QHBoxLayout *searchTargetLayout = new QHBoxLayout();
    QLabel *labelSearchTarget = new QLabel(tr("Search Target:"));
    QLineEdit *LineEditSearchTarget = new QLineEdit();
    QComboBox *comboBoxSearchTarget = new QComboBox;
    comboBoxSearchTarget->setLineEdit(LineEditSearchTarget);
    comboBoxSearchTarget->setEditText(searchString);
    comboBoxSearchTarget->setFixedWidth(190);
    QPushButton *buttonNext = new QPushButton(tr("Search next"));
    buttonNext->setFixedWidth(130);
    searchTargetLayout->addWidget(labelSearchTarget);
    searchTargetLayout->addWidget(comboBoxSearchTarget);
    searchTargetLayout->addWidget(buttonNext);
    searchTargetLayout->setAlignment(Qt::AlignRight|Qt::AlignTop);

    QHBoxLayout *replaceTargetLayout = new QHBoxLayout();
    QLabel *labelReplaceTarget = new QLabel(tr("Replace As:"));
    QLineEdit *LineEditReplaceTarget = new QLineEdit();
    QComboBox *comboBoxReplaceTarget = new QComboBox;
    comboBoxReplaceTarget->setLineEdit(LineEditReplaceTarget);
    comboBoxReplaceTarget->setFixedWidth(190);
    QPushButton *buttonReplace = new QPushButton(tr("Replace"));
    buttonReplace->setFixedWidth(130);
    replaceTargetLayout->addWidget(labelReplaceTarget);
    replaceTargetLayout->addWidget(comboBoxReplaceTarget);
    replaceTargetLayout->addWidget(buttonReplace);
    replaceTargetLayout->setAlignment(Qt::AlignRight);

    QHBoxLayout *layout = new QHBoxLayout;
    QGroupBox *groupReplaceAll = new QGroupBox;
    QHBoxLayout *layoutReplaceAll = new QHBoxLayout;
    QCheckBox *checkBoxReplaceAll = new QCheckBox(tr("Picked Area"));
    QPushButton *buttonReplaceAllOpenFile = new QPushButton(tr("Replace All"));
    buttonReplaceAllOpenFile->setFixedWidth(130);
    layoutReplaceAll->addWidget(checkBoxReplaceAll);
    layoutReplaceAll->addWidget(buttonReplaceAllOpenFile);
    layoutReplaceAll->setAlignment(Qt::AlignRight);
    layoutReplaceAll->setMargin(0);
    groupReplaceAll->setLayout(layoutReplaceAll);
    groupReplaceAll->setFixedSize(QSize(230,40));
    groupReplaceAll->setAlignment(Qt::AlignRight);
    groupReplaceAll->setContentsMargins(0,0,0,0);
    layout->addWidget(groupReplaceAll);
    layout->setAlignment(Qt::AlignRight);
    layout->setContentsMargins(0,0,0,0);

    wholeScreenLayout->addLayout(searchTargetLayout);
    wholeScreenLayout->addLayout(replaceTargetLayout);
    wholeScreenLayout->addLayout(layout);
    //0

    //1
    /*QHBoxLayout *searchCritriaLayout = new QHBoxLayout();

    QGroupBox *markGroup = new QGroupBox();
    QVBoxLayout *searchCritriaLayout_left = new QVBoxLayout();
    QCheckBox *checkBoxHighlightSearchResult = new QCheckBox(tr("Highlight Search Result"));
    QCheckBox *checkBoxClearLastMark = new QCheckBox(tr("Clear Last Mark"));
    searchCritriaLayout_left->addWidget(checkBoxHighlightSearchResult);
    searchCritriaLayout_left->addWidget(checkBoxClearLastMark);
    markGroup->setLayout(searchCritriaLayout_left);
    markGroup->setVisible(false);
    QSizePolicy sp_retain_markGroup = markGroup->sizePolicy();
    sp_retain_markGroup.setRetainSizeWhenHidden(true);
    markGroup->setSizePolicy(sp_retain_markGroup);

    QGroupBox *rangeGroup = new QGroupBox();
    QVBoxLayout *searchCritriaLayout_right = new QVBoxLayout();
    QCheckBox *checkBoxPickRange = new QCheckBox(tr("Pick From Range"));
    QPushButton *buttonClear = new QPushButton(tr("Clear"));
    searchCritriaLayout_right->addWidget(checkBoxPickRange);
    searchCritriaLayout_right->addWidget(buttonClear);
    rangeGroup->setLayout(searchCritriaLayout_right);
    rangeGroup->setVisible(false);
    QSizePolicy sp_retain = rangeGroup->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    rangeGroup->setSizePolicy(sp_retain);
    rangeGroup->setVisible(false);

    searchCritriaLayout->addWidget(markGroup);
    searchCritriaLayout->addWidget(rangeGroup);

    wholeScreenLayout_up_left->addLayout(searchCritriaLayout);*/
    //1

    //2
    QVBoxLayout *matchLayout = new QVBoxLayout();
    QCheckBox *checkBoxMatchWholeWord = new QCheckBox(tr("Match Whole Word"));
    QCheckBox *checkBoxMatchUpperLower = new QCheckBox(tr("Match upper or lower"));
    QCheckBox *checkBoxSearchLoop = new QCheckBox(tr("Loop Search"));
    matchLayout->addWidget(checkBoxMatchWholeWord);
    matchLayout->addWidget(checkBoxMatchUpperLower);
    matchLayout->addWidget(checkBoxSearchLoop);
    matchLayout->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
    matchLayout->setContentsMargins(0,16,0,0);
    wholeScreenLayout_middle->addLayout(matchLayout);

    QVBoxLayout *layoutButton = new QVBoxLayout;
    QPushButton *buttonReplaceCurrentFile = new QPushButton(tr("Replace All Opened File"));
    buttonReplaceCurrentFile->setFixedSize(QSize(130,30));
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    buttonCancel->setFixedWidth(130);
    layoutButton->addWidget(buttonReplaceCurrentFile);
    layoutButton->addWidget(buttonCancel);
    layoutButton->setAlignment(Qt::AlignTop|Qt::AlignRight);
    wholeScreenLayout_middle->addLayout(layoutButton);
    //2

    //3
    QHBoxLayout *othersLayout = new QHBoxLayout();

    QGroupBox *searchModeGroup = new QGroupBox(tr("Search Mode"));
    QVBoxLayout *searchModeLayout = new QVBoxLayout();
    QRadioButton *buttonNormal = new QRadioButton(tr("normal"));
    QRadioButton *buttonExtension = new QRadioButton(tr("Extension"));
    QRadioButton *buttonRegExp = new QRadioButton(tr("Reg Exp"));
    searchModeLayout->addWidget(buttonNormal);
    searchModeLayout->addWidget(buttonExtension);
    searchModeLayout->addWidget(buttonRegExp);
    searchModeGroup->setLayout(searchModeLayout);
    searchModeGroup->setFixedSize(190,85);
    othersLayout->addWidget(searchModeGroup);

    QGroupBox *directionGroup = new QGroupBox(tr("Drection"));
    QVBoxLayout *directionLayout = new QVBoxLayout();
    QRadioButton *buttonUp = new QRadioButton(tr("Up"));
    QRadioButton *buttonDown = new QRadioButton(tr("Down"));
    directionLayout->addWidget(buttonUp);
    directionLayout->addWidget(buttonDown);
    directionGroup->setLayout(directionLayout);
    directionGroup->setFixedSize(100,85);
    othersLayout->addWidget(directionGroup);


    QGroupBox *AlphaGroup = new QGroupBox(tr("Alpha"));
    QVBoxLayout *alphaLayout = new QVBoxLayout();
    QRadioButton *buttonAlphaLoseFocus = new QRadioButton(tr("After lose focus"));
    QRadioButton *buttonAlphaAlways = new QRadioButton(tr("Alaways"));
    QSlider *alphaSlider = new QSlider(Qt::Horizontal);
    alphaLayout->addWidget(buttonAlphaLoseFocus);
    alphaLayout->addWidget(buttonAlphaAlways);
    alphaLayout->addWidget(alphaSlider);
    AlphaGroup->setLayout(alphaLayout);
    AlphaGroup->setCheckable(true);
    AlphaGroup->setFixedSize(130,85);
    othersLayout->addWidget(AlphaGroup);

    wholeScreenLayout_down->addLayout(othersLayout);
    wholeScreenLayout_down->setAlignment(Qt::AlignBottom);
    wholeScreenLayout_down->setSizeConstraint(QLayout::SetFixedSize);
    //3

    wholeScreenLayout->addLayout(wholeScreenLayout_middle);
    wholeScreenLayout->addLayout(wholeScreenLayout_down);
    wholeScreenLayout->addStretch(1);
    setLayout(wholeScreenLayout);
}

DocumentSearchTab::DocumentSearchTab(QString searchString)
{
    qDebug()<<"DocumentSearchTab::DocumentSearchTab";
    QVBoxLayout *wholeScreenLayout = new QVBoxLayout();

    QHBoxLayout *wholeScreenLayout_middle = new QHBoxLayout();
    QHBoxLayout *wholeScreenLayout_down = new QHBoxLayout();

    //0
    QHBoxLayout *searchTargetLayout = new QHBoxLayout();
    QLabel *labelSearchTarget = new QLabel(tr("Search Target:"));
    QLineEdit *LineEditSearchTarget = new QLineEdit();
    QComboBox *comboBoxSearchTarget = new QComboBox;
    comboBoxSearchTarget->setLineEdit(LineEditSearchTarget);
    comboBoxSearchTarget->setEditText(searchString);
    comboBoxSearchTarget->setFixedWidth(190);
    QPushButton *buttonNext = new QPushButton(tr("Search All"));
    buttonNext->setFixedWidth(130);
    searchTargetLayout->addWidget(labelSearchTarget);
    searchTargetLayout->addWidget(comboBoxSearchTarget);
    searchTargetLayout->addWidget(buttonNext);
    searchTargetLayout->setAlignment(Qt::AlignRight|Qt::AlignTop);

    QHBoxLayout *replaceTargetLayout = new QHBoxLayout();
    QLabel *labelReplaceTarget = new QLabel(tr("Replace As:"));
    QLineEdit *LineEditReplaceTarget = new QLineEdit();
    QComboBox *comboBoxReplaceTarget = new QComboBox;
    comboBoxReplaceTarget->setLineEdit(LineEditReplaceTarget);
    comboBoxReplaceTarget->setFixedWidth(190);
    QPushButton *buttonReplace = new QPushButton(tr("Replace in Document"));
    buttonReplace->setFixedWidth(130);
    replaceTargetLayout->addWidget(labelReplaceTarget);
    replaceTargetLayout->addWidget(comboBoxReplaceTarget);
    replaceTargetLayout->addWidget(buttonReplace);
    replaceTargetLayout->setAlignment(Qt::AlignRight);

    QHBoxLayout *docTypeLayout = new QHBoxLayout();
    QLabel *labelDocType = new QLabel(tr("Document Type"));
    QLineEdit *LineEditDocType = new QLineEdit();
    QComboBox *comboBoxDocType = new QComboBox;
    comboBoxDocType->setLineEdit(LineEditDocType);
    comboBoxDocType->setFixedWidth(190);
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    buttonCancel->setFixedWidth(130);
    docTypeLayout->addWidget(labelDocType);
    docTypeLayout->addWidget(comboBoxDocType);
    docTypeLayout->addWidget(buttonCancel);
    docTypeLayout->setAlignment(Qt::AlignRight);

    QHBoxLayout *pathLayout = new QHBoxLayout;
    QLabel *pathLabel = new QLabel(tr("Path:"));
    QLineEdit *pathLineEdit = new QLineEdit;
    QComboBox *pathComboBox = new QComboBox;
    pathComboBox->setLineEdit(pathLineEdit);
    pathComboBox->setMinimumWidth(190);
    QPushButton *pathButton = new QPushButton(tr("..."));
    pathButton->setFixedWidth(30);
    QCheckBox *pathCheckBox = new QCheckBox(tr("Follow current doc"));
    pathCheckBox->setFixedWidth(130);
    pathLayout->setAlignment(Qt::AlignRight);
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathComboBox);
    pathLayout->addWidget(pathButton);
    pathLayout->addWidget(pathCheckBox);

    wholeScreenLayout->addLayout(searchTargetLayout);
    wholeScreenLayout->addLayout(replaceTargetLayout);
    wholeScreenLayout->addLayout(docTypeLayout);
    wholeScreenLayout->addLayout(pathLayout);
    //0

    //2
    QVBoxLayout *matchLayout = new QVBoxLayout();
    QCheckBox *checkBoxMatchWholeWord = new QCheckBox(tr("Match Whole Word"));
    QCheckBox *checkBoxMatchUpperLower = new QCheckBox(tr("Match upper or lower"));
    QCheckBox *checkBoxSearchLoop = new QCheckBox(tr("Loop Search"));
    QSizePolicy policyLoop = checkBoxSearchLoop->sizePolicy();
    policyLoop.setRetainSizeWhenHidden(true);
    checkBoxSearchLoop->setSizePolicy(policyLoop);
    checkBoxSearchLoop->setVisible(false);
    matchLayout->addWidget(checkBoxMatchWholeWord);
    matchLayout->addWidget(checkBoxMatchUpperLower);
    matchLayout->addWidget(checkBoxSearchLoop);
    matchLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    matchLayout->setContentsMargins(0,4,0,0);

    QVBoxLayout *includeLayout = new QVBoxLayout;
    QCheckBox *includeSubCheckBox = new QCheckBox(tr("Include Sub folder"));
    includeSubCheckBox->setFixedWidth(130);
    QCheckBox *includeHideCheckBox = new QCheckBox(tr("Include Hide folder"));
    includeHideCheckBox->setFixedWidth(130);
    includeLayout->addWidget(includeSubCheckBox);
    includeLayout->addWidget(includeHideCheckBox);
    includeLayout->setAlignment(Qt::AlignTop|Qt::AlignRight);

    wholeScreenLayout_middle->addLayout(matchLayout);
    wholeScreenLayout_middle->addLayout(includeLayout);
    //2

    //3
    QHBoxLayout *othersLayout = new QHBoxLayout();

    QGroupBox *searchModeGroup = new QGroupBox(tr("Search Mode"));
    QVBoxLayout *searchModeLayout = new QVBoxLayout();
    QRadioButton *buttonNormal = new QRadioButton(tr("normal"));
    QRadioButton *buttonExtension = new QRadioButton(tr("Extension"));
    QRadioButton *buttonRegExp = new QRadioButton(tr("Reg Exp"));
    searchModeLayout->addWidget(buttonNormal);
    searchModeLayout->addWidget(buttonExtension);
    searchModeLayout->addWidget(buttonRegExp);
    searchModeGroup->setLayout(searchModeLayout);
    searchModeGroup->setFixedSize(190,85);
    othersLayout->addWidget(searchModeGroup);

    QGroupBox *directionGroup = new QGroupBox(tr("Drection"));
    QVBoxLayout *directionLayout = new QVBoxLayout();
    QRadioButton *buttonUp = new QRadioButton(tr("Up"));
    QRadioButton *buttonDown = new QRadioButton(tr("Down"));
    directionLayout->addWidget(buttonUp);
    directionLayout->addWidget(buttonDown);
    directionGroup->setLayout(directionLayout);
    directionGroup->setFixedSize(100,85);
    QSizePolicy sizePolicy = directionGroup->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden(true);
    directionGroup->setSizePolicy(sizePolicy);
    directionGroup->setVisible(false);
    othersLayout->addWidget(directionGroup);

    QGroupBox *AlphaGroup = new QGroupBox(tr("Alpha"));
    QVBoxLayout *alphaLayout = new QVBoxLayout();
    QRadioButton *buttonAlphaLoseFocus = new QRadioButton(tr("After lose focus"));
    QRadioButton *buttonAlphaAlways = new QRadioButton(tr("Alaways"));
    QSlider *alphaSlider = new QSlider(Qt::Horizontal);
    alphaLayout->addWidget(buttonAlphaLoseFocus);
    alphaLayout->addWidget(buttonAlphaAlways);
    alphaLayout->addWidget(alphaSlider);
    AlphaGroup->setLayout(alphaLayout);
    AlphaGroup->setCheckable(true);
    AlphaGroup->setFixedSize(130,85);
    othersLayout->addWidget(AlphaGroup);

    wholeScreenLayout_down->addLayout(othersLayout);
    wholeScreenLayout_down->setAlignment(Qt::AlignBottom);
    wholeScreenLayout_down->setSizeConstraint(QLayout::SetFixedSize);
    //3

    wholeScreenLayout->addLayout(wholeScreenLayout_middle);
    wholeScreenLayout->addLayout(wholeScreenLayout_down);
    wholeScreenLayout->addStretch(1);
    setLayout(wholeScreenLayout);
}

