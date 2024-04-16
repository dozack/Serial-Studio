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

namespace DBC
{

class Loader : public QObject
{
    // clang-format off
    Q_OBJECT
    Q_PROPERTY(QString dbcFileName 
               READ dbcFileName
               NOTIFY dbcFileChanged)
    Q_PROPERTY(QString dbcFilePath 
               READ dbcFilePath
               NOTIFY dbcFileChanged)
    // clang-format on

Q_SIGNALS:
    void dbcFileChanged();

private:
    explicit Loader();
    Loader(Loader &&) = delete;
    Loader(const Loader &) = delete;
    Loader &operator=(Loader &&) = delete;
    Loader &operator=(const Loader &) = delete;

public:
    static Loader &instance();
    QString dbcFileName() const;
    QString dbcFilePath() const;
    QList<QCanMessageDescription> dbcContent() const;

public Q_SLOTS:
    void dbcFileLoad();
    void dbcFileLoad(const QString &path);

private Q_SLOTS:
    void onJsonFileMapChanged();

private:
    QList<QCanMessageDescription> m_dbcContent;
    QString m_dbcPath;
};

}