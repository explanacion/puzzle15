#ifndef PUZZLE15_H
#define PUZZLE15_H

#include <QWidget>
#include <QCloseEvent>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <windows.h>
#include <sddl.h>
#include <accctrl.h>
#include <Aclapi.h>
#include <QTimer>

class QGridLayout;
class QPoint;
class QPushButton;

namespace Ui {
class Puzzle15;
}

// переопределяю класс Qlabel для ячеек
class QGameLabel : public QLabel
{
    Q_OBJECT
public:
    QGameLabel(QWidget *parent=0);
    void setIndex(int ind);
    int index; // индекс фрагмента изображения
    QObject *object; // родительский объект (экземпляр Puzzle15)
public slots:
    void slotClicked(); // слот обработки клика на ячейке
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent *e);
private:
};

class Puzzle15 : public QWidget
{
    Q_OBJECT

public:
    explicit Puzzle15(QWidget *parent = 0);
    QGridLayout* grid; // компоновщик виджетов в сетку
    int keycellpos;
    QList<int> numbers;
    QTimer *newtimer;
    ~Puzzle15();

public slots:
    void move(int labelpos);
    void WatchTskMgr();

private:
    virtual void closeEvent(QCloseEvent *event);
    virtual void focusEvent(QFocusEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void changeEvent(QEvent *ev);
    // secret key combination
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    void createButton(QPushButton*, int, int, int);
    void checkGameOver();
    bool isRunning(const QString &process);
    void CloseApp(QString appname);
    QList<QPixmap> desktopPieces; // части исходного рисунка
    QGameLabel imglabels[16];
    QList<QPushButton*> buttons;
    Ui::Puzzle15 *ui;

    int skey;
};

inline int pos2x(int pos) {
    return pos % 4;
}

inline int pos2y(int pos) {
    return pos / 4;
}

#endif // PUZZLE15_H
