#include "qhome.h"
#include "qdownloader.h"
#include "utils.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTimer>
#include <QStandardPaths>
#include <QApplication>

void QHome::saveFile(const QByteArray &data)
{
    setStatus("Saving data...");
    qDebug() << __LINE__ << __PRETTY_FUNCTION__ << QDir::currentPath();

    QFile::remove(QDir::currentPath() + "/content.zip");
    QFile file(QDir::currentPath() + "/content.zip");

    if (!file.open(QIODevice::WriteOnly)) {
        setStatus("Error: Unable to save file - " + file.errorString());
        return;
    }

    if (file.write(data) == -1) {
        setStatus("Error: Unable to write data - " + file.errorString());
        return;
    }

    file.close();
    setStatus("File saved successfully.");
    unzipFile();
}
void QHome::unzipFile()
{
    QString program = "C:/Program Files/WinRAR/winRAR.exe";
    if (!QFile::exists(program)) {
        setStatus("Error: WinRAR not found at " + program);
        return;
    }

    QString extractDirectory = QDir::currentPath() + "/unzip";
    QDir(extractDirectory).removeRecursively();
    if (!QDir().mkdir(extractDirectory)) {
        setStatus("Error: Unable to create extraction directory.");
        return;
    }

    QStringList arguments;
    arguments << "x" << "-y"
              << QDir::toNativeSeparators(QDir::currentPath() + "/content.zip")
              << QDir::toNativeSeparators(extractDirectory);

    QProcess *myProcess = new QProcess(this);
    connect(myProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        setStatus("Error: Unzipping failed - " + myProcess->errorString());
    });
    connect(myProcess, &QProcess::started, this, [=]() {
        setStatus("Unzipping...");
    });
    connect(myProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    setStatus("Unzipping completed successfully.");
                    QTimer::singleShot(1, this, &QHome::mergeStreams);
                } else {
                    setStatus("Error: Unzipping failed with exit code " + QString::number(exitCode));
                }
            });

    myProcess->start(program, arguments);
}

void QHome::mergeStreams()
{
    QDir directory(QDir::currentPath() + "/unzip");
    QStringList streams = directory.entryList(QStringList() << "*.flv", QDir::Files);

    QString videoInput;
    QString audioInput;

    for (const QString &filename : streams) {
        if (filename.contains("screenshare") && videoInput.isEmpty()) {
            videoInput = filename;
        } else if (filename.contains("cameraVoip") && audioInput.isEmpty()) {
            audioInput = filename;
        }
    }

    if (videoInput.isEmpty() || audioInput.isEmpty()) {
        setStatus("Error: Required video/audio files not found.");
        return;
    }

    QFile::remove(QDir::currentPath() + "/output.flv");
    QString program = QDir::currentPath() + "/ffmpeg.exe";

    if (!QFile::exists(program)) {
        setStatus("Error: ffmpeg not found at " + program);
        return;
    }

    QStringList arguments;
    arguments << "-i" << directory.filePath(videoInput)
              << "-i" << directory.filePath(audioInput)
              << "-c:v" << "copy" << "-c:a" << "aac"
              << "-map" << "0:v:0" << "-map" << "1:a:0"
              << "output.flv";

    QProcess *myProcess = new QProcess(this);
    connect(myProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        setStatus("Error: Merging failed - " + myProcess->errorString());
    });
    connect(myProcess, &QProcess::started, this, [=]() {
        setStatus("Merging files...");
        setProcessing(true);
    });
    connect(myProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        setLog(myProcess->readAllStandardOutput());
    });
    connect(myProcess, &QProcess::readyReadStandardError, this, [=]() {
        setLog(myProcess->readAllStandardError());
    });
    connect(myProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    setStatus("Files merged successfully.");
                    setProcessing(false);
                    showDialog();
                } else {
                    setStatus("Error: Merging failed with exit code " + QString::number(exitCode));
                }
            });

    myProcess->start(program, arguments);
}

QHome::QHome():
    m_processing(false),
    m_loading(false)
{

}


void QHome::saveFileAs(const QString &fileName)
{
    QString path = QUrl(fileName).path().removeFirst();
    qDebug() << __LINE__ << __PRETTY_FUNCTION__ << fileName << path;

    QFile file(QDir::currentPath() + "/output.flv");
    if (!file.copy(path)) {
        setStatus("Error: Unable to save video - " + file.errorString());
    } else {
        setStatus("Video saved successfully: " + path);
    }
    setLoading(false);
}

void QHome::registerType()
{
    qmlRegisterType<QHome>("org.bc.main",1,0,"Home");
}

void QHome::downloadFile(const QString &url)
{
    if(url.isEmpty()){
        setStatus("Error: URL cannot be empty.");
        return;
    }
    auto urlParts = url.split("?");
    QString convertedUrl = urlParts[0] + "output/content.zip?" + urlParts[1] + "&download=zip";
    QUrl _url(convertedUrl);

    auto downloader = new QDownloader(_url);
    connect(downloader, &QDownloader::downloaded, this, &QHome::onDownloadFinished);
    connect(downloader, &QDownloader::progress, this, &QHome::onProgress);
    connect(downloader, &QDownloader::downloadFailed, this, [=](const QString &error) {
        setStatus("Error: " + error);
        setLoading(false);
    });

    setLoading(true);
    setStatus("Downloading file from " + url);
    m_lastTime = QDateTime::currentMSecsSinceEpoch();
    m_lastReaded = 0;
    downloader->download();

    qDebug() << __LINE__ << __PRETTY_FUNCTION__ << url << _url;
}


void QHome::onDownloadFinished()
{
    setStatus("Download Finished :)");
    setProcessing(true);
    auto obj = reinterpret_cast<QDownloader*>(sender());
    auto data = obj->downloadedData();
    saveFile(data);
    obj->deleteLater();
    qDebug()<<__LINE__<<__PRETTY_FUNCTION__;
}

void QHome::onProgress(qint64 read, qint64 total)
{
    setReadInt(read);
    setTotalInt(total);
    auto percent = (static_cast<double>(read) / static_cast<double>(total)) * 100;
    setPercent(QString::number(percent,'f',1)+" %");
    setTotal(QString(humanSize((total))));
    setRead(QString(humanSize((read))));
    auto speed = (static_cast<double>(read - m_lastReaded))/(static_cast<double>(QDateTime::currentMSecsSinceEpoch() - m_lastTime))*1000;
    setSpeed(QString(humanSize((speed))) + "/s");
}

QString QHome::status() const
{
    return m_status;
}

void QHome::setStatus(const QString &newStatus)
{
    if (m_status == newStatus)
        return;
    m_status = newStatus;
    emit statusChanged();
}

QString QHome::read() const
{
    return m_read;
}

void QHome::setRead(const QString &newRead)
{
    if (m_read == newRead)
        return;
    m_read = newRead;
    emit readChanged();
}

QString QHome::total() const
{
    return m_total;
}

void QHome::setTotal(const QString &newTotal)
{
    if (m_total == newTotal)
        return;
    m_total = newTotal;
    emit totalChanged();
}

QString QHome::speed() const
{
    return m_speed;
}

void QHome::setSpeed(const QString &newSpeed)
{
    if (m_speed == newSpeed)
        return;
    m_speed = newSpeed;
    emit speedChanged();
}

QString QHome::percent() const
{
    return m_percent;
}

void QHome::setPercent(const QString &newPercent)
{
    if (m_percent == newPercent)
        return;
    m_percent = newPercent;
    emit percentChanged();
}

bool QHome::loading() const
{
    return m_loading;
}

void QHome::setLoading(bool newLoading)
{
    if (m_loading == newLoading)
        return;
    m_loading = newLoading;
    emit loadingChanged();
}

qint64 QHome::readInt() const
{
    return m_readInt;
}

void QHome::setReadInt(qint64 newReadInt)
{
    if (m_readInt == newReadInt)
        return;
    m_readInt = newReadInt;
    emit readIntChanged();
}

qint64 QHome::totalInt() const
{
    return m_totalInt;
}

void QHome::setTotalInt(qint64 newTotalInt)
{
    if (m_totalInt == newTotalInt)
        return;
    m_totalInt = newTotalInt;
    emit totalIntChanged();
}

bool QHome::processing() const
{
    return m_processing;
}

void QHome::setProcessing(bool newProcessing)
{
    if (m_processing == newProcessing)
        return;
    m_processing = newProcessing;
    emit processingChanged();
}

QString QHome::log() const
{
    return m_log;
}

void QHome::setLog(const QString &newLog)
{
    if (m_log == newLog)
        return;
    m_log = newLog;
    emit logChanged();
}
