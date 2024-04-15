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
#include <IO/Manager.h>

#include <Misc/Utilities.h>

DBC::Loader::Loader() { }

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

    if (!file.isEmpty())
    {
        dbcFileLoad(file);
    }
}

void DBC::Loader::dbcFileLoad(const QString &path)
{
    if (path == m_dbcPath)
    {
        return;
    }

    if (!m_dbcPath.isEmpty())
    {
        m_dbcPath = QString();
    }

    auto parser = QCanDbcFileParser();

    if (!parser.parse(path))
    {
        Misc::Utilities::showMessageBox(tr("DBC parse error"), parser.errorString());
        return;
    }

    auto warnings = parser.warnings();

    if (!warnings.isEmpty())
    {
        for (auto warning : warnings)
        {
            qWarning().noquote() << warning;
        }
    }

    m_dbcPath = path;

    Q_EMIT dbcFileChanged();
}
