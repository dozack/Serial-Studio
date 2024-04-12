#include <IO/Manager.h>
#include <IO/Drivers/CanBus.h>
#include <Misc/TimerEvents.h>

#include <QCanBus>

#define PLUGIN_NAME (QStringLiteral("socketcan"))

IO::Drivers::CanBus::CanBus()
    : m_connected(false)
    , m_interface(Q_NULLPTR)
    , m_interfaceIndex(0)
{
    if (isSupported())
    {
        StringList interfaces;
        interfaces.append(tr("Select interface"));

        auto validInterfaceList = validInterfaces();
        Q_FOREACH (QCanBusDeviceInfo info, validInterfaceList)
        {
            interfaces.append(info.name());
        }

        m_interfaceList = interfaces;
    }

    /* clang-format off */
    connect(&Misc::TimerEvents::instance(), &Misc::TimerEvents::timeout1Hz,
            this, &IO::Drivers::CanBus::onTimerElapsed);
    connect(this, &IO::Drivers::CanBus::interfaceIndexChanged, 
            this, &IO::Drivers::CanBus::onInterfaceIndexChanged);
    connect(this, &IO::Drivers::CanBus::interfaceIndexChanged, 
            this, &IO::Drivers::CanBus::configurationChanged);
    /* clang-format on */
}

IO::Drivers::CanBus::~CanBus()
{
    if (interface())
    {
        disconnectDevice();
    }
}

IO::Drivers::CanBus &IO::Drivers::CanBus::instance()
{
    static CanBus singleton;
    return singleton;
}

void IO::Drivers::CanBus::close()
{
    if (isOpen())
    {
        interface()->disconnectDevice();
    }
}

bool IO::Drivers::CanBus::isOpen() const
{
    if (interface())
    {
        return (m_connected);
    }

    return (false);
}

bool IO::Drivers::CanBus::isReadable() const
{
    if (isOpen())
    {
        return (interface()->framesAvailable() > 0);
    }

    return (false);
}

bool IO::Drivers::CanBus::isWritable() const
{
    if (isOpen())
    {
        return (true);
    }

    return (false);
}

bool IO::Drivers::CanBus::configurationOk() const
{
    return (interfaceIndex() > 0);
}

quint64 IO::Drivers::CanBus::write(const QByteArray &data)
{
    return (data.length());
}

bool IO::Drivers::CanBus::open(const QIODevice::OpenMode mode)
{
    Q_UNUSED(mode);

    auto interfaces = validInterfaces();
    auto interfaceId = interfaceIndex() - 1;

    if ((interfaceId >= 0) && (interfaceId < validInterfaces().count()))
    {
        disconnectDevice();

        m_interfaceIndex = (interfaceId + 1);
        Q_EMIT interfaceIndexChanged();

        m_interface = QCanBus::instance()->createDevice(PLUGIN_NAME, m_interfaceName);

        if (interface())
        {
            /* clang-format off */
            connect(interface(), &QCanBusDevice::errorOccurred, 
                    this,&IO::Drivers::CanBus::onErrorOccurred);
            connect(interface(), &QCanBusDevice::framesReceived, 
                    this,&IO::Drivers::CanBus::onFramesReceived);
            connect(interface(), &QCanBusDevice::framesWritten, 
                    this,&IO::Drivers::CanBus::onFramesWritten);
            connect(interface(), &QCanBusDevice::stateChanged, 
                    this,&IO::Drivers::CanBus::onStateChanged);
            /* clang-format on */

            auto configurationKeys = interface()->configurationKeys();
            Q_FOREACH (QCanBusDevice::ConfigurationKey key, configurationKeys)
            {
                interface()->setConfigurationParameter(key, QVariant());
            }

            m_connected = interface()->connectDevice();
        }

        return (m_connected);
    }

    disconnectDevice();
    return (false);
}

bool IO::Drivers::CanBus::isSupported() const
{
    return (QCanBus::instance()->plugins().contains(PLUGIN_NAME));
}

QString IO::Drivers::CanBus::interfaceName() const
{
    if (interface())
    {
        return (interface()->deviceInfo().name());
    }

    return (tr("No Device"));
}

QCanBusDevice *IO::Drivers::CanBus::interface() const
{
    return (m_interface);
}

quint8 IO::Drivers::CanBus::interfaceIndex() const
{
    return (m_interfaceIndex);
}

StringList IO::Drivers::CanBus::interfaceList() const
{
    return (m_interfaceList);
}

void IO::Drivers::CanBus::disconnectDevice()
{
    if (interface())
    {
        interface()->disconnect(this);
        interface()->disconnectDevice();
        interface()->deleteLater();
    }

    m_interface = Q_NULLPTR;
    m_connected = false;
    Q_EMIT availableInterfacesChanged();
}

void IO::Drivers::CanBus::setInterfaceIndex(const quint8 interfaceIndex)
{
    auto interfaceId = (interfaceIndex - 1);

    if ((interfaceId >= 0) && (interfaceId < validInterfaces().count()))
    {
        m_interfaceIndex = interfaceIndex;
    }
    else
    {
        m_interfaceIndex = 0;
    }

    Q_EMIT interfaceIndexChanged();
}

void IO::Drivers::CanBus::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    Q_UNUSED(error)
}

#include <iostream>

void IO::Drivers::CanBus::onFramesReceived()
{
    Q_ASSERT(interface() != Q_NULLPTR);

    while (interface()->framesAvailable() > 0)
    {
        QCanBusFrame frame = interface()->readFrame();

        qInfo().noquote() << frame.toString();
    }
}

void IO::Drivers::CanBus::onFramesWritten(qint64 framesCount)
{
    Q_UNUSED(framesCount)
}

void IO::Drivers::CanBus::onStateChanged(QCanBusDevice::CanBusDeviceState state)
{
    Q_UNUSED(state)
}

void IO::Drivers::CanBus::onInterfaceIndexChanged()
{
    m_interfaceName = interfaceList().at(interfaceIndex());

    Q_EMIT interfaceNameChanged();
}

void IO::Drivers::CanBus::onTimerElapsed()
{
    StringList interfaces;
    interfaces.append(tr("Select interface"));

    auto validInterfaceList = validInterfaces();
    Q_FOREACH (QCanBusDeviceInfo info, validInterfaceList)
    {
        interfaces.append(info.name());
    }

    if (interfaceList() != interfaces)
    {
        m_interfaceList = interfaces;

        if (interface())
        {
            auto name = interface()->deviceInfo().name();
            for (int i = 0; i < validInterfaceList.count(); i++)
            {
                if (validInterfaceList.at(i).name() == name)
                {
                    m_interfaceIndex = (i + 1);
                    break;
                }
            }
        }
    }

    Q_EMIT availableInterfacesChanged();
}

QVector<QCanBusDeviceInfo> IO::Drivers::CanBus::validInterfaces() const
{
    return (QCanBus::instance()->availableDevices(PLUGIN_NAME));
}

#ifdef SERIAL_STUDIO_INCLUDE_MOC
#    include "moc_CanBus.cpp"
#endif