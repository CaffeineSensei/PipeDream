#ifndef SCREENWRITER_H
#define SCREENWRITER_H

#include <QObject>
#include <QGraphicsScene>

class Emitter;
class Affector;


class Screenwriter : public QObject
{
    Q_OBJECT
public:
    explicit Screenwriter(QGraphicsScene* scene,QObject *parent = nullptr) : QObject(parent),m_scene(scene){}
    virtual ~Screenwriter(){}

    void start(){m_showing = true;}
    bool isShowing(){return m_showing;}
    void shouldStop(){m_shouldStop = true;}
    bool isExecuted(){return m_exeunted;}

    virtual void precondition() = 0;    //事先准备
    virtual void actOut() = 0;          //演出

protected:
    QGraphicsScene *m_scene;
    bool m_showing = false;
    bool m_shouldStop = false;
    bool m_exeunted = false;

};

//动态框景
class EnframedScenery : public Screenwriter
{
    Q_OBJECT
public:
    using Screenwriter::Screenwriter;
    ~EnframedScenery();
    void addDynamicPixmap(const QPixmap &pix){dynamicPixmaps.append(pix);}      //添加动态图片

    void precondition() override;
    void actOut() override;

private:
    void createFalling();
    QVector<QPixmap> dynamicPixmaps;

};

//粒子系统
class ParticleSystem : public Screenwriter
{
    Q_OBJECT
public:
    using Screenwriter::Screenwriter;
    ~ParticleSystem();
    void addEmitter(Emitter* emitter) { emitters.append(emitter); }       //添加粒子发射器
    void addAffector(Affector* affector) { affectors.append(affector); }  //添加粒子干扰器

    void precondition() override;
    void actOut() override;

private:
    void emitParticles();
    QList<Emitter*> emitters;
    QList<Affector*> affectors;
};


class CustomScenery : public Screenwriter
{
    Q_OBJECT
public:
    using Screenwriter::Screenwriter;
    ~CustomScenery();
    void precondition() override;
    void actOut() override;

private:
    void addOrchid();
    void addPipe();
    void addFireWork();
};


#endif // SCREENWRITER_H
