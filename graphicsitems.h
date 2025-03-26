#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include <QGraphicsObject>
#include <QVector2D>

#define STEP_TIME 0.1
#define Gravity 6.0

namespace GraphicsItem {

    QColor gradientColor(const QColor &color1,const QColor &color2,int step,int n);
    qreal gradientWidth(float w1,float w2,int step,int n);
    QPointF calculateParabolaTrack(QPointF startPoint,int vx,int vy,float dt,int num);
}

typedef struct
{
    QPointF position;       //粒子坐标
    qreal speed;            //粒子速度
    QVector2D direction;    //速度方向
    QVector2D velocity;     //矢量速度
    QColor startColor;      //粒子初始颜色
    QColor endColor;        //粒子结束颜色
    qreal size;             //粒子尺寸
    int lifeTime;           //粒子生命周期

}ParticleParams;

//-------------------------------------------------------------------------------------------

//粒子基类
class Particle : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Particle(const ParticleParams& params, QGraphicsItem* parent = nullptr);

    //更新粒子状态
    virtual void updatePaint();

    int age() const { return m_age; }
    bool isDead() const { return m_age >= m_params.lifeTime; }
    const ParticleParams &params(){return m_params;}

    void setParams(const ParticleParams &params){m_params = params;}
    void setVibration(qreal orthometricAmplitude,qreal parallelAmplitude,qreal frequency,bool randomPhase = true,qreal phase = 0);
    void setDelay(int delay){m_delay = delay;}
protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    QColor interpolateColor() const;

    ParticleParams m_params;
    QPixmap m_pixmap;
    int m_age;
    int m_delay;

public:
    qreal m_orthometricAmplitude;   //正交振幅
    qreal m_parallelAmplitude;      //平行振幅
    qreal m_frequency;              //频率
    qreal m_phase;                  //初相位
};

//----------------------------------------------------------------------------------------------

//光源粒子
class LampParticle : public Particle
{
public:
    LampParticle(const ParticleParams& params, QGraphicsItem* parent = nullptr);
    void updatePaint() override;
    void setFlickerFrequency(int frequency){m_flickerFrequency = frequency;}

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    int m_flickerFrequency;     // 闪烁频率（Hz）
    qreal m_flickerPhase;       // 相位 [0, 2π]
    qreal m_flickerProgress;    // 动画进度 [0,1]
    qreal m_pulseIntensity;     // 尺寸脉冲强度
};


class FlameParticle : public LampParticle
{
public:
    FlameParticle(const ParticleParams& params, QGraphicsItem* parent = nullptr)
        : LampParticle(params,parent)
        , m_splash(false)
        , m_explode(false){}
    void updatePaint() override;
    void setExplodeParams(bool splash,bool explode){m_splash = splash;m_explode = explode;}
    void setScene(QGraphicsScene *scene){m_scene = scene;}
protected:
    virtual void splashing();
    virtual void exploding();
    QGraphicsScene *m_scene;
private:

    bool m_splash;
    bool m_explode;
};

class FireworkParticle : public FlameParticle
{
public:
    using FlameParticle::FlameParticle;
    void exploding() override;
    QVector2D calculateHeartPosition(qreal angle) const;
};

//重写PixmapItem类,坑爹玩意QGraphicsPixmapItem是继承的QGraphicsItem而非QGraphicsObject，不能应用QPropertyAnimation
class PixmapItem : public QGraphicsObject
{
public:
    PixmapItem(const QPixmap& pixmap, QGraphicsItem* parent = nullptr) : QGraphicsObject(parent), pix(pixmap){}

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QPixmap pix;

};


class BackgroundItem : public PixmapItem
{
public:
    using PixmapItem::PixmapItem;
};


class FallingItem : public PixmapItem
{
public:
    using PixmapItem::PixmapItem;
};

class OrchidItem : public QGraphicsObject
{
public:
    OrchidItem(const QVector2D &point,const QVector2D &v, QGraphicsScene *scene,QGraphicsItem *parent = nullptr);
    void setOrchid(int length,const QColor &color1,const QColor &color2,float width1,float width2); //设置轨迹参数
    void setPainting(int waitTime,int paintingTime,int fadingTime);                                 //设置绘制参数
    void start();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    QRectF boundingRect() const override;

private:
    QGraphicsScene *m_scene;
    QPointF m_startPoint;
    int m_length;

    float m_vx,m_vy;
    QColor m_color1,m_color2;
    float m_width1,m_width2;
    int m_waitTime;
    int m_paintingTime;
    int m_fadingTime;

    struct Primitive
    {
        QPointF point;
        QColor color;
        qreal width;
    };

    QVector<Primitive> track;
    QRectF m_boundingRect;
};


class CustomButton : public PixmapItem
{
    Q_OBJECT
public:
    using PixmapItem::PixmapItem;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
signals:
    void clicked();
};

#endif // GRAPHICSITEMS_H
