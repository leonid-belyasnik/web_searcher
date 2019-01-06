/**
* \file	IProcessDelegate.h
* \brief Interface of process delegate
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	04/01/2019
*/

#ifndef IPROCESSDELEGATE_H
#define IPROCESSDELEGATE_H

#include <QString>

namespace dx_test {

/**
 * @brief The IProcessDelegate class-interface of process delegate
 */
class IProcessDelegate
{
public:
    virtual ~IProcessDelegate(){}
    virtual void addNew(uint32_t id, uint32_t parent_id, const QString &url) = 0;
    virtual void onLoad(uint32_t id) = 0;
    virtual void onSearch(uint32_t id) = 0;
    virtual void onFound(uint32_t id, bool value) = 0;
    virtual void onError(uint32_t id, const QString &msg) = 0;
    virtual void onStarted() = 0;
    virtual void onPaused() = 0;
    virtual void onStopped() = 0;
    virtual void onResume() = 0;
    virtual void onDone() = 0;
    virtual void onStatus(const QString &msg) = 0;
    virtual void onProgress(uint32_t used_num) = 0;
};

} // dx_test
#endif // IPROCESSDELEGATE_H
