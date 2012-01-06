/*
 * Copyright (C) 2011 Zeno Albisser <zeno@webkit.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "QtNetworkReply.h"

#include "SharedMemory.h"
#include "WebFrameNetworkingContext.h"
#include "WebPage.h"
#include "WebProcess.h"
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace WebKit {

QtNetworkReply::QtNetworkReply(const QNetworkRequest& req, QtNetworkAccessManager* parent)
    : QNetworkReply(parent)
    , m_bytesAvailable(0)
    , m_sharedMemorySize(0)
{
    setRequest(req);
    setOperation(QNetworkAccessManager::GetOperation);
    setUrl(req.url());
    setOpenMode(QIODevice::ReadOnly);
    setHeader(QNetworkRequest::ContentTypeHeader, QVariant(QString::fromLocal8Bit("text/html; charset=UTF-16")));
}

void QtNetworkReply::setData(const SharedMemory::Handle& handle, qint64 dataSize)
{
    m_sharedMemory = SharedMemory::create(handle, SharedMemory::ReadOnly);
    if (!m_sharedMemory)
        return;

    m_bytesAvailable = dataSize;
    m_sharedMemorySize = dataSize;
}

void QtNetworkReply::setReplyData(const QtNetworkReplyData& replyData)
{
    if (replyData.m_operation)
        setOperation(replyData.m_operation);
    if (!replyData.m_contentDisposition.isNull())
        setHeader(QNetworkRequest::ContentDispositionHeader, QString(replyData.m_contentDisposition));
    if (!replyData.m_contentType.isNull())
        setHeader(QNetworkRequest::ContentTypeHeader, QString(replyData.m_contentType));
    if (!replyData.m_location.isNull())
        setHeader(QNetworkRequest::LocationHeader, QString(replyData.m_location));
    if (replyData.m_lastModified)
        setHeader(QNetworkRequest::LastModifiedHeader, QDateTime::fromMSecsSinceEpoch(replyData.m_lastModified));
    if (!replyData.m_cookie.isNull())
        setHeader(QNetworkRequest::SetCookieHeader, QVariant::fromValue(QNetworkCookie::parseCookies(QString(replyData.m_cookie).toAscii())));
    if (!replyData.m_userAgent.isNull())
        setHeader(QNetworkRequest::UserAgentHeader, QString(replyData.m_userAgent));
    if (!replyData.m_server.isNull())
        setHeader(QNetworkRequest::ServerHeader, QString(replyData.m_server));
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant::fromValue(replyData.m_contentLength));
    setData(replyData.m_dataHandle, replyData.m_contentLength);
}

qint64 QtNetworkReply::readData(char* data, qint64 maxlen)
{
    qint64 bytesRead = maxlen < m_bytesAvailable ? maxlen : m_bytesAvailable;
    if (qMemCopy(data, static_cast<char*>(m_sharedMemory->data()) + m_sharedMemorySize - m_bytesAvailable, bytesRead)) {
        m_bytesAvailable -= bytesRead;
        return bytesRead;
    }
    return 0;
}

qint64 QtNetworkReply::bytesAvailable() const
{
    return m_bytesAvailable + QNetworkReply::bytesAvailable();
}

void QtNetworkReply::setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
    QNetworkReply::setHeader(header, value);
}

void QtNetworkReply::abort() { }
void QtNetworkReply::close() { }
void QtNetworkReply::setReadBufferSize(qint64 size) { }
bool QtNetworkReply::canReadLine () const { return true; }

void QtNetworkReply::finalize()
{
    QNetworkReply::setFinished(true);
    emit readyRead();
    emit finished();
}

} // namespace WebKit
