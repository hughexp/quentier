#ifndef __LIB_QUTE_NOTE__UTILITY__FILE_SYSTEM_WATCHER_PRIVATE_H
#define __LIB_QUTE_NOTE__UTILITY__FILE_SYSTEM_WATCHER_PRIVATE_H

#include <qute_note/utility/FileSystemWatcher.h>
#include <QFileSystemWatcher>
#include <QSet>
#include <boost/bimap.hpp>

namespace qute_note {

class FileSystemWatcherPrivate: public QObject
{
    Q_OBJECT
public:
    explicit FileSystemWatcherPrivate(FileSystemWatcher & parent, const int removalTimeoutMSec);
    explicit FileSystemWatcherPrivate(FileSystemWatcher & parent, const QStringList & paths, const int removalTimeoutMSec);

    virtual ~FileSystemWatcherPrivate();

    void addPath(const QString & path);
    void addPaths(const QStringList & paths);

    QStringList directories() const;
    QStringList files() const;

    void removePath(const QString & path);
    void removePaths(const QStringList & paths);

Q_SIGNALS:
    void directoryChanged(const QString & path);
    void directoryRemoved(const QString & path);

    void fileChanged(const QString & path);
    void fileRemoved(const QString & path);

private Q_SLOTS:
    void onFileChanged(const QString & path);
    void onDirectoryChanged(const QString & path);

private:
    void createConnections();
    void processFileRemoval(const QString & path);
    void processDirectoryRemoval(const QString & path);

private:
    virtual void timerEvent(QTimerEvent * pEvent) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(FileSystemWatcherPrivate)

private:
    FileSystemWatcher & m_parent;
    QFileSystemWatcher  m_watcher;
    int                 m_removalTimeoutMSec;

    typedef boost::bimap<QString, QString> WatchedFilePathWithDirPaths;
    WatchedFilePathWithDirPaths     m_watchedFilesWithDirs;

    QSet<QString>       m_watchedDirectories;
    QSet<QString>       m_implicitlyWatchedDirectories;

    typedef boost::bimap<QString, int> PathWithTimerId;
    PathWithTimerId     m_justRemovedFilePathsWithPostRemovalTimerIds;
    PathWithTimerId     m_justRemovedDirectoryPathsWithPostRemovalTimerIds;
};

} // namespace qute_note

#endif // __LIB_QUTE_NOTE__UTILITY__FILE_SYSTEM_WATCHER_PRIVATE_H
