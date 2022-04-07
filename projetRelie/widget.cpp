#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

using namespace std;

double DISTANCE_MAX = 5.0;
double DISTANCE_MIN = 0.1;

int REVERB;
int VOLUME = 0;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->showFullScreen();
    this->setStyleSheet("background-color:black;");

    ui->distanceMaxSpinBox->setStyleSheet(" background-color: rgba(0,0,0,0);");
    ui->distanceMinSpinBox->setStyleSheet(" background-color: rgba(0,0,0,0);");

    connect(ui->distanceMinSpinBox,SIGNAL(valueChanged(double)),SLOT(setValeurMin()));
    connect(ui->distanceMaxSpinBox,SIGNAL(valueChanged(double)),SLOT(setValeurMax()));

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);// Défini ce qui joue la musique
    musique = Mix_LoadMUS("../projetRelie/resources/musique.wav"); // Chargement de la musique
    Mix_PlayMusic(musique, -1); // Jouer infiniment la musique
    Mix_PauseMusic();

    system("sudo chmod 777 /dev/serial0");

    SerialPort widgetSerialPort("/dev/serial0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort = widgetSerialPort;
    serialPort.SetTimeout(-1);
    serialPort.Open();

    buzzer_start();

    screenHeight = 1100;
    screenWidth = 1900;

    initGammes();
    *gammeActuelle=*gammeBlues;


    calibrageDistanceMax();
    sleep(2);

    connect(loopTimer, SIGNAL(timeout()), this, SLOT(tryLoop()));
    loopTimer->start(1);
    connect(repaintTimer, SIGNAL(timeout()), this, SLOT(repaint()));
    repaintTimer->start(33);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initGammes()
{
    //Octave 3
    Note mib3("mib3",	311.13, true,   2);		//63
    Note fa3("fa3",		349.23, false,  3);             //65
    Note solb3("solb3",	369.99, true,   4);		//66
    Note lab3("lab3",	415.30, true,   5);		//68
    Note la3("la3",		440.00, false,  5);		        //69
    Note sib3("sib3",	466.16, true,   6);		//70
    Note si3("si3",		493.88, false,  6);		        //71

    //Octave 4
    Note reb4("reb4",	554.37, true,   8);		//73
    Note mib4("mib4",	622.25, true,   9);		//75
    Note fa4("fa4",		698.46, false,  10);		        //77
    Note solb4("solb4",	739.99, true,   11);		//78
    Note lab4("lab4",	830.61, true,   12);		//80
    Note la4("la4",		880.00, false,  12);		        //81
    Note sib4("sib4",	932.33, true,   13);		//82
    Note si4("si4",		987.77, false,  13);		        //82

    //Octave 5
    Note reb5("reb5",	1108.73, true,  15);		//85
    Note mib5("mib5",	1244.51, true,  16);		//85
    Note fa5("fa5",		1396.91, false, 17);		    //85

    gammeBlues->ajouteNotes(
            mib3,
            solb3,
            lab3,
            la3,
            sib3,
            reb4,
            mib4,
            solb4,
            lab4,
            la4,
            sib4,
            reb5
        );
    int index=1;
    for(Note &uneNote : gammeBlues->vecteurDeNotes)
    {
        uneNote.dimensionsNote = QRect((screenWidth/14)*(index+0.8),((60+3.725*(-uneNote.hauteur))*screenHeight)/144, screenWidth/21, screenHeight/5.4);
        if(uneNote.estBemol) uneNote.path = ":/new/prefix1/images/resources/NoteBemol.png";
        else uneNote.path = ":/new/prefix1/images/resources/Note.png";
        index++;
    }

    gammeMineure->ajouteNotes(
        mib3,
        fa3,
        solb3,
        lab3,
        sib3,
        si3,
        reb4,
        mib4,
        fa4,
        solb4,
        lab4,
        sib4
    );
    index=1;
    for(Note &uneNote : gammeMineure->vecteurDeNotes)
    {
        uneNote.dimensionsNote = QRect((screenWidth/14)*(index+0.8),((60+3.725*(-uneNote.hauteur))*screenHeight)/144, screenWidth/21, screenHeight/5.4);
        if(uneNote.estBemol) uneNote.path = ":/new/prefix1/images/resources/NoteBemol.png";
        else uneNote.path = ":/new/prefix1/images/resources/Note.png";
        index++;
    }

    gammeJaponaise->ajouteNotes(
        mib3,
        fa3,
        solb3,
        sib3,
        si3,
        mib4,
        fa4,
        solb4,
        sib4,
        si4,
        mib5,
        fa5
    );
    index=1;
    for(Note &uneNote : gammeJaponaise->vecteurDeNotes)
    {
        uneNote.dimensionsNote = QRect((screenWidth/14)*(index+0.8),((60+3.725*(-uneNote.hauteur))*screenHeight)/144, screenWidth/21, screenHeight/5.4);
        if(uneNote.estBemol) uneNote.path = ":/new/prefix1/images/resources/NoteBemol.png";
        else uneNote.path = ":/new/prefix1/images/resources/Note.png";
        index++;
    }
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(event->x()>screenWidth/8-8 and event->x()<screenWidth/8+106 and event->y()>(screenHeight/12)*11-38 and event->y()<(screenHeight/12)*11+12)  *gammeActuelle=*gammeBlues;
        else if(event->x()>(screenWidth/8)*3.5-8
                and event->x()<(screenWidth/8)*3.5+160
                and event->y()>(screenHeight/12)*11-38
                and event->y()<(screenHeight/12)*11+12) *gammeActuelle=*gammeMineure;
        else if(event->x()>(screenWidth/8)*6-8
                and event->x()<(screenWidth/8)*6+187
                and event->y()>(screenHeight/12)*11-38
                and event->y()<(screenHeight/12)*11+12) *gammeActuelle=*gammeJaponaise;
        else if(event->x()>(screenWidth/2.25)
                and event->x()<(screenWidth/2.25)+(screenWidth/15)
                and event->y()>(screenHeight/12)*7.5
                and event->y()<(screenHeight/12)*7.5+screenWidth/15) musicOn = !musicOn;
    }

    //(screenWidth/2.25,(screenHeight/12)*7.5,screenWidth/15,screenWidth/15);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    // TODO : Mettre toutes les gammes dans une liste et itérer dessus pour que ça marche peut importe le nb de gammes

    if(event->key()==Qt::Key_Plus)
    {
        system("+");
    }
    if(event->key()==Qt::Key_Minus)
    {
        system("-");
    }


    if(event->key()==Qt::Key_Left)
    {
        if(gammeActuelle->nomGamme=="Blues") *gammeActuelle=*gammeJaponaise;
        else if(gammeActuelle->nomGamme=="Japonaise") *gammeActuelle=*gammeMineure;
        else if(gammeActuelle->nomGamme=="Mineure") *gammeActuelle=*gammeBlues;
    }

    if(event->key()==Qt::Key_Right)
    {
        if(gammeActuelle->nomGamme=="Blues") *gammeActuelle=*gammeMineure;
        else if(gammeActuelle->nomGamme=="Mineure") *gammeActuelle=*gammeJaponaise;
        else if(gammeActuelle->nomGamme=="Japonaise") *gammeActuelle=*gammeBlues;
    }

    if(event->key()==Qt::Key_B)
    {
        *gammeActuelle=*gammeBlues;
    }

    if(event->key()==Qt::Key_M)
    {
        *gammeActuelle=*gammeMineure;
    }

    if(event->key()==Qt::Key_J)
    {
        *gammeActuelle=*gammeJaponaise;
    }

    if(event->key()==Qt::Key_Escape)
    {
        loopTimer->stop();
        QMessageBox *messageBox = new QMessageBox("IHM Thérémine",
                                                  "Quitter ?",
                                                  QMessageBox::Question,
                                                  QMessageBox::Ok,
                                                  QMessageBox::Cancel | QMessageBox::Default | QMessageBox::Escape,0);
        int res = messageBox->exec();
        if (res == QMessageBox::Ok){
            this->close();
        }
        loopTimer->start();
        serialPort.Open();
    }    

    if(event->key()==Qt::Key_Space or event->key()==Qt::Key_P)
    {
        musicOn=!musicOn;
        if(musicOn)
        {
            Mix_ResumeMusic();
        }
        else
        {
            Mix_PauseMusic();
        }
    }
}

void Widget::displayCopyrights()
{
    QPainter* copyrightsPainter = new QPainter(this);
    copyrightsPainter->setPen(Qt::white);
    copyrightsPainter->setFont(QFont("Rockwell", 20));
    QString copyrights = "IUT LR - N. Godard, L. Stanislas, L. Lalida, B. Wang";

    copyrightsPainter->drawText(QPoint(screenWidth/8*2.5,(screenHeight/10)),copyrights);

    copyrightsPainter->end();
}

void Widget::displayNotesText()
{
    QPainter* notesLabel = new QPainter(this);
    notesLabel->setPen(Qt::white);
    notesLabel->setFont(QFont("Rockwell", 20));

    int indexPlacementNote=2;
    for(const Note &note : gammeActuelle->vecteurDeNotes)
    {
        std::string copyNote = note.nomNote;
        copyNote.pop_back();
        QString text = QString::fromStdString(copyNote);
        notesLabel->drawText(QPoint((screenWidth/14)*indexPlacementNote,(screenHeight/12)*7),text);
        indexPlacementNote++;
    }
    notesLabel->end();
}

void Widget::displayNotesImage()
{
    QPainter* notesPainter = new QPainter(this);
    QPen pen(Qt::white, 3, Qt::SolidLine);

    // Lignes Mesure
    notesPainter->setPen(pen);
    notesPainter->drawLine(10,(45*screenHeight)/144,screenWidth-10,(45*screenHeight)/144);
    notesPainter->drawLine(10,(52.5*screenHeight)/144,screenWidth-10,(52.5*screenHeight)/144);
    notesPainter->drawLine(10,(60*screenHeight)/144,screenWidth-10,(60*screenHeight)/144);
    notesPainter->drawLine(10,(67.5*screenHeight)/144,screenWidth-10,(67.5*screenHeight)/144);
    notesPainter->drawLine(10,(75*screenHeight)/144,screenWidth-10,(75*screenHeight)/144);

    // Clé de Sol
    QRect dimensionsCleSol((screenWidth/14)*0.35,(8*screenHeight)/30,screenWidth/14,((5*screenHeight)/9.30) - ((5*screenHeight)/24));
    QImage imageCleSol;
    imageCleSol.load(":/new/prefix1/images/resources/cleSol.png");

    notesPainter->drawImage(dimensionsCleSol,imageCleSol);

    // Impaire = barre au milieu de la note
    float heightLine = 0;
    if((noteActuelle->hauteur%2)==1) heightLine = noteActuelle->dimensionsNote.y() + (noteActuelle->dimensionsNote.height()/4)*3.4;
    else heightLine = noteActuelle->dimensionsNote.y() + noteActuelle->dimensionsNote.height();

    float xLine=0;
    if(noteActuelle->estBemol) xLine=noteActuelle->dimensionsNote.x() + (noteActuelle->dimensionsNote.width()/2.1);
    else xLine=noteActuelle->dimensionsNote.x();

    noteActuelle->draw(notesPainter);
    if(noteActuelle->hauteur > 11)
    {
        pen = QPen(Qt::black, 5, Qt::SolidLine);
        notesPainter->setPen(pen);
        notesPainter->drawLine(xLine, heightLine, noteActuelle->dimensionsNote.x()+noteActuelle->dimensionsNote.width(), heightLine);

        pen = QPen(Qt::white, 3, Qt::SolidLine);
        notesPainter->setPen(pen);
        notesPainter->drawLine(xLine, heightLine, noteActuelle->dimensionsNote.x()+noteActuelle->dimensionsNote.width(), heightLine);
    }
    notesPainter->end();
}

void Widget::displayBoutonsGamme()
{
    QPainter* boutonsPainter = new QPainter(this);
    boutonsPainter->setPen(Qt::white);
    boutonsPainter->setFont(QFont("Rockwell", 30));

    // TODO : Afficher une liste de boutons (en fonction du nombre de gamme)
    boutonsPainter->drawText(QPoint(screenWidth/8,(screenHeight/12)*11),QString("Blues"));
    if(gammeActuelle->nomGamme!="Blues") boutonsPainter->setPen(Qt::black);
    boutonsPainter->drawRect(screenWidth/8-2,(screenHeight/12)*11-38,114,50);
    boutonsPainter->setPen(Qt::white);

    boutonsPainter->drawText(QPoint((screenWidth/8)*3.5,(screenHeight/12)*11),QString("Mineure"));
    if(gammeActuelle->nomGamme!="Mineure") boutonsPainter->setPen(Qt::black);
    boutonsPainter->drawRect((screenWidth/8)*3.5-2,(screenHeight/12)*11-38,168,50);
    boutonsPainter->setPen(Qt::white);

    boutonsPainter->drawText(QPoint((screenWidth/8)*6,(screenHeight/12)*11),QString("Japonaise"));
    if(gammeActuelle->nomGamme!="Japonaise") boutonsPainter->setPen(Qt::black);
    boutonsPainter->drawRect((screenWidth/8)*6-2,(screenHeight/12)*11-38,195,50);
    boutonsPainter->setPen(Qt::white);

    boutonsPainter->end();
}

void Widget::displayBoutonPlay()

{
    QPainter* boutonPainter = new QPainter(this);

    QRect dimensionsBoutonPlay(screenWidth/2.25,(screenHeight/12)*7.5,screenWidth/15,screenWidth/15);
    QImage imageBoutonPlay;
    if(this->musicOn) imageBoutonPlay.load(":/new/prefix1/images/resources/pause.png");
    else imageBoutonPlay.load(":/new/prefix1/images/resources/play.png");
    boutonPainter->drawImage(dimensionsBoutonPlay,imageBoutonPlay);

    boutonPainter->end();
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    REVERB = ui->reverb->value();
    VOLUME = ui->volume->value();
    displayCopyrights();
    displayBoutonPlay();
    displayBoutonsGamme();
    displayNotesText();
    displayNotesImage();
}

bool valeurHorsLimite(double uneValeur)
{
    return (uneValeur < DISTANCE_MIN or uneValeur > DISTANCE_MAX);
}

bool Widget::deuxValeurSurTroisHorsLimite()
{
    bool deuxValeurSurTroisHorsLimite = false;

    if(valeurHorsLimite(valeur_precedente) && valeurHorsLimite(valeur_future))
    {
        deuxValeurSurTroisHorsLimite = true;
    }
    else if(valeurHorsLimite(valeur_precedente) && valeurHorsLimite(valeur_actuelle))
    {
        deuxValeurSurTroisHorsLimite = true;
    }
    else if(valeurHorsLimite(valeur_actuelle) && valeurHorsLimite(valeur_future))
    {
        deuxValeurSurTroisHorsLimite = true;
    }
    return deuxValeurSurTroisHorsLimite;
}

void Widget::restartALSAServer(){
    qDebug() << "Crash LIDAR";
    serialPort.Close();
    system("sudo chmod 777 /dev/serial0");
    SerialPort widgetSerialPort("/dev/serial0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort = widgetSerialPort;
    serialPort.SetTimeout(-1);
    serialPort.Open();
}

void Widget::calibrageDistanceMax(){
    /*
    int loopCount=0;
    bool distanceFound=false;
    qDebug() << "Calibrating DISTANCE_MAX";
    while(!distanceFound and loopCount<50){
        qDebug() << ".";
        if(loopCount == 10)
        {
            restartALSAServer();
        }
        serialPort.Read(lidarData);
        if(lidarData[0] == 0x59) //header 1
        {
            serialPort.Read(lidarData); //header 2
            if(lidarData[0] == 0x59)
            {
                serialPort.Read(distance_L);
                serialPort.Read(distance_H);
                serialPort.Read(strenght_L);
                serialPort.Read(strenght_H);
                serialPort.Read(temp_L);
                serialPort.Read(temp_H);
                serialPort.Read(checksum);

                if(((distance_H[0]*256 + distance_L[0])/100.0) <= 12.0)
                {
                    distance = (distance_H[0]*256 + distance_L[0])/100.0;
                    distanceFound=true;
                }
            }
        }
        loopCount++;
    }
    if(!distanceFound or valeurHorsLimite(distance)){
        qDebug() << "Failed to calibrate DISTANCE_MAX";
    }else{
        qDebug() << "DISTANCE_MAX calibrated at " << int(distance+1) << " meters";
        #ifdef DISTANCE_MAX
        #undef DISTANCE_MAX
        #endif
        #define DISTANCE_MAX int(distance+1)
    }
    */

}

void Widget::tryLoop(){
    if(!inLoop){
        loop();
    }
}

void Widget::loop()
{
    inLoop=true;
    loopCount = 0;
    qDebug() << "am in loop\n";
    while(distanceFound == false)
    {
        loopCount++;
        if(loopCount >= 10)
        {
            loopCount = 0;
            restartALSAServer();
        }
        serialPort.Read(lidarData);
        qDebug() << lidarData[0];
        if(lidarData[0] == 0x59) //header 1
        {
            serialPort.Read(lidarData); //header 2
            if(lidarData[0] == 0x59)
            {
                serialPort.Read(distance_L);
                serialPort.Read(distance_H);
                serialPort.Read(strenght_L);
                serialPort.Read(strenght_H);
                serialPort.Read(temp_L);
                serialPort.Read(temp_H);
                serialPort.Read(checksum);

                if(((distance_H[0]*256 + distance_L[0])/100.0) <= 12.0)
                {
                    distance = (distance_H[0]*256 + distance_L[0])/100.0;
                    distanceFound = true;
                }
                qDebug() << "Distance : " << distance;
            }
        }
    }
    if(!valeurHorsLimite(distance))
    {
        if(valeur_actuelle <= valeur_future + 1.0 && valeur_actuelle >= valeur_precedente - 1.0)
        {
            double distanceTotal=DISTANCE_MAX - DISTANCE_MIN;
            if(int((valeur_actuelle-1)*1000) / int(distanceTotal/12*1000) <= 12)
            {
                *noteActuelle=gammeActuelle->trouveNoteSelonDistance(distance);
                buzzer_set_freq(noteActuelle->frequenceNote);
                msleep(100);
            }
        }
    }
    else
    {
        if(deuxValeurSurTroisHorsLimite())
        {
            buzzer_set_freq(0);
            *noteActuelle = *noteParDefault;
        }
    }
    valeur_precedente = valeur_actuelle;
    valeur_actuelle = valeur_future;
    valeur_future = distance;
    distanceFound = false;
    // Appel la fonction paintEvent
    //repaint();
    inLoop=false;
}

void Widget::setValeurMin()
{
    //QString str = QString("%1").arg(ui->distanceMin->value());
    /*ui->valeurMin->setText(QString(ui->distanceMin->value()));
    if(ui->distanceMin->value()>ui->distanceMax->value())
        ui->distanceMax->setValue(ui->distanceMin->value()+10);
    DISTANCE_MAX = double(ui->distanceMax->value())/10;
    DISTANCE_MIN = double(ui->distanceMin->value())/10;*/
    DISTANCE_MIN = ui->distanceMinSpinBox->value();
}

void Widget::setValeurMax()
{
    //QString str = QString("%1").arg(ui->distanceMax->value());
    /*ui->valeurMax->setText(QString(ui->distanceMax->value()));
    if(ui->distanceMin->value()>ui->distanceMax->value())
        ui->distanceMin->setValue(ui->distanceMax->value()-10);
    DISTANCE_MAX = double(ui->distanceMax->value())/10;
    DISTANCE_MIN = double(ui->distanceMin->value())/10;*/
    DISTANCE_MAX = ui->distanceMaxSpinBox->value();
}
