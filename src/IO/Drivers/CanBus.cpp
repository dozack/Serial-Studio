#include <IO/Drivers/CanBus.h>

#include <QCanBus>

IO::Drivers::CanBus::CanBus()
    : m_supported(isSupported())
{
}

IO::Drivers::CanBus &IO::Drivers::CanBus::instance()
{
    static CanBus singleton;
    return singleton;
}

void IO::Drivers::CanBus::close() { }

bool IO::Drivers::CanBus::isOpen() const
{
    return 0;
}

bool IO::Drivers::CanBus::isReadable() const
{
    return 0;
}

bool IO::Drivers::CanBus::isWritable() const
{
    return 0;
}

bool IO::Drivers::CanBus::configurationOk() const
{
    return 0;
}

quint64 IO::Drivers::CanBus::write(const QByteArray &data)
{
    return 0;
}

bool IO::Drivers::CanBus::open(const QIODevice::OpenMode mode)
{
    return 0;
}

bool IO::Drivers::CanBus::isSupported() const
{
    return QCanBus::instance()->plugins().contains(QStringLiteral("socketcan"));
}

QString IO::Drivers::CanBus::interfaceName() const
{
    return QString("");
}

QCanBusDevice *IO::Drivers::CanBus::interface() const
{
    return nullptr;
}

quint8 IO::Drivers::CanBus::interfaceIndex() const
{
    return 0;
}

StringList IO::Drivers::CanBus::interfaceList() const
{
    return StringList();
}

void IO::Drivers::CanBus::setInterfaceIndex(const quint8 portIndex) { }

void IO::Drivers::CanBus::onReadyRead() { }

void IO::Drivers::CanBus::readSettings() { }

void IO::Drivers::CanBus::writeSettings() { }

void IO::Drivers::CanBus::refreshInterfaceList() { }

void IO::Drivers::CanBus::onErrorOccurred(const QCanBusDevice::CanBusError err) { }