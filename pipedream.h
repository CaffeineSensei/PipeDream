#ifndef PIPEDREAM_H
#define PIPEDREAM_H

#include <QGraphicsView>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "sequencer.h"
/**
 * @brief The PipeDream class
 *
 * PipeDream(QGraphicsView)相当于剧场
 * |
 * ├────────────────────────────────────┐
 * │                                    │
 * │                                    │
 * ├ QGraphicsScene相当于舞台             ├Sequencer相当于舞台调度
 * │                                    │
 * │                                    │
 * │                                    │
 * │                                    │
 * ├ QGraphicsObject相当于演员、演出道具   ├Screenwriter相当于编剧────────────────────EnframedScenery 动态框景
 *                                                                │
 *                                                                │
 *                                                                │
 *                                                                ├───────────────ParticleSystem  粒子系统
 *
 *
 *
 */

class GraphicsScene;

class PipeDream : public QGraphicsView
{
    Q_OBJECT

public:
    PipeDream(QWidget *parent = nullptr);
    ~PipeDream();

public slots:
    void onMusicPositionChanged(qint64 position);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onCustomButtonClicked();

signals:
    void played();
private:

    //音乐播放
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;

    //绘图场景
    QGraphicsScene *m_scene;
    Sequencer *m_sequencer;

    //计时
    int m_count = 0;

};
#endif // PIPEDREAM_H
