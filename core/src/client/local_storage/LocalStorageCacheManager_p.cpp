#include "LocalStorageCacheManager_p.h"
#include "DefaultLocalStorageCacheExpiryChecker.h"
#include "LocalStorageCacheManagerException.h"
#include <tools/QuteNoteCheckPtr.h>
#include <logging/QuteNoteLogger.h>
#include <QDateTime>

namespace qute_note {

LocalStorageCacheManagerPrivate::LocalStorageCacheManagerPrivate(LocalStorageCacheManager & q) :
    q_ptr(&q),
    m_cacheExpiryChecker(new DefaultLocalStorageCacheExpiryChecker(q)),
    m_notesCache(),
    m_notebooksCache(),
    m_tagsCache(),
    m_linkedNotebooksCache()
{}

LocalStorageCacheManagerPrivate::~LocalStorageCacheManagerPrivate()
{}

#define NUM_CACHED_OBJECTS(type, method_name, cache_name, IndexType) \
size_t LocalStorageCacheManagerPrivate::method_name() const \
{ \
    const auto & index = cache_name.get<type##Holder::IndexType>(); \
    return index.size(); \
}

NUM_CACHED_OBJECTS(Note, numCachedNotes, m_notesCache, ByLocalGuid)
NUM_CACHED_OBJECTS(Notebook, numCachedNotebooks, m_notebooksCache, ByLocalGuid)
NUM_CACHED_OBJECTS(Tag, numCachedTags, m_tagsCache, ByLocalGuid)
NUM_CACHED_OBJECTS(LinkedNotebook, numCachedLinkedNotebooks, m_linkedNotebooksCache, ByGuid)

#undef NUM_CACHED_OBJECTS

#define CACHE_OBJECT(Type, name, cache_type, cache_name, expiry_checker, IndexType, IndexAccessor) \
void LocalStorageCacheManagerPrivate::cache##Type(const Type & name) \
{ \
    QUTE_NOTE_CHECK_PTR(m_cacheExpiryChecker.data()); \
    \
    typedef boost::multi_index::index<cache_type,Type##Holder::ByLastAccessTimestamp>::type LastAccessTimestampIndex; \
    LastAccessTimestampIndex & latIndex = cache_name.get<Type##Holder::ByLastAccessTimestamp>(); \
    \
    if (Q_LIKELY(!m_cacheExpiryChecker.isNull())) \
    {   \
        bool res = false;   \
        while(!res && !latIndex.empty()) \
        { \
            res = m_cacheExpiryChecker->expiry_checker(); \
            if (Q_UNLIKELY(!res)) { \
                Q_UNUSED(cache_name.erase(latIndex.rbegin().base())); \
                continue; \
            } \
        } \
    } \
    \
    Type##Holder name##Holder; \
    name##Holder.m_##name = name; \
    name##Holder.m_lastAccessTimestamp = QDateTime::currentMSecsSinceEpoch(); \
    \
    /* See whether the note is already in the cache */ \
    typedef boost::multi_index::index<cache_type,Type##Holder::IndexType>::type Index; \
    Index & uniqueIndex = cache_name.get<Type##Holder::IndexType>(); \
    Index::iterator it = uniqueIndex.find(name.IndexAccessor()); \
    if (it != uniqueIndex.end()) { \
        uniqueIndex.replace(it, name##Holder); \
        QNDEBUG("Updated " #name " in the local storage cache: " << name); \
        return; \
    } \
    \
    /* If got here, no existing note was found in the cache */ \
    auto insertionResult = cache_name.insert(name##Holder); \
    if (Q_UNLIKELY(!insertionResult.second)) { \
        QNWARNING("Failed to insert " #name " into the cache of local storage manager: " << name); \
        throw LocalStorageCacheManagerException("Unable to insert " #name " into the local storage cache"); \
    } \
    \
    QNDEBUG("Added " #name " to the local storage cache: " << name); \
}

CACHE_OBJECT(Note, note, NotesCache, m_notesCache, checkNotes, ByLocalGuid, localGuid)
CACHE_OBJECT(Notebook, notebook, NotebooksCache, m_notebooksCache, checkNotebooks, ByLocalGuid, localGuid)
CACHE_OBJECT(Tag, tag, TagsCache, m_tagsCache, checkTags, ByLocalGuid, localGuid)
CACHE_OBJECT(LinkedNotebook, linkedNotebook, LinkedNotebooksCache, m_linkedNotebooksCache,
             checkLinkedNotebooks, ByGuid, guid)

#undef CACHE_OBJECT

#define EXPUNGE_OBJECT(Type, name, cache_type, cache_name) \
void LocalStorageCacheManagerPrivate::expunge##Type(const Type & name) \
{ \
    bool name##HasGuid = name.hasGuid(); \
    const QString guid = (name##HasGuid ? name.guid() : name.localGuid()); \
    \
    if (name##HasGuid) \
    { \
        typedef boost::multi_index::index<cache_type,Type##Holder::ByGuid>::type GuidIndex; \
        GuidIndex & index = cache_name.get<Type##Holder::ByGuid>(); \
        GuidIndex::iterator it = index.find(guid); \
        if (it != index.end()) { \
            index.erase(it); \
            QNDEBUG("Expunged " #name " from the local storage cache: " << name); \
        } \
    } \
    else \
    { \
        typedef boost::multi_index::index<cache_type,Type##Holder::ByLocalGuid>::type GuidIndex; \
        GuidIndex & index = cache_name.get<Type##Holder::ByLocalGuid>(); \
        GuidIndex::iterator it = index.find(guid); \
        if (it != index.end()) { \
            index.erase(it); \
            QNDEBUG("Expunged " #name " from the local storage cache: " << name); \
        } \
    } \
}

EXPUNGE_OBJECT(Note, note, NotesCache, m_notesCache)
EXPUNGE_OBJECT(Notebook, notebook, NotebooksCache, m_notebooksCache)
EXPUNGE_OBJECT(Tag, tag, TagsCache, m_tagsCache)

#undef EXPUNGE_OBJECT

void LocalStorageCacheManagerPrivate::expungeLinkedNotebook(const LinkedNotebook & linkedNotebook)
{
    const QString guid = linkedNotebook.guid();

    typedef boost::multi_index::index<LinkedNotebooksCache,LinkedNotebookHolder::ByGuid>::type GuidIndex;
    GuidIndex & index = m_linkedNotebooksCache.get<LinkedNotebookHolder::ByGuid>();
    GuidIndex::iterator it = index.find(guid);
    if (it != index.end()) {
        index.erase(it);
        QNDEBUG("Expunged linked notebook from the local storage cache: " << linkedNotebook);
    }
}

#define FIND_OBJECT(Type, name, tag, cache_name) \
const Type * LocalStorageCacheManagerPrivate::find##Type##tag(const QString & guid) const \
{ \
    const auto & index = cache_name.get<Type##Holder::tag>(); \
    auto it = index.find(guid); \
    if (it == index.end()) { \
        return nullptr; \
    } \
    \
    return &(it->m_##name); \
}

FIND_OBJECT(Note, note, ByLocalGuid, m_notesCache)
FIND_OBJECT(Note, note, ByGuid, m_notesCache)
FIND_OBJECT(Notebook, notebook, ByLocalGuid, m_notebooksCache)
FIND_OBJECT(Notebook, notebook, ByGuid, m_notebooksCache)
FIND_OBJECT(Tag, tag, ByLocalGuid, m_tagsCache)
FIND_OBJECT(Tag, tag, ByGuid, m_tagsCache)
FIND_OBJECT(LinkedNotebook, linkedNotebook, ByGuid, m_linkedNotebooksCache)

#undef FIND_OBJECT

void LocalStorageCacheManagerPrivate::installCacheExpiryFunction(const ILocalStorageCacheExpiryChecker & checker)
{
    m_cacheExpiryChecker.reset(checker.clone());
}

#define GET_GUID(Type, name) \
const QString LocalStorageCacheManagerPrivate::Type##Holder::guid() const \
{ \
    /* NOTE: This precaution is required for storage of local notes in the cache */ \
    if (m_##name.hasGuid()) { \
        return m_##name.guid(); \
    } \
    else { \
        return QString(); \
    } \
}

GET_GUID(Note, note)
GET_GUID(Notebook, notebook)
GET_GUID(Tag, tag)
GET_GUID(LinkedNotebook, linkedNotebook)

#undef GET_GUID

} // namespace qute_note
