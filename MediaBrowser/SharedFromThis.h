/*
 * qsharedfromthis.h
 * see http://maemo.gitorious.org/maemo-af/libqttracker/blobs/8579609b7d1283ac3776ae9f9e08bebbbcf822f0/src/qsharedfromthis.h
 *
 *  Created on: Jan 12, 2009
 *      Author: iridian
 */

#ifndef SHAREDFROMTHIS_H
#define SHAREDFROMTHIS_H

#include <QtCore/QSharedPointer>

template<typename T>
class SharedFromThis
{
public:

    QSharedPointer<T> sharedFromThis()
    {
        QSharedPointer<T> ret = shared_from_this;
        if(!ret)
            shared_from_this = ret = QSharedPointer<T>(static_cast<T *>(this));
        return ret;
    }

    QSharedPointer<T const> sharedFromThis() const
    {
        QSharedPointer<T> ret = shared_from_this;
        if(!ret)
            shared_from_this = ret = QSharedPointer<T>(const_cast<T *>(static_cast<T const *>(this)));
        return ret;
    }

    template<typename UpCasted_>
    QSharedPointer<UpCasted_> sharedFromThis()
    {
        return qSharedPointerCast<UpCasted_>(sharedFromThis());
    }

    template<typename UpCasted_>
    QSharedPointer<UpCasted_ const> sharedFromThis() const
    {
        return qSharedPointerCast<UpCasted_ const>(sharedFromThis());
    }

private:
    mutable QWeakPointer<T> shared_from_this;

};

#endif // SHAREDFROMTHIS_H
