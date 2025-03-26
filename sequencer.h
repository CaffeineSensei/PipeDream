#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <QThread>
#include <QTimer>
#include <QGraphicsScene>
class Screenwriter;
//class GraphicsScene;

struct TimelineEvent {
    int timestamp; // 时间戳（秒）
    std::function<void()> callback; // 回调函数

};

class Sequencer : public QThread
{
    Q_OBJECT
public:
    explicit Sequencer(QGraphicsScene *scene,QObject *parent = nullptr);
    ~Sequencer();
    void setCurrentTimestamp(int timestamp);                                //接收父对象的时间戳，即音乐播放进度

protected:
    void run() override;                                                    //线程任务

public slots:
    void onTimerTimeout();
    void onBackgroundLoading();
    void onBackgroundChanged(qreal start, qreal end, int duration);         //背景改变

private:
    void addEvent(int timestamp, std::function<void()> callback);           //添加任务事件

    //场景任务
    void backgroundFadein();
    void backgroundFadeout();
    void sakura();                      //场景一：樱树
    void firefly();                     //场景二：萤火虫
    void spiralParticle();              //场景三：粒子环绕
    void fireworks();                   //场景四：烟花
    void orchidBubbleFireworks();       //场景五：兰花、泡影

    void endOfCurrentScene();           //每场演出结束
    void sceneTransition2();            //场景过渡（白切黑）
    void sceneTransition3();            //场景过渡（黑切白）

    void setSceneColorGrayGradually(int start,int end);

signals:
    void backgroundLoading();
    void backgroundChanged(qreal start, qreal end, int duration);

private:
    QTimer * m_timer;
    QGraphicsScene *m_scene;
    QList<TimelineEvent> m_events;
    int m_timestamp;

    QList<Screenwriter*> m_screenwriters;
    //bool m_showing = false;

};

#endif // SEQUENCER_H
