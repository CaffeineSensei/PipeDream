#include "sequencer.h"
#include "screenwriter.h"
#include "graphicsitems.h"
#include "emitter.h"
#include "affector.h"

#include <QDebug>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QUrl>
#include <QDesktopServices>

Sequencer::Sequencer(QGraphicsScene *scene, QObject *parent)
    : QThread(parent)
    , m_scene(scene)
{
    connect(this,&Sequencer::backgroundLoading,this,&Sequencer::onBackgroundLoading);
    connect(this,&Sequencer::backgroundChanged,this,&Sequencer::onBackgroundChanged);

    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&Sequencer::onTimerTimeout);
    m_timer->start(20);
}

Sequencer::~Sequencer()
{
    if(!m_screenwriters.isEmpty())
    {
        for (int i = 0; i < m_screenwriters.count(); ++i) {
            delete m_screenwriters.at(i);
        }
        m_screenwriters.clear();
    }
}

void Sequencer::setCurrentTimestamp(int timestamp)
{
    m_timestamp = timestamp;
}

void Sequencer::run()
{
    m_timestamp = 0;
    emit backgroundLoading();

    addEvent(3,std::bind(&Sequencer::backgroundFadein,this));       //背景淡入
    addEvent(5,std::bind(&Sequencer::sakura,this));                 //花瓣飘落
    addEvent(50,std::bind(&Sequencer::endOfCurrentScene,this));      //花瓣飘落结束
    addEvent(55,std::bind(&Sequencer::sceneTransition2,this));      //切换夜景(65)
    addEvent(60,std::bind(&Sequencer::firefly,this));               //萤火虫(70)
    addEvent(65,std::bind(&Sequencer::backgroundFadeout,this));     //背景淡出(75)
    addEvent(85,std::bind(&Sequencer::endOfCurrentScene,this));      //萤火虫结束(85)
    addEvent(90,std::bind(&Sequencer::spiralParticle,this));        //粒子环绕(90)
    addEvent(110,std::bind(&Sequencer::endOfCurrentScene,this));     //粒子环绕结束(110)
    addEvent(115,std::bind(&Sequencer::fireworks,this));            //烟花(120)
    addEvent(180,std::bind(&Sequencer::endOfCurrentScene,this));     //烟花结束(180)
    addEvent(182,std::bind(&Sequencer::sceneTransition3,this));     //切换白景(190)
    addEvent(186,std::bind(&Sequencer::orchidBubbleFireworks,this));//兰花(185)


    qDebug() << "任务已开始";

    auto eventIter = m_events.begin();
    while (!isInterruptionRequested() && (eventIter != m_events.end())) {
        if (m_timestamp >= eventIter->timestamp) {
            eventIter->callback();
            eventIter = m_events.erase(eventIter);
        }
        QThread::msleep(10);
    }

    qDebug() << "任务已结束";
}

void Sequencer::onTimerTimeout()
{
    if(!m_screenwriters.isEmpty())
    {
        if(m_screenwriters.first()->isShowing())
        {
            m_screenwriters.first()->actOut();         //演出
            m_scene->update();
        }
        if(m_screenwriters.first()->isExecuted())      //演出结束
        {
            delete m_screenwriters.first();
            m_screenwriters.removeFirst();
            qDebug() << "下一个节目";
        }
    }
}

void Sequencer::onBackgroundLoading()
{
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<BackgroundItem*>(item)) {
            m_scene->removeItem(item);
            delete item;
        }
    }
    endOfCurrentScene();
    BackgroundItem *bg =new BackgroundItem(QPixmap(":/background.png"));
    bg->setPos(m_scene->sceneRect().center());
    bg->setZValue(-1);
    bg->setOpacity(0.00);
    m_scene->addItem(bg);
}

void Sequencer::onBackgroundChanged(qreal start, qreal end, int duration)
{
    foreach(auto item, m_scene->items()) {
        if (auto bg = dynamic_cast<BackgroundItem*>(item)) {
            QPropertyAnimation* anim = new QPropertyAnimation(bg, "opacity");
            anim->setDuration(duration);
            anim->setStartValue(start);
            anim->setEndValue(end);
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
}

void Sequencer::addEvent(int timestamp, std::function<void ()> callback)
{
    m_events.push_back({timestamp, callback});
    // 按时间戳排序
    // std::sort(events.begin(), events.end(), [](const TimelineEvent& a, const TimelineEvent& b) {
    //     return a.timestamp < b.timestamp;
    // });
}

void Sequencer::backgroundFadein()
{
    emit backgroundChanged(0.0,1.0,2000);
}

void Sequencer::backgroundFadeout()
{
    emit backgroundChanged(1.0,0.0,2000);
}

void Sequencer::sakura()
{
    qDebug() << "开始绘制樱花";
    EnframedScenery *sakuraScene = new EnframedScenery(m_scene);

    QPixmap petals(":/petal.png");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            sakuraScene->addDynamicPixmap(petals.copy(j*50,i*50,50,50));
        }
    }
    sakuraScene->start();
    m_screenwriters.append(sakuraScene);
    qDebug() << sakuraScene;
}

void Sequencer::firefly()
{
    qDebug() << "开始绘制萤火虫";
    ParticleSystem *fireflyParticles = new ParticleSystem(m_scene);     //创建粒子系统
    auto fireflyFactory = [](const ParticleParams& params) {
        LampParticle *firefly = new LampParticle(params);               //闪烁粒子
        firefly->setFlickerFrequency(5);                                //闪烁频率
        return firefly;
    };

    Emitter *fireflyEmitter = new Emitter(m_scene,fireflyFactory);      //创建发射器
    fireflyEmitter->setPointRange(0,m_scene->width(),m_scene->height(),m_scene->height() + 20);
    fireflyEmitter->setVelocity(QVector2D(0,-1),2.0,5.0);
    fireflyEmitter->setColor(QColor(200,240,50,255),QColor(200,240,50,0));
    fireflyEmitter->setSizeRange(10.0,15.0);
    fireflyEmitter->setLifeTimeRange(150,200);

    fireflyParticles->addEmitter(fireflyEmitter);                                   //添加发射器
    fireflyParticles->addAffector(new TurbulenceAffector(m_scene->sceneRect()));    //添加随机扰动

    fireflyParticles->start();
    m_screenwriters.append(fireflyParticles);
    qDebug() << fireflyParticles;
}

void Sequencer::spiralParticle()
{
    qDebug() << "开始绘制粒子环绕";
    ParticleSystem *spiral = new ParticleSystem(m_scene);

    //环绕粒子
    auto spiralParticles = [](const ParticleParams& params){
        LampParticle *particle = new LampParticle(params);
        particle->setVibration(500,250,0.01);
        particle->setFlickerFrequency(10);
        return particle;
    };

    //环绕粒子发射器
    Emitter *spiralParticlesEmitter = new Emitter(m_scene,spiralParticles);
    spiralParticlesEmitter->setPointRange(m_scene->width()/2 - 100,m_scene->width()/2 + 100, m_scene->height()-300, m_scene->height()-250); //发射范围
    spiralParticlesEmitter->setEmitingParams(0,3,0);
    spiralParticlesEmitter->setVelocity(QVector2D(0,-1.0),0.0,0.1);                 //速度方向及范围
    spiralParticlesEmitter->setSizeRange(5.0,8.0);                                //粒子大小
    spiralParticlesEmitter->setLifeTimeRange(400,450);                              //粒子寿命
    spiralParticlesEmitter->setColor(QColor(38,191,221),QColor(38,191,221,0));          //粒子颜色

    //添加粒子发射器
    spiral->addEmitter(spiralParticlesEmitter);

    //添加干扰器
    spiral->addAffector(new TurbulenceAffector(QRectF(0,0,m_scene->width(),m_scene->height()/2)));
    spiral->addAffector(new ForceAffector(QRectF(m_scene->width()/2 - 100,m_scene->height()-350,200,100),QVector2D(0,-0.3)));
    spiral->addAffector((new AmplitudeAffector(QRectF(m_scene->sceneRect()),0.007)));
    //开始
    spiral->start();

    //加人节目单
    m_screenwriters.append(spiral);
    qDebug() << spiral;
}

void Sequencer::fireworks()
{
    qDebug() << "开始绘制烟花";
    ParticleSystem *firework = new ParticleSystem(m_scene);

    //创建粒子工厂
    auto fireworksParticles = [](const ParticleParams& params){
        FireworkParticle *p = new FireworkParticle(params);
        p->setExplodeParams(true,true);
        p->setFlickerFrequency(10);
        return p;
    };

    //创建粒子发射器
    Emitter * fireworksParticlesEmitter = new Emitter(m_scene,fireworksParticles);
    fireworksParticlesEmitter->setPointRange(50,m_scene->width()-50,m_scene->height()-100,m_scene->height());
    fireworksParticlesEmitter->setEmitingParams(0,1,50);
    fireworksParticlesEmitter->setVelocity(QVector2D(0,-1.0),4.0,6.0);
    fireworksParticlesEmitter->setSizeRange(15.0,20.0);
    fireworksParticlesEmitter->setLifeTimeRange(100,120);

    firework->addEmitter(fireworksParticlesEmitter);

    firework->addAffector(new TurbulenceAffector(m_scene->sceneRect()));

    firework->start();
    m_screenwriters.append(firework);
    qDebug() << firework;
}

void Sequencer::orchidBubbleFireworks()
{
    qDebug() << "开始绘制兰花";
    CustomScenery *orchid = new CustomScenery(m_scene);

    orchid->start();
    m_screenwriters.append(orchid);
    qDebug() << orchid;

    QString filePath = "./readme.txt";
    QFileInfo fileInfo(filePath);

    if(!fileInfo.exists())
    {
        qDebug() << "文件不存在:" << fileInfo.absoluteFilePath();
        return;
    }

    QUrl fileUrl = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    if(!QDesktopServices::openUrl(fileUrl))
    {
        qDebug() << "无法打开文件,请检查默认应用程序设置。";
    }
}

void Sequencer::endOfCurrentScene()
{
    if(!m_screenwriters.isEmpty())
    {
        m_screenwriters.first()->shouldStop();
    }
}

void Sequencer::sceneTransition2()
{
    setSceneColorGrayGradually(255,0);
}

void Sequencer::sceneTransition3()
{
    setSceneColorGrayGradually(0,255);
}

void Sequencer::setSceneColorGrayGradually(int start, int end)
{
    int total = std::abs(start - end);
    for (int var = 0; var < total + 1; ++var) {
        int Gray = start + (end - start) * var / total;
        m_scene->setBackgroundBrush(QColor(Gray,Gray,Gray));
        QThread::msleep(10);
    }
}
