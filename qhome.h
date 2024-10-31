#ifndef QHOME_H
#define QHOME_H

#include <QQmlEngine>
#include <QMainWindow>
class QHome:public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool processing READ processing WRITE setProcessing NOTIFY processingChanged FINAL)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged FINAL)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged FINAL)
    Q_PROPERTY(QString read READ read WRITE setRead NOTIFY readChanged FINAL)
    Q_PROPERTY(qint64 readInt READ readInt WRITE setReadInt NOTIFY readIntChanged FINAL)
    Q_PROPERTY(QString total READ total WRITE setTotal NOTIFY totalChanged FINAL)
    Q_PROPERTY(qint64 totalInt READ totalInt WRITE setTotalInt NOTIFY totalIntChanged FINAL)
    Q_PROPERTY(QString speed READ speed WRITE setSpeed NOTIFY speedChanged FINAL)
    Q_PROPERTY(QString percent READ percent WRITE setPercent NOTIFY percentChanged FINAL)
    Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged FINAL)
public:
    void saveFile(const QByteArray& data);
    Q_INVOKABLE void unzipFile();
    void mergeStreams();
    QHome();
    Q_INVOKABLE void saveFileAs(const QString& fileName);
    static void registerType();
    Q_INVOKABLE void downloadFile(const QString& url);

    QString status() const;
    void setStatus(const QString &newStatus);

    QString read() const;
    void setRead(const QString &newRead);

    QString total() const;
    void setTotal(const QString &newTotal);

    QString speed() const;
    void setSpeed(const QString &newSpeed);

    QString percent() const;
    void setPercent(const QString &newPercent);

    bool loading() const;
    void setLoading(bool newLoading);

    qint64 readInt() const;
    void setReadInt(qint64 newReadInt);

    qint64 totalInt() const;
    void setTotalInt(qint64 newTotalInt);

    bool processing() const;
    void setProcessing(bool newProcessing);

    QString log() const;
    void setLog(const QString &newLog);

public slots:
    void onDownloadFinished();
    void onProgress(qint64 read, qint64 total);
signals:
    void showDialog();
    void statusChanged();
    void readChanged();

    void totalChanged();

    void speedChanged();

    void percentChanged();

    void loadingChanged();

    void readIntChanged();

    void totalIntChanged();

    void processingChanged();

    void logChanged();

private:
    qint64 m_lastReaded;
    qint64 m_lastTime;
    QString m_status;
    QString m_read;
    QString m_total;
    QString m_speed;
    QString m_percent;
    bool m_loading;
    qint64 m_readInt;
    qint64 m_totalInt;
    bool m_processing;
    QString m_log;
};

#endif // QHOME_H
