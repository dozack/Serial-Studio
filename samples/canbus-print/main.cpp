
#include <IO/Drivers/CanBus.h>
#include <Misc/TimerEvents.h>

#include <QApplication>

#include <thread>
#include <chrono>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    auto can = &IO::Drivers::CanBus::instance();

    Misc::TimerEvents::instance().startTimers();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (can->isSupported())
    {
        can->setInterfaceIndex(1);
        can->open(QIODevice::ReadOnly);
    }

    return app.exec();
}