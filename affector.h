#ifndef AFFECTOR_H
#define AFFECTOR_H

#include <QGraphicsObject>
#include <QVector2D>
#include <QPointF>

class Particle;

//干扰器基类
class Affector : public QObject
{
public:
    Affector(const QRectF &range) : m_range(range){}
    virtual void affect(QGraphicsObject *item) = 0;
protected:
    bool isInside(const QPointF &p){return m_range.contains(p);}
    QRectF m_range;
};

//粒子力场干扰器(重力、风力等)
class ForceAffector : public Affector
{
public:
    ForceAffector(const QRectF &range,const QVector2D &force): Affector(range),m_force(force){}
    void affect(QGraphicsObject *item) override;
private:
    QVector2D m_force;
};

//粒子随机扰动干扰器
class TurbulenceAffector : public Affector
{
public:
    using Affector::Affector;
    void affect(QGraphicsObject *item) override;
};

//粒子振幅衰减干扰器
class AmplitudeAffector : public Affector
{
public:
    AmplitudeAffector(const QRectF &range,qreal decayRate = 0.01) :Affector(range),m_decayRate(decayRate){}
    void affect(QGraphicsObject *item) override;
private:
    qreal m_decayRate;
};
// // 心形区域干扰器
// class HeartShapeAffector : public Affector
// {
// public:
//     HeartShapeAffector(QPointF center, qreal scale)
//         : m_center(center), m_scale(scale) {}

//     void affect(QGraphicsObject* item) override;

// private:
//     // 清理无效粒子指针
//     void cleanupInvalidParticles();

//     // 判断坐标是否在心形区域内
//     bool isInsideHeart(const QPointF& pos) const;

//     QPointF m_center;    // 心形中心坐标
//     qreal m_scale;       // 缩放系数
//     QHash<Particle*, qreal> m_originalOpacities; // 保存原始透明度
// };

// //飘落干扰器(用于树叶、花瓣、雪的飘落)
// class FloatDownAffector : public Affector
// {
// public:
//     void affect(QGraphicsObject *item) override;
// };


class HeartRepelAffector : public Affector
{
public:
    HeartRepelAffector(const QRectF &range ,QPointF center, qreal scale, qreal repelForce = 1.0)
        :Affector(range), m_center(center), m_scale(scale), m_repelForce(repelForce) {}

    void affect(QGraphicsObject* item) override;

private:
    // 计算点到心形边缘的最近距离
    qreal distanceToHeartEdge(const QPointF& pos) const;

    // 计算排斥方向（指向最近的心形边缘）
    QVector2D calculateRepelDirection(const QPointF& pos) const;

    QPointF m_center;    // 心形中心
    qreal m_scale;       // 缩放系数
    qreal m_repelForce;  // 排斥力强度
    qreal m_repelRange = 50.0; // 排斥作用范围
};


#endif // AFFECTOR_H
