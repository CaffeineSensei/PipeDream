#include "affector.h"
#include "graphicsitems.h"
#include <QRandomGenerator>

//粒子力场干扰(施加固定的力值对粒子的速度进行干扰)
void ForceAffector::affect(QGraphicsObject *item)
{
    if (Particle *particle = dynamic_cast<Particle*>(item))
    {
        if(!isInside(particle->pos()))
        {
            return;
        }
        auto params = particle->params();
        params.velocity += m_force;
        particle->setParams(params);
    }
}

//粒子随机扰动
void TurbulenceAffector::affect(QGraphicsObject *item)
{
    if (Particle *particle = dynamic_cast<Particle*>(item))
    {
        if(!isInside(particle->pos()))
        {
            return;
        }
        auto params = particle->params();
        params.velocity += QVector2D(-0.1 + QRandomGenerator::global()->bounded(0.2), -0.1 + QRandomGenerator::global()->bounded(0.2));
        particle->setParams(params);
    }
}




// void HeartShapeAffector::affect(QGraphicsObject *item)
// {
//     cleanupInvalidParticles();

//     if (Particle* particle = dynamic_cast<Particle*>(item))
//     {
//         const bool inside = isInsideHeart(particle->pos());

//         if (inside && !m_originalOpacities.contains(particle))
//         {
//             // 进入区域：记录原始透明度并设为透明
//             m_originalOpacities[particle] = particle->opacity();
//             particle->setOpacity(0.0);
//         }
//         else if (!inside && m_originalOpacities.contains(particle))
//         {
//             // 离开区域：恢复透明度
//             particle->setOpacity(m_originalOpacities[particle]);
//             m_originalOpacities.remove(particle);
//         }
//     }
// }

// void HeartShapeAffector::cleanupInvalidParticles()
// {
//     QList<Particle*> toRemove;
//     for (auto it = m_originalOpacities.begin(); it != m_originalOpacities.end(); ++it)
//     {
//         if (!it.key() || !it.key()->scene())
//             toRemove.append(it.key());
//     }
//     for (Particle* p : toRemove)
//         m_originalOpacities.remove(p);
// }

// bool HeartShapeAffector::isInsideHeart(const QPointF &pos) const
// {
//     // 转换为标准化坐标
//     const qreal x = (pos.x() - m_center.x()) / m_scale;
//     const qreal y = (pos.y() - m_center.y()) / m_scale;

//     // 心形隐式方程：(x² + y² - 1)³ - x²y³ ≤ 0
//     const qreal x2 = x * x;
//     const qreal y2 = y * y;
//     const qreal temp = x2 + y2 - 1.0;
//     return (temp * temp * temp - x2 * y2 * y) <= 0;
// }

void HeartRepelAffector::affect(QGraphicsObject *item)
{
    if (Particle* particle = dynamic_cast<Particle*>(item))
    {
        const QPointF pos = particle->pos();
        const qreal distance = distanceToHeartEdge(pos);

        // 在影响范围内施加排斥力
        if (distance < m_repelRange)
        {
            const QVector2D repelDir = calculateRepelDirection(pos);
            const qreal strength = (m_repelRange - distance) / m_repelRange;

            auto params = particle->params();
            params.velocity += repelDir * strength * m_repelForce;
            particle->setParams(params);
        }
    }
}

qreal HeartRepelAffector::distanceToHeartEdge(const QPointF &pos) const
{
    const qreal x = (pos.x() - m_center.x()) / m_scale;
    const qreal y = (pos.y() - m_center.y()) / m_scale;

    // 心形隐式方程：(x² + y² - 1)³ - x²y³ = 0
    const qreal x2 = x * x;
    const qreal y2 = y * y;
    const qreal temp = x2 + y2 - 1.0;
    const qreal heartValue = temp * temp * temp - x2 * y2 * y;

    // 近似距离（符号表示内外）
    return heartValue / (x2 + y2 + 1e-6);
}

QVector2D HeartRepelAffector::calculateRepelDirection(const QPointF &pos) const
{
    const qreal epsilon = 1e-3;
    const qreal dx = distanceToHeartEdge(pos + QPointF(epsilon, 0)) -
                     distanceToHeartEdge(pos - QPointF(epsilon, 0));
    const qreal dy = distanceToHeartEdge(pos + QPointF(0, epsilon)) -
                     distanceToHeartEdge(pos - QPointF(0, epsilon));

    QVector2D gradient(dx, dy);
    return gradient / (gradient.toPointF().manhattanLength() + 1e-6);
}

void AmplitudeAffector::affect(QGraphicsObject *item)
{
    if (Particle *particle = dynamic_cast<Particle*>(item))
    {
        if(!isInside(particle->pos()))
        {
            return;
        }
        particle->m_orthometricAmplitude = (1-m_decayRate) * particle->m_orthometricAmplitude;
        particle->m_parallelAmplitude = (1-m_decayRate) * particle->m_parallelAmplitude;
    }
}
