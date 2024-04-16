#include <IO/Manager.h>
#include <IO/Drivers/CanBus.h>
#include <Misc/TimerEvents.h>

#define SUPPORTED_PLUGINS                                                                \
    {                                                                                    \
        "socketcan"                                                                      \
    }

IO::Drivers::CanBus::CanBus()
{
    listAvailableDevices();

    /* clang-format off */
    connect(&Misc::TimerEvents::instance(), &Misc::TimerEvents::timeout1Hz,
            this, &IO::Drivers::CanBus::onTimerElapsed);
    connect(this, &IO::Drivers::CanBus::interfaceIndexChanged, 
            this, &IO::Drivers::CanBus::configurationChanged);
    /* clang-format on */
}

IO::Drivers::CanBus::~CanBus()
{
    close();
    disconnectDevice();
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

        m_connected = false;

        connect(&Misc::TimerEvents::instance(), &Misc::TimerEvents::timeout1Hz, this,
                &IO::Drivers::CanBus::onTimerElapsed);
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

    close();
    disconnectDevice();

    auto info = m_deviceInfo.at(interfaceIndex() - 1);

    m_interface = QCanBus::instance()->createDevice(info.plugin(), info.name());

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

        if (m_connected)
        {
            disconnect(this, SLOT(onTimerElapsed()));
        }
    }

    return (m_connected);
}

bool IO::Drivers::CanBus::isSupported() const
{
    return true;
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

qsizetype IO::Drivers::CanBus::interfaceIndex() const
{
    return (m_interfaceIndex);
}

QStringList IO::Drivers::CanBus::availableDevices() const
{
    auto deviceList = QStringList(tr("Select device"));

    deviceList.append(availableDeviceNames());

    return deviceList;
}

QStringList IO::Drivers::CanBus::availableDeviceNames() const
{
    auto deviceNames = QStringList();

    Q_FOREACH (QCanBusDeviceInfo deviceInfo, m_deviceInfo)
    {
        deviceNames.append(deviceInfo.name());
    }

    deviceNames.sort();

    return deviceNames;
}

void IO::Drivers::CanBus::listAvailableDevices()
{
    auto newDeviceInfo = QList<QCanBusDeviceInfo>();
    auto newDeviceNames = QStringList();
    auto pluginNames = QCanBus::instance()->plugins();

    pluginNames.removeIf([](QString pluginName) {
        return !QStringList(SUPPORTED_PLUGINS).contains(pluginName);
    });

    Q_FOREACH (auto pluginName, pluginNames)
    {
        auto deviceInfos = QCanBus::instance()->availableDevices(pluginName);

        Q_FOREACH (auto deviceInfo, deviceInfos)
        {
            newDeviceInfo.append(deviceInfo);
            newDeviceNames.append(deviceInfo.name());
        }
    }

    newDeviceNames.sort();

    if (newDeviceNames != availableDeviceNames())
    {
        m_deviceInfo = newDeviceInfo;

        Q_EMIT availableDevicesChanged();
    }
}

void IO::Drivers::CanBus::disconnectDevice()
{
    if (interface())
    {
        interface()->disconnect(this);

        delete m_interface;
    }

    m_interface = Q_NULLPTR;
}

void IO::Drivers::CanBus::setInterfaceIndex(const qsizetype interfaceIndex)
{
    m_interfaceIndex = interfaceIndex;

    Q_EMIT interfaceIndexChanged();
}

void IO::Drivers::CanBus::onErrorOccurred(QCanBusDevice::CanBusError error)
{
    Q_UNUSED(error)
}

void IO::Drivers::CanBus::onFramesReceived()
{
    Q_ASSERT(interface());

    auto packet = QByteArray();
    auto stream = QDataStream(&packet, QIODevice::WriteOnly);

    while (interface()->framesAvailable() > 0)
    {
        stream << interface()->readFrame();
    }

    if (!packet.isEmpty())
    {
        Q_EMIT dataReceived(packet);
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

void IO::Drivers::CanBus::onTimerElapsed()
{
    if (!m_connected)
    {
        listAvailableDevices();
    }
}

#ifdef SERIAL_STUDIO_INCLUDE_MOC
#    include "moc_CanBus.cpp"
#endif