#include "pipedream.h"
#include "graphicsitems.h"
#include <QGraphicsScene>

PipeDream::PipeDream(QWidget *parent)
    : QGraphicsView(parent)
{
    //绘图窗口设置
    setWindowTitle("PipeDream");
    setFixedSize(1440,900);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //绘图场景设置
    m_scene = new QGraphicsScene(this);                                      //场景演出元素管理器
    m_scene->setSceneRect(0,0,this->width(),this->height());
    m_sequencer = new Sequencer(m_scene,this);                              //场景调度器
    setScene(m_scene);

    //音乐播放器设置
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl("qrc:/music.aac"));
    //m_player->setSource(QUrl("qrc:/RingYourSong.mp3"));
    connect(m_player,&QMediaPlayer::positionChanged,this,&PipeDream::onMusicPositionChanged,Qt::QueuedConnection);
    connect(m_player,&QMediaPlayer::mediaStatusChanged,this,&PipeDream::onMediaStatusChanged,Qt::QueuedConnection);
    connect(m_sequencer,&Sequencer::started,m_player,&QMediaPlayer::play,Qt::QueuedConnection);

    m_sequencer->start();
}

PipeDream::~PipeDream()
{
    if(m_sequencer->isRunning())
    {
        m_sequencer->requestInterruption();
        m_sequencer->wait();
    }
    delete m_sequencer;
    delete m_scene;
}


void PipeDream::onMusicPositionChanged(qint64 position)
{
    int sec = position/1000;
    if(sec <= m_count)
    {
        return;
    }
    m_count = sec;
    qDebug() << m_count;
    m_sequencer->setCurrentTimestamp(m_count);
}

void PipeDream::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        qDebug() << "播放结束！";
        // 在这里处理播放结束后的逻辑
        m_count = 0;
        CustomButton *replay = new CustomButton(QPixmap(":/replay.png"));
        replay->setPos(50,50);
        connect(replay,&CustomButton::clicked,this,&PipeDream::onCustomButtonClicked);
        m_scene->addItem(replay);

    }
}

void PipeDream::onCustomButtonClicked()
{
    qDebug() << "点击";

    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<CustomButton*>(item)) {
            m_scene->removeItem(item);
            delete item;
        }
    }

    QMetaObject::invokeMethod(m_sequencer, "start", Qt::QueuedConnection);
}
