#include "puzzle15.h"
#include "ui_puzzle15.h"
#include <QScreen>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QProcess>
#include <QTime>

Puzzle15::Puzzle15(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Puzzle15)
{
    ui->setupUi(this);
    skey = 0;

    //QString documentsdir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0);
    //qDebug() << documentsdir;
    //QDesktopServices::openUrl(QUrl::fromLocalFile(documentsdir));
    // снимок экрана
    QPixmap originalPixmap;
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        originalPixmap = screen->grabWindow(0);
    }
    //originalPixmap содержит снимок
    // делим экран на 16 частей, 4 на 4

    int horizontalPace = int(originalPixmap.width() / 4.0);
    int verticalPace = int(originalPixmap.height() / 4.0);
    for (int j = 0; j < originalPixmap.height(); j = j + verticalPace) {
        for (int i = 0; i < originalPixmap.width(); i = i + horizontalPace)
        {
            QRect rect(i, j, horizontalPace, verticalPace);
            QPixmap piece = originalPixmap.copy(rect);
            desktopPieces.append(piece);
        }
    }
    // Далее следует генерация расклада
    // в контейнере numbers содержатся позиции фрагментов (сначала по порядку)
    numbers << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15;

    //qDebug() << numbers;
    // перемешиваем позиции фрагментов случайным образом
    int threshold = 0;
    // критерий окончания генерации поля - число элементов не на своей позиции не менее 2/3
    int n = numbers.length() * 2/3;

    while (threshold < n) {
        int temp=0;
        for (int i=0; i<16; i++)
        {
            if (numbers[i] != i) temp++;
        }

        threshold = temp;
        // случайная перестановка
        int blankpos = numbers.indexOf(15);
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int elem = qrand() % 4;
        if (elem == 0) {
            // 1 путь - левый элемент
            if (pos2x(blankpos) > 0) { numbers.swap(blankpos,blankpos-1); continue; }
            else { elem++; }
        }
        if (elem == 1) {
            // 2 путь - верхний элемент
            if (pos2y(blankpos) > 0) { numbers.swap(blankpos, blankpos-4); continue; }
            else { elem++; }
        }
        if (elem == 2) {
            // 3 путь - правый
            if (pos2x(blankpos) < 3) { numbers.swap(blankpos, blankpos+1); continue; }
            else { elem++; }
        }
        if (elem == 3) {
            // 4 - нижний
            if (pos2y(blankpos) < 3) { numbers.swap(blankpos,blankpos+4); continue; }
            else { elem++; }
        }
    }




    // пустой элемент для игровой ячейки
    QPixmap white = desktopPieces.last();
    white.fill(Qt::white);
    desktopPieces.append(white);

    // настройка сетки виджетов
    grid = new QGridLayout(this);
    grid->setHorizontalSpacing(1);
    grid->setVerticalSpacing(1);
    grid->setContentsMargins(2, 2, 2, 2);

    // отрисовываем фрагменты в соответствии с их текущими позициями в numbers
    int k = 0;
    for (int j=0; j < 4; j++) {
        for (int i=0; i < 4; i++) {
            if (k > 15) break;
            imglabels[k].setPixmap(desktopPieces[numbers[k]]);
            imglabels[k].setIndex(numbers[k]);
            if (numbers[k] == 15) { keycellpos = k; imglabels[k].setPixmap(desktopPieces.last()); }
            imglabels[k].object = this;
            // imglabels[k].setPixmap(desktopPieces[k]); // по порядку
            grid->addWidget(&imglabels[k],j,i);
            k++;
        }
    }

    setLayout(grid);
    newtimer = new QTimer();
    connect(newtimer,SIGNAL(timeout()),this,SLOT(WatchTskMgr()));
    newtimer->start(500);
}

void Puzzle15::WatchTskMgr() {
    if (isRunning("taskmgr.exe")) CloseApp("taskmgr.exe");
    if (isRunning("Taskmgr.exe")) CloseApp("Taskmgr.exe");
    if (isRunning("cmd.exe")) CloseApp("cmd.exe");
}

// определение важных функций перегруженного label
QGameLabel::QGameLabel(QWidget *parent) : QLabel(parent)
{
    connect(this,SIGNAL(clicked()),this,SLOT(slotClicked()));
}

void QGameLabel::setIndex(int ind) {
    this->index = ind;
}

void QGameLabel::slotClicked()
{
    // обрабатываем клик по ячейке
    Puzzle15 *obj = qobject_cast<Puzzle15 *>(object);

    // calculate the item we just clicked
    int x;
    int y;
    int xs;
    int ys;
    int ind = obj->grid->indexOf(this);
    obj->grid->getItemPosition(ind,&y,&x,&xs,&ys);
    int position = (x)+4*(y);
    //qDebug() << "pressed! " + QString::number(position);
    obj->move(position);
}

void QGameLabel::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit clicked(); // генерация сигнала нажатия
    }
}

// является ли ячейка cellforcheck смежной по отношению к curpos (можно ли её двигать)
bool isadjoiningcell(int curpos, int cellforcheck)
{
    int x = curpos;
    int y = cellforcheck;
    if (x-y == 4 || x-y == -4 || x-y == 1 || x-y == -1)
    {
        if (pos2x(x) == pos2x(y)) return true;
        if (pos2y(x) == pos2y(y)) return true;
    }
    return false;
}

// процедура хода, обрабатывающая нажатие мыши и меняющая ячейки местами
void Puzzle15::move(int clickedpos)
{
    //qDebug() << "clickedpos is " + QString::number(clickedpos);
    //qDebug() << "keycellpos is " + QString::number(keycellpos);
    if (clickedpos == keycellpos) return;
    // проверяем, надо ли обрабатывать это нажатие

    if (isadjoiningcell(clickedpos, keycellpos))
    {
        numbers.swap(clickedpos,keycellpos);
        // определяем номер фрагмента, которому соответствует кликнутая позиция
        int clickedpiece = imglabels[clickedpos].index;
        // отображаем новые изменения на лейблах
        // пустая ячейка копирует ранее нажатую
        QPixmap tmpimage = imglabels[clickedpos].pixmap()->copy();
        imglabels[keycellpos].setPixmap(tmpimage);
        // нажатая ячейка становится ключевой (пустой)
        imglabels[clickedpos].setPixmap(desktopPieces.last());
        keycellpos = clickedpos;
        imglabels[keycellpos].index = clickedpiece;
        imglabels[clickedpos].index = imglabels[keycellpos].index;
        // qDebug() << "keycellpos is " + QString::number(keycellpos);

        // проверяем, достигнут ли финал игры
        checkGameOver();
    }
}

void Puzzle15::checkGameOver()
{
    for (int i = 0; i<15; i++)
    {
        if (numbers[i] > numbers[i+1]) {
            return;
        }
    }
    Beep(523,500);
    skey = 3;
    close();
}

void Puzzle15::closeEvent(QCloseEvent *event)
{
    if (skey < 3)
        event->ignore();
}

// regain the focus
void Puzzle15::focusEvent(QFocusEvent *event)
{
    qDebug() << "focus";
    if (event->type() == QEvent::ActionChanged)
    {
        if (!this->isActiveWindow())
        {
            this->raise();
            this->activateWindow();
            this->showMaximized();
        }
    }
}

void Puzzle15::resizeEvent(QResizeEvent *event)
{
    //event->ignore();
}
void Puzzle15::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::WindowStateChange)
    {
        this->raise();
        this->activateWindow();
        this->showMaximized();
    }
}

// if task manager is running
bool Puzzle15::isRunning(const QString &process) {
                  QProcess tasklist;
                  tasklist.start(
                        "tasklist",
                        QStringList() << "/NH"
                                      << "/FO" << "CSV"
                                      << "/FI" << QString("IMAGENAME eq %1").arg(process));
                  tasklist.waitForFinished();
                  QString output = tasklist.readAllStandardOutput();

                  return output.startsWith(QString("\"%1").arg(process));
}

void Puzzle15::CloseApp(QString appname)
{
    QProcess taskkill;
    QString app = QString("taskkill /f /IM ") + appname;
    taskkill.start(app);
    taskkill.waitForFinished();
}

void Puzzle15::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_L:
            skey++;
            break;
        case Qt::Key_A:
            skey++;
            break;
        case Qt::Key_B:
            skey++;
            break;
    }
    if (skey >= 3) { this->close(); }
}

void Puzzle15::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_L:
            skey=0;
            break;
        case Qt::Key_A:
            skey=0;
            break;
        case Qt::Key_B:
            skey=0;
            break;
    }
}

Puzzle15::~Puzzle15()
{
    delete ui;
}



