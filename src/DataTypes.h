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

#include <QAbstractButton>
#include <QAbstractSocket>
#include <QApplication>
#include <QBasicTimer>
#include <QByteArray>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanDbcFileParser>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QHostInfo>
#include <QIODevice>
#include <QJsonObject>
#include <QLocale>
#include <QMessageBox>
#include <QObject>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcess>
#include <QPushButton>
#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QSettings>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QTimerEvent>
#include <QToolBar>
#include <QTranslator>
#include <QUdpSocket>
#include <QUrl>
#include <QVBoxLayout>
#include <QVariant>
#include <QVector>
#include <QtCore>
#include <QtMath>
#include <QtSerialPort>

#include <QJSEngine>
#include <QJSValue>

#ifdef QT_QML_LIB
#    include <QtQml>
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#    include <QStringList>
#    define StringList QStringList
#else
#    define StringList QVector<QString>
#endif

#define PlotData QVector<double>
