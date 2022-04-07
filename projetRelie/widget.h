#ifndef WIDGET_H
#define WIDGET_H

#include "Gamme.h"
#include "variables.h"

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QSize>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QImage>
#include <QRect>
#include <QMessageBox>

#include <SDL/SDL_mixer.h>

//include pour code thérémine
#include <CppLinuxSerial/SerialPort.hpp>
#include <iostream>
#include <cmath>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "son.h"

using namespace mn::CppLinuxSerial;

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    Mix_Music *musique;
    QTimer * loopTimer=new QTimer(this);
    QTimer * repaintTimer=new QTimer(this);
    int screenWidth;
    int screenHeight;
    double distance = 0;
    double valeur_precedente = 0.0;
    double valeur_actuelle = 0.0;
    double valeur_future = 0.0;
    string lidarData = "";
    string distance_L = "";
    string distance_H = "";
    string strenght_L = "";
    string strenght_H = "";
    string temp_L = "";
    string temp_H = "";
    string checksum = "";
    SerialPort serialPort;
    int loopCount;
    bool inLoop=false;
    bool musicOn=false;
    bool distanceFound = false;

    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void initGammes();
    void displayCopyrights();
    void displayNotesText();
    void displayNotesImage();
    void displayBoutonsGamme();
    void displayBoutonPlay();
    bool deuxValeurSurTroisHorsLimite();
    void calibrageDistanceMax();
    void restartALSAServer();

    Note* noteActuelle = new Note();
    Note* noteParDefault = new Note();

    Gamme* gammeBlues = new Gamme("Blues");
    Gamme* gammeMineure = new Gamme("Mineure");
    Gamme* gammeJaponaise = new Gamme("Japonaise");

    Gamme* gammeActuelle = new Gamme("Blues");

private slots:
    void tryLoop();
    void loop();

    void setValeurMin();
    void setValeurMax();
};
#endif // WIDGET_H
