#ifndef EMITTER_H
#define EMITTER_H

#include <QGraphicsScene>
#include <QTimer>
#include "graphicsitems.h"

class Emitter : public QObject
{
    Q_OBJECT
public:
    using ParticleFactory = std::function<Particle*(const ParticleParams&)>;
    explicit Emitter(QGraphicsScene* scene,ParticleFactory factory, QObject* parent = nullptr);

    void setEmitingParams(int delay,int quantity,int interval); //发射参数，(发射延迟时间，一次发射量，发射间隔时间)
    void setPointRange(qreal minX,qreal maxX,qreal minY,qreal maxY);
    void setVelocity(QVector2D direction,qreal minVelocity,qreal maxVelocity);
    void setColor(const QColor &startColor,const QColor &endColor);
    void setSizeRange(qreal minSize,qreal maxSize);
    void setLifeTimeRange(int minLife,int maxLife);

    void emitParticle();

protected:
    virtual ParticleParams generateParams();



protected:
    qreal min_x;                //x坐标最小值
    qreal max_x;                //x坐标最大值
    qreal min_y;                //y坐标最小值
    qreal max_y;                //y坐标最大值
    qreal min_v;                //速度最小值
    qreal max_v;                //速度最大值
    QVector2D v_direction;      //运动方向

    QColor m_startColor;        //粒子初始颜色
    QColor m_endColor;          //粒子结束颜色

    qreal min_size;             //最小尺寸
    qreal max_size;             //最大尺寸

    int min_lifeTime;           //最小生命周期
    int max_lifeTime;           //最大生命周期

    QGraphicsScene* m_scene;
    ParticleFactory m_factory;

    int m_delay;
    int m_quantity;
    int m_interval;

    int m_count = 0;
};





#endif // EMITTER_H
