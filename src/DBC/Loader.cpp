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

#include "Loader.h"

#include <Project/Model.h>
#include <JSON/Generator.h>
#include <IO/Drivers/CanBus.h>

#include <Misc/Utilities.h>

DBC::Loader::Loader()
{
    auto path = QSettings().value("can_dbc_location", "").toString();

    dbcFileLoad(path);

    connect(&JSON::Generator::instance(), &JSON::Generator::jsonFileMapChanged, this,
            &DBC::Loader::onJsonFileMapChanged);

    onJsonFileMapChanged();
}

DBC::Loader &DBC::Loader::instance()
{
    static Loader singleton;
    return singleton;
}

QString DBC::Loader::dbcFileName() const
{
    if (!m_dbcPath.isEmpty())
    {
        auto fileInfo = QFileInfo(m_dbcPath);
        return fileInfo.fileName();
    }

    return "";
}

QString DBC::Loader::dbcFilePath() const
{
    if (!m_dbcPath.isEmpty())
    {
        auto fileInfo = QFileInfo(m_dbcPath);
        return fileInfo.filePath();
    }

    return "";
}

void DBC::Loader::dbcFileLoad()
{
    // clang-format off
    auto file = QFileDialog::getOpenFileName(Q_NULLPTR, 
                                             tr("Select DBC file"), 
                                             Project::Model::instance().jsonProjectsPath(),
                                             tr("DBC files") + " (*.dbc)");
    // clang-format on

    if (!file.isEmpty() && (file != m_dbcPath))
    {
        dbcFileLoad(file);
    }
}

void DBC::Loader::dbcFileLoad(const QString &path)
{
    if (path.isEmpty() || (path == m_dbcPath))
    {
        return;
    }

    if (!m_dbcPath.isEmpty())
    {
        m_dbcPath = QString();

        QSettings().setValue("can_dbc_location", m_dbcPath);

        Q_EMIT dbcFileChanged();
    }

    auto parser = QCanDbcFileParser();

    if (!parser.parse(path))
    {
        Misc::Utilities::showMessageBox(tr("DBC parse error"), parser.errorString());
        return;
    }

    Q_FOREACH (auto warning, parser.warnings())
    {
        qWarning().noquote() << warning;
    }

    m_dbcContent = parser.messageDescriptions();
    m_dbcPath = path;

    QSettings().setValue("can_dbc_location", m_dbcPath);

    Q_EMIT dbcFileChanged();
}

void DBC::Loader::onJsonFileMapChanged()
{
    auto dbcNew = QList<QCanMessageDescription>();
    auto json = JSON::Generator::instance().json();
    auto added = QStringList();

    auto groups = json.value("groups").toArray();
    Q_FOREACH (auto group, groups)
    {
        auto datasets = group.toObject().value("datasets").toArray();
        Q_FOREACH (auto dataset, datasets)
        {
            auto tag = dataset.toObject().value("tag").toString();
            if (!added.contains(tag))
            {
                Q_FOREACH (auto messageDesc, m_dbcContent)
                {
                    auto id = static_cast<quint32>(messageDesc.uniqueId());
                    auto newTag = QString("0x%1").arg(id, 0, 16);

                    if (tag == newTag)
                    {
                        dbcNew.append(messageDesc);
                        added.append(newTag);
                    }
                }
            }
        }
    }

    IO::Drivers::CanBus::instance().setFrameProcessor(dbcNew);
}