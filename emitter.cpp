#include "emitter.h"
#include <QRandomGenerator>

Emitter::Emitter(QGraphicsScene *scene, ParticleFactory factory, QObject *parent)
    : QObject(parent), m_scene(scene), m_factory(factory)
{
    min_x = 0;
    min_y = 0;
    max_x = m_scene->width();
    max_y = m_scene->height();
    max_v = 1.0;

    min_size = 1.0;
    max_size = 2.0;

    min_lifeTime = 5;
    max_lifeTime = 10;

    m_delay = 0;
    m_quantity = 1;
    m_interval = 0;
}

void Emitter::setEmitingParams(int delay, int quantity, int interval)
{
    m_delay = std::max(0,delay);
    m_quantity = std::max(1,quantity);
    m_interval = std::max(0,interval);
}


void Emitter::setPointRange(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
    min_x = minX;
    max_x = maxX;
    min_y = minY;
    max_y = maxY;
}

void Emitter::setVelocity(QVector2D direction, qreal minVelocity, qreal maxVelocity)
{
    v_direction = direction;
    min_v = minVelocity;
    max_v = maxVelocity;
}

void Emitter::setColor(const QColor &startColor, const QColor &endColor)
{
    m_startColor = startColor;
    m_endColor = endColor;
}

void Emitter::setSizeRange(qreal minSize, qreal maxSize)
{
    min_size = minSize;
    max_size = maxSize;
}

void Emitter::setLifeTimeRange(int minLife, int maxLife)
{
    min_lifeTime = minLife;
    max_lifeTime = maxLife;
}

ParticleParams Emitter::generateParams()
{
    ParticleParams params;

    //设置初始位置
    qreal point_x = min_x + QRandomGenerator::global()->bounded(max_x - min_x);
    qreal point_y = min_y + QRandomGenerator::global()->bounded(max_y - min_y);
    params.position = QPointF(point_x,point_y);

    //设置初始速度
    params.speed = min_v + QRandomGenerator::global()->bounded(max_v - min_v);
    params.direction = v_direction;
    params.velocity = params.speed * params.direction;

    //设置颜色
    if(m_startColor.isValid())
    {
        params.startColor = m_startColor;
        params.endColor = m_endColor;
    }
    else {
        // int r = QRandomGenerator::global()->bounded(256);
        // int g = QRandomGenerator::global()->bounded(256);
        // int b = QRandomGenerator::global()->bounded(256);
        // int a = QRandomGenerator::global()->bounded(256);
        // QColor startColor(r,g,b,a);
        // QColor endColor(r,g,b,0);
        // params.startColor = startColor;
        // params.endColor = endColor;
        int h = QRandomGenerator::global()->bounded(360);
        int s = QRandomGenerator::global()->bounded(100);
        int l = QRandomGenerator::global()->bounded(100);
        params.startColor = QColor::fromHsl(h,100,100);
        params.endColor = QColor::fromHsl(h,s,l,0);
    }

    //设置粒子大小
    params.size = min_size + QRandomGenerator::global()->bounded(max_size - min_size);

    //设置粒子寿命
    params.lifeTime = min_lifeTime + QRandomGenerator::global()->bounded(max_lifeTime - min_lifeTime);

    return params;
}

void Emitter::emitParticle() {
    if(m_delay > 0)
    {
        m_delay--;
        return;
    }

    if(m_count < m_interval)
    {
        m_count++;
        return;
    }
    m_count = 0;
    for (int i = 0; i < m_quantity; ++i) {
        auto params = generateParams();
        Particle* p = m_factory(params);
        if(FlameParticle *flame = dynamic_cast<FlameParticle*>(p))
        {
            flame->setScene(m_scene);
            m_scene->addItem(flame);
        }
        else {
            m_scene->addItem(p);
        }
    }
}


