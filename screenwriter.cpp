#include "screenwriter.h"
#include "emitter.h"
#include "affector.h"
#include "graphicsitems.h"

#include <QTimer>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

EnframedScenery::~EnframedScenery()
{
}

void EnframedScenery::precondition()
{

}

void EnframedScenery::actOut()
{
    if(!m_shouldStop)
    {
        static int count = 0;
        count++;
        if(count > 2)
        {
            createFalling();
            count = 0;
        }
    }
    else
    {
        bool containsFallingItem = false;
        QList<QGraphicsItem*> items = m_scene->items();
        for (QGraphicsItem *item : items) {
            if (dynamic_cast<FallingItem*>(item)) {
                containsFallingItem = true;
                break;
            }
        }
        if(!containsFallingItem)
        {
            m_exeunted = true;
            m_showing = false;
        }
    }
}

void EnframedScenery::createFalling()
{
    int index = QRandomGenerator::global()->bounded(dynamicPixmaps.count());
    FallingItem *item = new FallingItem(dynamicPixmaps.at(index));
    int startX = QRandomGenerator::global()->bounded(m_scene->width());
    item->setPos(startX,-50);
    m_scene->addItem(item);
    // 创建动画组合
    QPropertyAnimation* posAnim = new QPropertyAnimation(item, "pos");
    QPropertyAnimation* rotateAnim = new QPropertyAnimation(item, "rotation");
    QPropertyAnimation* opacityAnim = new QPropertyAnimation(item, "opacity");

    // 动画持续时间（3-5秒）
    int duration = 3000 + QRandomGenerator::global()->bounded(2000);

    // 位置动画（正弦曲线路径）
    posAnim->setDuration(duration);
    posAnim->setStartValue(QPointF(startX, -50));
    posAnim->setEndValue(QPointF(
        startX + QRandomGenerator::global()->bounded(400) - 200, // 随机水平偏移
        m_scene->height() + 50));
    posAnim->setEasingCurve(QEasingCurve::InQuad);

    // 旋转动画
    rotateAnim->setDuration(duration);
    rotateAnim->setStartValue(0);
    rotateAnim->setEndValue(QRandomGenerator::global()->bounded(360) + 360);

    // 透明度动画
    opacityAnim->setDuration(duration);
    opacityAnim->setStartValue(1.0);
    opacityAnim->setEndValue(0.0);

    // 启动动画
    QParallelAnimationGroup* group = new QParallelAnimationGroup(item);
    group->addAnimation(posAnim);
    group->addAnimation(rotateAnim);
    group->addAnimation(opacityAnim);
    group->start(QAbstractAnimation::DeleteWhenStopped);

    // 动画完成时删除花瓣
    connect(group, &QParallelAnimationGroup::finished,m_scene,[=]() {
        m_scene->removeItem(item);
        delete item;
    });
}


ParticleSystem::~ParticleSystem()
{
    if(!emitters.isEmpty())
    {
        for (int i = 0; i < emitters.count(); ++i) {
            delete emitters.at(i);
        }
        emitters.clear();
    }
    if(!affectors.isEmpty())
    {
        for (int i = 0; i < affectors.count(); ++i) {
            delete affectors.at(i);
        }
    }
}

void ParticleSystem::precondition()
{

}

void ParticleSystem::actOut()
{
    if(!m_shouldStop)
    {
        emitParticles();
    }
    bool containsParticle = false;
    QList<QGraphicsItem*> items = m_scene->items();
    for(QGraphicsItem *item : items) {
        if (auto p = dynamic_cast<Particle*>(item)) {
            containsParticle = true;
            // 应用干扰器
            foreach(auto affector, affectors) {
                affector->affect(p);
            }

            // 更新粒子
            p->updatePaint();

            // 移除失效粒子
            if (p->isDead()) {
                m_scene->removeItem(p);
                delete p;
            }
        }
        else {

        }
    }
    if(m_shouldStop)
    {
        if(!containsParticle)
        {
            m_exeunted = true;
            m_showing = false;
        }
    }
}

void ParticleSystem::emitParticles()
{
    foreach (auto emitter, emitters)
    {
        emitter->emitParticle();
    }
}

CustomScenery::~CustomScenery()
{

}

void CustomScenery::precondition()
{
    addOrchid();
    addPipe();
    addFireWork();
}

void CustomScenery::actOut()
{
    if(!m_shouldStop)
    {
        precondition();
    }
    bool containsParticle = false;
    QList<QGraphicsItem*> items = m_scene->items();
    for(QGraphicsItem *item : items) {
        if (auto p = dynamic_cast<Particle*>(item)) {
            containsParticle = true;

            // 更新粒子
            p->updatePaint();

            // 移除失效粒子
            if (p->isDead()) {
                m_scene->removeItem(p);
                delete p;
            }
        }
        if(OrchidItem *orchid = dynamic_cast<OrchidItem*>(item))
        {
            containsParticle = true;
        }
    }
    if(m_shouldStop)
    {
        if(!containsParticle)
        {
            m_exeunted = true;
            m_showing = false;
        }
    }
}

void CustomScenery::addOrchid()
{
    static int count = 0;
    count++;
    if(count>12)
    {
        count = 0;

        //绘图时间
        int delay = QRandomGenerator::global()->bounded(5,40);
        int execTime = QRandomGenerator::global()->bounded(32,40);
        int quitTime = QRandomGenerator::global()->bounded(20,35);

        //初始位置
        float x = QRandomGenerator::global()->bounded(-60,60) + m_scene->sceneRect().width()/2;
        float y = QRandomGenerator::global()->bounded(100,200) + m_scene->sceneRect().height();

        //初速度
        float vy = QRandomGenerator::global()->bounded(30.0) - 110.0;
        float vx = QRandomGenerator::global()->bounded(40.0) - 20.0;

        //渐变色
        QColor color1(0,128+QRandomGenerator::global()->bounded(128),128+QRandomGenerator::global()->bounded(128),255);
        QColor color2(128,128+QRandomGenerator::global()->bounded(128),128+QRandomGenerator::global()->bounded(128),255);

        //轨迹宽度
        float width1 = 10.0 + QRandomGenerator::global()->bounded(10.0);
        float width2 = 0.1;

        //轨迹长度
        int length = QRandomGenerator::global()->bounded(100,150);

        OrchidItem *orchid = new OrchidItem(QVector2D(x,y),QVector2D(vx,vy),m_scene);
        orchid->setOrchid(length,color1,color2,width1,width2);
        orchid->setPainting(delay,execTime,quitTime);
        orchid->start();
    }
}

void CustomScenery::addPipe()
{
    static int count = 0;
    count++;
    if(count>8)
    {
        count = 0;

        float x = QRandomGenerator::global()->bounded(m_scene->sceneRect().width());
        float y = QRandomGenerator::global()->bounded(100) + m_scene->sceneRect().height();
        float size1 = QRandomGenerator::global()->bounded(20) + 20;

        int r,g,b,a;
        r = QRandomGenerator::global()->bounded(256);
        g = QRandomGenerator::global()->bounded(256);
        b = QRandomGenerator::global()->bounded(256);
        a = QRandomGenerator::global()->bounded(128)+128;

        //
        qreal radian = (M_PI / 180) * (-120 + QRandomGenerator::global()->bounded(60));
        qreal length = 2.0 + QRandomGenerator::global()->bounded(3.0);
        QVector2D offset(qCos(radian),qSin(radian));
        QPointF point(x,y);

        ParticleParams params;
        params.position = point;                    //当前位置
        params.direction = offset.normalized();     //随机方向
        params.speed = length;
        params.velocity = params.speed * params.direction;
        params.startColor = QColor(r,g,b,a);                        //当前颜色
        params.endColor = QColor(r,g,b,0);                          //结束颜色
        params.size = size1;
        params.lifeTime = 100 + QRandomGenerator::global()->bounded(100);
        Particle* p = new Particle(params);
        p->setZValue(-1);
        m_scene->addItem(p);
    }
}

void CustomScenery::addFireWork()
{

}
