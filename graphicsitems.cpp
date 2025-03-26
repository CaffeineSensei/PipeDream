#include "graphicsitems.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QGraphicsSceneMouseEvent>


QColor GraphicsItem::gradientColor(const QColor &color1, const QColor &color2, int step, int n)
{
    int r1,g1,b1;
    int r2,g2,b2;
    color1.getRgb(&r1,&g1,&b1);
    color2.getRgb(&r2,&g2,&b2);
    int R = r1  + (r2 - r1) * n / step;
    int G = g1  + (g2 - g1) * n / step;
    int B = b1  + (b2 - b1) * n / step;
    return QColor(R,G,B);
}

qreal GraphicsItem::gradientWidth(float w1, float w2, int step, int n)
{
    return w1  + (w2 - w1) * n / step;
}

QPointF GraphicsItem::calculateParabolaTrack(QPointF startPoint, int vx, int vy, float dt, int num)
{
    QPointF point;                                                                                  // ----------------->x
    float t = 0.0;                                                                                  // | ↙ dirAngle
    if(dt > 0 && num > 0)                                                                           // |
    {                                                                                               // |
        t = dt * num;                                                                               // |
    }                                                                                               // ↓y
    point.setX(startPoint.x() + vx*t);
    point.setY(startPoint.y() + vy * t + 0.5 * Gravity * t * t);
    return point;
}

//------------------------------------------------------------------------------------------------

Particle::Particle(const ParticleParams &params, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_params(params)
    , m_age(0)
    , m_delay(0)
    , m_orthometricAmplitude(0)
    , m_parallelAmplitude(0)
    , m_frequency(0)
    , m_phase(QRandomGenerator::global()->bounded(2*M_PI))
{
    setPos(params.position);
    // 设置素材图片
    QPixmap image(":/ball.png");
    m_pixmap = image.scaled(params.size,params.size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void Particle::updatePaint()
{
    QVector2D direction = m_params.direction.normalized();
    QVector2D normal(direction.y(),-direction.x()); //粒子运动方向的法向量
    QVector2D displacement = normal * m_orthometricAmplitude * qCos(m_frequency * m_age + m_phase);     //计算粒子正交方向振动的位移向量
    QVector2D displacement2 = direction * m_parallelAmplitude *qSin(m_frequency * m_age + m_phase);     //计算粒子平行方向振动的位移向量
    QPointF newPos = m_params.position + displacement.toPointF() + displacement2.toPointF();//振动中心点加上位移向量
    setPos(newPos);
    if(m_delay <= 0)
    {
        m_age++;
    }
    else {
        m_delay--;
    }
    m_params.position  += m_params.velocity.toPointF();
}

void Particle::setVibration(qreal orthometricAmplitude, qreal parallelAmplitude, qreal frequency, bool randomPhase, qreal phase)
{
    m_orthometricAmplitude = orthometricAmplitude;
    m_parallelAmplitude = parallelAmplitude;
    m_frequency = frequency;
    if(!randomPhase)
    {
        m_phase = phase;
    }
}

//重写绘图范围函数
QRectF Particle::boundingRect() const
{
    return QRectF(-m_params.size/2, -m_params.size/2, m_params.size, m_params.size);
}

//重写绘图过程
void Particle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_delay > 0)
    {
        return;
    }
    QColor color = interpolateColor();

    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(boundingRect());
    //painter->setCompositionMode(QPainter::CompositionMode_Overlay);
    //painter->drawPixmap(boundingRect(), m_pixmap, m_pixmap.rect());
}

//渐变颜色计算
QColor Particle::interpolateColor() const
{
    qreal ratio = static_cast<qreal>(m_age) / m_params.lifeTime;
    return QColor::fromRgbF(
        m_params.startColor.redF()   + (m_params.endColor.redF()   - m_params.startColor.redF())   * ratio,
        m_params.startColor.greenF() + (m_params.endColor.greenF() - m_params.startColor.greenF()) * ratio,
        m_params.startColor.blueF()  + (m_params.endColor.blueF()  - m_params.startColor.blueF())  * ratio,
        m_params.startColor.alphaF() + (m_params.endColor.alphaF() - m_params.startColor.alphaF()) * ratio
        );
}


//------------------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------

//闪烁粒子
LampParticle::LampParticle(const ParticleParams &params, QGraphicsItem *parent)
    : Particle(params,parent)
    , m_flickerFrequency(2)
    , m_flickerPhase(QRandomGenerator::global()->bounded(M_PI * 2))
    , m_flickerProgress(0)
    , m_pulseIntensity(0.5 + QRandomGenerator::global()->bounded(0.5)) // 随机脉冲强度
{

}

void LampParticle::updatePaint()
{
    // 计算相位增量（基于频率和帧时间）
    const qreal dt = 0.016; // 假设60FPS，每帧约16ms
    m_flickerPhase += 2 * M_PI * m_flickerFrequency * dt;

    // 限制相位范围
    if (m_flickerPhase > 2 * M_PI) {
        m_flickerPhase -= 2 * M_PI;
    }

    // 计算正弦波值 [-1, 1]，转换为进度 [0,1]
    m_flickerProgress = (qSin(m_flickerPhase) + 1.0) / 2.0;

    Particle::updatePaint();
}

void LampParticle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QColor color = interpolateColor();
    // 计算闪烁颜色
    QColor flickerColor = color.lighter(100 + m_flickerProgress * 50); // 亮度变化
    flickerColor.setAlphaF(color.alphaF() * (0.5 + m_flickerProgress * 0.5)); // 透明度变

    painter->setPen(Qt::NoPen);
    painter->setBrush(flickerColor);
    painter->drawEllipse(boundingRect());
    painter->setCompositionMode(QPainter::CompositionMode_Overlay);
    painter->drawPixmap(boundingRect(), m_pixmap, m_pixmap.rect());

}


void FlameParticle::updatePaint()
{
    //执行父类的更新函数
    LampParticle::updatePaint();

    //执行溅射
    if(m_splash)
    {
        splashing();
    }

    //执行爆炸
    if(m_explode && (m_age >= m_params.lifeTime))
    {
        exploding();
    }

}

void FlameParticle::splashing()
{
    ParticleParams params;
    params.position = this->pos();              //当前位置
    params.direction = - m_params.direction;    //反方向
    params.speed = QRandomGenerator::global()->bounded(0.2);    //速度随机0~0.5;
    params.velocity = params.speed * params.direction;
    params.startColor = m_params.startColor.lighter();          //当前颜色
    params.endColor = m_params.startColor;                      //结束颜色
    params.size = QRandomGenerator::global()->bounded(0.2 * m_params.size);
    params.lifeTime = m_params.lifeTime - m_age;
    LampParticle* p = new LampParticle(params);
    p->setVibration(5,5,0.01);
    p->setFlickerFrequency(20);
    m_scene->addItem(p);
}

void FlameParticle::exploding()
{
    qreal explodingRadius = 20.0 + QRandomGenerator::global()->bounded(30.0);
    for (int i = 0; i < 30; ++i) {
        qreal radian = QRandomGenerator::global()->bounded(2 * M_PI);
        qreal length = QRandomGenerator::global()->bounded(explodingRadius);
        QVector2D offset(length * qCos(radian),length * qSin(radian));
        QPointF point = this->pos() + offset.toPointF();

        ParticleParams params;
        params.position = point;                    //当前位置
        params.direction = offset.normalized();     //随机方向
        params.speed = 0;                           //速度随机0~0.5;
        params.velocity = params.speed * params.direction;
        params.startColor = m_params.endColor.darker();             //当前颜色
        params.endColor = m_params.endColor.lighter();             //结束颜色
        params.size = 1.5 + QRandomGenerator::global()->bounded(1.5);
        params.lifeTime = 5 + QRandomGenerator::global()->bounded(5);
        Particle* p = new Particle(params);
        p->setDelay(QRandomGenerator::global()->bounded(40));
        m_scene->addItem(p);
    }
}


void FireworkParticle::exploding()
{
    int lifeTime = 30 + QRandomGenerator::global()->bounded(10);
    for (int i = 0; i < 30; ++i) {
        qreal radian = i * 2 * M_PI / 30;
        QVector2D v = calculateHeartPosition(radian);
        ParticleParams params;
        params.position = this->pos();                    //当前位置
        params.direction = v.normalized();     //随机方向
        params.speed = v.length();
        params.velocity = params.speed * params.direction;
        params.startColor = m_params.startColor.lighter();        //当前颜色
        params.endColor = m_params.endColor;                      //结束颜色
        params.size = 0.5 * m_params.size;
        params.lifeTime = lifeTime;
        FlameParticle* p = new FlameParticle(params);
        p->setFlickerFrequency(20);
        p->setExplodeParams(true,false);
        p->setScene(m_scene);
        m_scene->addItem(p);
    }
    for (int i = 0; i < 30; ++i) {
        qreal radian = i * 2 * M_PI / 30;
        QVector2D v = calculateHeartPosition(radian);

        ParticleParams params;
        params.position = this->pos();                    //当前位置
        params.direction = v.normalized();     //随机方向
        params.speed = 0.4 * v.length();
        params.velocity = params.speed * params.direction;
        params.startColor = m_params.startColor.lighter();        //当前颜色
        params.endColor = m_params.startColor;                      //结束颜色
        params.size = 0.2 * m_params.size;
        params.lifeTime = lifeTime;
        FlameParticle* p = new FlameParticle(params);
        p->setExplodeParams(true,true);
        p->setScene(m_scene);
        m_scene->addItem(p);
    }
}

QVector2D FireworkParticle::calculateHeartPosition(qreal angle) const
{
    const qreal x = 16 * qPow(qSin(angle), 3);
    const qreal y = 13 * qCos(angle) - 5 * qCos(2*angle)
                    - 2 * qCos(3*angle) - qCos(4*angle);
    return QVector2D(x, -y) * 5.0 * 0.1; // 缩放系数调整
}

//-------------------------------------------------------------------------------------------

OrchidItem::OrchidItem(const QVector2D &point, const QVector2D &v, QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_scene(scene)
{
    this->setPos(point.x(),point.y());
    m_vx = v.x();
    m_vy = v.y();
    m_boundingRect = QRectF(-960,-540,1920,1080);
}

void OrchidItem::setOrchid(int length, const QColor &color1, const QColor &color2, float width1, float width2)
{
    m_length = length;
    m_color1 = color1;
    m_color2 = color2;
    m_width1 = width1;
    m_width2 = width2;
}

void OrchidItem::setPainting(int waitTime, int paintingTime, int fadingTime)
{
    m_waitTime = waitTime;
    m_paintingTime = paintingTime;
    m_fadingTime = fadingTime;
}

void OrchidItem::start()
{
    //添加轨迹
    m_paintingTime = std::max(m_paintingTime,m_length);
    for (int i = 0; i < m_length; ++i) {
        Primitive p;
        QPointF temp = GraphicsItem::calculateParabolaTrack(QPointF(0,0),m_vx,m_vy,2*STEP_TIME,i);
        QColor tempColor = GraphicsItem::gradientColor(m_color1,m_color2,m_length,i);
        float tempWidth = GraphicsItem::gradientWidth(m_width1,m_width2,m_length,i);
        p.point = temp;
        p.color = tempColor;
        p.width = tempWidth;
        track.append(p);
    }
    m_scene->addItem(this);
}

void OrchidItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_waitTime > 0)
    {
        m_waitTime--;
    }
    else
    {
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        if(m_paintingTime > 0)
        {
            m_paintingTime--;
            int paintCount = track.count() - m_paintingTime;
            if(paintCount > 0)
            {
                if(paintCount >= track.count())
                {
                    paintCount = track.count() - 1;
                }
                for (int i = 0; i < paintCount; ++i) {
                    pen.setColor(track.at(i).color);
                    pen.setWidthF(track.at(i).width);
                    painter->setPen(pen);
                    painter->drawLine(track.at(i).point,track.at(i+1).point);
                }
            }
        }
        else
        {
            if(m_fadingTime > 1) //执行退场绘制
            {
                m_fadingTime--;
                for (int i = 0; i < track.count() - 1; ++i) {
                    pen.setColor(track.at(i).color);
                    pen.setWidthF(track.at(i).width - track.at(i).width/m_fadingTime);
                    painter->setPen(pen);
                    painter->drawLine(track.at(i).point,track.at(i+1).point);
                }
            }
            else
            {
                m_scene->removeItem(this);
                delete this;
            }
        }
    }
}

QRectF OrchidItem::boundingRect() const
{
    return m_boundingRect;
}

//-------------------------------------------------------------------------------------------

QRectF PixmapItem::boundingRect() const
{
    return QRect(-pix.width()/2,-pix.height()/2,pix.width(),pix.height());
}

void PixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(boundingRect(), pix, pix.rect());
}

//----------------------------------------------------------------------------------------





void CustomButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 记录按下状态或执行其他操作
        event->accept(); // 表示事件已处理
    } else {
        event->ignore(); // 忽略其他按钮事件
    }
}

void CustomButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && boundingRect().contains(event->pos())) {
        // 如果释放位置在对象内部，则触发点击信号
        event->accept();
        emit clicked();
    } else {
        event->ignore();
    }
}














