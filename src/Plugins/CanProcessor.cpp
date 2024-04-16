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

#include <IO/Manager.h>
#include <IO/Drivers/CanBus.h>
#include <JSON/Generator.h>
#include <Plugins/CanProcessor.h>
#include <Project/Model.h>

Plugins::CanProcessor::CanProcessor()
{
    connect(&DBC::Loader::instance(), &DBC::Loader::dbcFileChanged, this,
            &Plugins::CanProcessor::onDatabaseChanged);
    connect(&JSON::Generator::instance(), &JSON::Generator::jsonFileMapChanged, this,
            &Plugins::CanProcessor::onSignalMappingChanged);
    connect(&IO::Manager::instance(), &IO::Manager::connectedChanged, this,
            &Plugins::CanProcessor::onConnectionChanged);

    onSignalMappingChanged();
    onDatabaseChanged();

    m_processor.setUniqueIdDescription(QCanDbcFileParser::uniqueIdDescription());
}

Plugins::CanProcessor::~CanProcessor() { }

Plugins::CanProcessor &Plugins::CanProcessor::instance()
{
    static CanProcessor singleton;
    return singleton;
}

void Plugins::CanProcessor::onConnectionChanged()
{
    auto io = &IO::Manager::instance();

    if (io->connected() && (io->selectedDriver() == IO::Manager::SelectedDriver::CanBus))
    {
        connect(&IO::Drivers::CanBus::instance(), &IO::Drivers::CanBus::dataReceived,
                this, &Plugins::CanProcessor::onDataReceived);
    }
    else
    {
        disconnect(&IO::Drivers::CanBus::instance(), &IO::Drivers::CanBus::dataReceived,
                   this, &Plugins::CanProcessor::onDataReceived);
    }
}

void Plugins::CanProcessor::onDatabaseChanged()
{
    m_processor.setMessageDescriptions(DBC::Loader::instance().dbcContent());
}

void Plugins::CanProcessor::onSignalMappingChanged()
{
    m_indexMap = QMap<QString, int>();
    m_index = QList<QString>();

    auto json = JSON::Generator::instance().json();
    auto groups = json.value("groups").toArray();
    Q_FOREACH (auto group, groups)
    {
        auto datasets = group.toObject().value("datasets").toArray();
        Q_FOREACH (auto dataset, datasets)
        {
            auto dataObject = dataset.toObject();
            auto dataTitle = dataObject.value("title").toString();
            auto dataIndex = dataObject.value("index").toInt();

            m_indexMap.insert(dataTitle, dataIndex);
            m_signalValues.insert(dataTitle, QVariant(0));

            if (dataIndex >= m_index.size())
            {
                m_index.resize((dataIndex + 1));
            }

            m_index.replace(dataIndex, dataTitle);
        }
    }
}

void Plugins::CanProcessor::onDataReceived(const QByteArray &data)
{
    auto output = QString();
    auto reader = QDataStream(data);

    while (!reader.atEnd() && (reader.status() == QDataStream::Status::Ok))
    {
        QCanBusFrame canFrame;
        reader >> canFrame;

        auto values = m_processor.parseFrame(canFrame).signalValues;

        for (auto [key, value] : values.asKeyValueRange())
        {
            if (m_signalValues.contains(key))
            {
                m_signalValues.insert(key, value);
            }
        }
    }

    Q_FOREACH (auto signalName, m_index)
    {
        if (!m_signalValues.contains(signalName))
        {
            continue;
        }

        auto signalValue = m_signalValues.value(signalName);

        output.append(signalValue.toString());

        if (!m_index.endsWith(signalName))
        {
            output.append(IO::Manager::instance().separatorSequence());
        }
    }

    IO::Manager::instance().processPayload(output.toUtf8());
}

#ifdef SERIAL_STUDIO_INCLUDE_MOC
#    include "moc_CanProcessor.cpp"
#endif