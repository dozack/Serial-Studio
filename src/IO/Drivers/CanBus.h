/*
 * Copyright (c) 2020-2023 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <IO/Driver.h>

namespace IO
{
namespace Drivers
{

class CanBus : public IO::Driver
{
    // clang-format off
    Q_OBJECT
    Q_PROPERTY(qsizetype interfaceIndex
               READ interfaceIndex
               WRITE setInterfaceIndex
               NOTIFY interfaceIndexChanged)
    Q_PROPERTY(QStringList availableDevices
               READ availableDevices
               NOTIFY availableDevicesChanged)
    Q_PROPERTY(bool isSupported 
               READ isSupported 
               CONSTANT)
    // clang-format on

Q_SIGNALS:
    void interfaceIndexChanged();
    void availableDevicesChanged();

private:
    explicit CanBus();
    CanBus(CanBus &&) = delete;
    CanBus(const CanBus &) = delete;
    CanBus &operator=(CanBus &&) = delete;
    CanBus &operator=(const CanBus &) = delete;

    ~CanBus();

public:
    static CanBus &instance();

    void close() override;
    bool isOpen() const override;
    bool isReadable() const override;
    bool isWritable() const override;
    bool configurationOk() const override;
    quint64 write(const QByteArray &data) override;
    bool open(const QIODevice::OpenMode mode) override;

    QString interfaceName() const;
    QCanBusDevice *interface() const;

    bool isSupported() const;

    qsizetype interfaceIndex() const;

    QStringList availableDevices() const;

private:
    QStringList availableDeviceNames() const;

public Q_SLOTS:
    void listAvailableDevices();
    void disconnectDevice();
    void setInterfaceIndex(const qsizetype interfaceIndex);

private Q_SLOTS:
    void onErrorOccurred(QCanBusDevice::CanBusError);
    void onFramesReceived();
    void onFramesWritten(qint64 framesCount);
    void onStateChanged(QCanBusDevice::CanBusDeviceState state);
    void onTimerElapsed();

private:
    bool m_connected;
    QCanBusDevice *m_interface;
    qsizetype m_interfaceIndex;
    QStringList m_interfaceList;
    QList<QCanBusDeviceInfo> m_deviceInfo;
};

}
}