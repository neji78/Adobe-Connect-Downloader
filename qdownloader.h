#ifndef QDOWNLOADER_H
#define QDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class QDownloader : public QObject
{
    Q_OBJECT
public:
    explicit QDownloader(const QUrl& url, QObject *parent = nullptr);
    QByteArray downloadedData() const;
    virtual ~QDownloader();

signals:
    void downloadFailed(const QString& errorString);
    void downloaded();
    void progress(qint64 read, qint64 total);
public slots:
    void onErrorOccurred(QNetworkReply::NetworkError code);
    void download();
private slots:
    void fileDownloaded(QNetworkReply* pReply);
    // void onProgress(qint64 read, qint64 total);
private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
    QUrl m_url;

signals:
};

#endif // QDOWNLOADER_H
