#include "qdownloader.h"
#include <QNetworkReply>
#include <QDebug>

QDownloader::QDownloader(const QUrl& url, QObject *parent) :
    QObject(parent),
    m_url(url)
{
    connect(
        &m_WebCtrl, &QNetworkAccessManager::finished,
        this, &QDownloader::fileDownloaded
        );
}

QDownloader::~QDownloader() { }

void QDownloader::download()
{
    QNetworkRequest request(m_url);

    // Setting redirect policy to handle HTTP redirects
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply* reply = m_WebCtrl.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &QDownloader::progress);
    connect(reply, &QNetworkReply::errorOccurred, this, &QDownloader::onErrorOccurred);
}

void QDownloader::fileDownloaded(QNetworkReply* pReply) {
    if (pReply->error() == QNetworkReply::NoError) {
        // Successfully downloaded, read data
        m_DownloadedData = pReply->readAll();
        emit downloaded();
    } else {
        // Log the error message
        qWarning() << "Download failed with error:" << pReply->errorString();
    }

    pReply->deleteLater();
}

void QDownloader::onErrorOccurred(QNetworkReply::NetworkError code) {
    QString errorMessage;
    switch (code) {
    case QNetworkReply::HostNotFoundError:
        errorMessage = "Error: Host not found.";
        break;
    case QNetworkReply::TimeoutError:
        errorMessage = "Error: Connection timed out.";
        break;
    case QNetworkReply::ConnectionRefusedError:
        errorMessage = "Error: Connection refused by the server.";
        break;
    case QNetworkReply::ContentNotFoundError:
        errorMessage = "Error: Requested content not found on the server.";
        break;
    default:
        errorMessage = "Error: An unknown network error occurred.";
        break;
    }

    qWarning() << errorMessage;

    // Emit a custom error signal with the error message
    emit downloadFailed(errorMessage);
}

QByteArray QDownloader::downloadedData() const {
    return m_DownloadedData;
}
