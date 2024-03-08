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

#include <DataTypes.h>
#include <IO/HAL_Driver.h>

#include <QCanBusDevice>

namespace IO
{
namespace Drivers
{

class CanBus : public HAL_Driver
{
    // clang-format off
    Q_OBJECT
    Q_PROPERTY(QString interfaceName
               READ interfaceName
               NOTIFY interfaceNameChanged)
    Q_PROPERTY(quint8 interfaceIndex
               READ interfaceIndex
               WRITE setInterfaceIndex
               NOTIFY interfaceIndexChanged)
    Q_PROPERTY(StringList interfaceList 
               READ interfaceList 
               NOTIFY availableInterfacesChanged)
    Q_PROPERTY(bool isSupported 
               READ isSupported 
               CONSTANT)
    // clang-format on

Q_SIGNALS:
    void interfaceNameChanged();
    void interfaceIndexChanged();
    void availableInterfacesChanged();

private:
    explicit CanBus();
    CanBus(CanBus &&) = delete;
    CanBus(const CanBus &) = delete;
    CanBus &operator=(CanBus &&) = delete;
    CanBus &operator=(const CanBus &) = delete;

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

    quint8 interfaceIndex() const;

    StringList interfaceList() const;

public Q_SLOTS:
    void setInterfaceIndex(const quint8 portIndex);

private Q_SLOTS:
    void onReadyRead();
    void readSettings();
    void writeSettings();
    void refreshInterfaceList();
    void onErrorOccurred(const QCanBusDevice::CanBusError err);

private:
    QVector<QCanBusDeviceInfo> validInterfaces() const;

private:
    const bool m_supported;

    QCanBusDevice *m_device;

    quint8 m_interfaceIndex;

    QStringList m_interfaceList;
};

}
}