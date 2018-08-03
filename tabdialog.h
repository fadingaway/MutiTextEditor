#ifndef TABDIALOG_H
#define TABDIALOG_H

#include "QTabWidget"
#include "QDialog"
#include "QThread"
#include "QComboBox"
#include "QCheckBox"
#include "QRadioButton"
class FindTab;
class ReplaceTab;
class DocumentSearchTab;
class TabDialog:public QDialog
{
    Q_OBJECT
public:
    explicit TabDialog(QString searchString, int tabIndex, QWidget *parent);

signals:
    void tabChanged(int tabIndex);
private slots:
    void updateWindowTitle(int tab_Id);
    void setTabAlpha(int value);
public:
    QTabWidget *tabWidget;
    double currentAlpa = 0;
    FindTab *findtab;
    ReplaceTab *replaceTab;
    DocumentSearchTab *documentTab;
protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
};

class FindTab:public QWidget
{
    Q_OBJECT
public:
    explicit FindTab(QString searchString = 0);
    QString getSearchString();
signals:
    void notifyTabWidget(int value);
    void notifySearchAllClicked(QString searchString, bool markLine, bool highlightResult, bool clearMark);
    void notifySearchNextClicked(QString searchString, bool matchWholeWord, bool matchCaseSencitive,
                                 bool searchInLoop, bool searchDirection);
    void ClearMarkClicked();
    void notifyCountClicked();
    void notifySearchAllOpenedFileClicked();
    void notifySearchCurrentOpenedFileClicked();
    void notifyCancelClicked();
public slots:
    void valueChange(int value);
    void SearchAll();
    void SearchNext();
    void ClearMarks();
    void CountClicked();
    void SearchAllFile();
    void SearchCurrentFile();
    void cancel();
private:
    QComboBox *comboBox;
    QCheckBox *checkBoxMarkLine;
    QCheckBox *checkBoxHighlightSearchResult;
    QCheckBox *checkBoxClearLastMark;
    QCheckBox *checkBoxPickRange;
    QCheckBox *checkBoxMatchWholeWord;
    QCheckBox *checkBoxMatchUpperLower;
    QCheckBox *checkBoxSearchLoop;
    QRadioButton *buttonUp;
};

class ReplaceTab:public QWidget
{
    Q_OBJECT
public:
    explicit ReplaceTab(QString searchString = 0);
};

class DocumentSearchTab:public QWidget
{
    Q_OBJECT
public:
    explicit DocumentSearchTab(QString searchString = 0);
};

#endif // TABDIALOG_H
