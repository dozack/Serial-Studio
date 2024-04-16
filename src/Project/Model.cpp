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

#include "Model.h"
#include "CodeEditor.h"

#include <AppInfo.h>
#include <IO/Manager.h>
#include <JSON/Generator.h>
#include <Misc/Utilities.h>

//
// For invalid group returns, avoids crashes while creating a new project & the UI
// is in the process of being updated
//
static JSON::Group EMPTY_GROUP;

//----------------------------------------------------------------------------------------
// Constructor/deconstructor & singleton
//----------------------------------------------------------------------------------------

/**
 * Constructor function. Initializes internal members and configures the signals/slots so
 * that the editor can know if the user modified the JSON document. Finally, the
 * constructor configures signals/slots with the JSON Generator to share the same JSON
 * document file.
 */
Project::Model::Model()
    : m_title("")
    , m_separator("")
    , m_frameParserCode("")
    , m_frameEndSequence("")
    , m_frameStartSequence("")
    , m_modified(false)
    , m_filePath("")
{
    // clang-format off

    // Connect signals/slots
    connect(this, &Project::Model::groupChanged,
            this, &Project::Model::onGroupChanged);
    connect(this, &Project::Model::titleChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::datasetChanged,
            this, &Project::Model::onDatasetChanged);
    connect(this, &Project::Model::separatorChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::groupCountChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::groupOrderChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::frameParserCodeChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::frameEndSequenceChanged,
            this, &Project::Model::onModelChanged);
    connect(this, &Project::Model::frameStartSequenceChanged,
            this, &Project::Model::onModelChanged);

    // Load current JSON map file into C++ model
    connect(&JSON::Generator::instance(), &JSON::Generator::jsonFileMapChanged,
            this, &Project::Model::onJsonLoaded);

    // clang-format on
}

/**
 * Returns a pointer to the only instance of the editor class.
 */
Project::Model &Project::Model::instance()
{
    static Model singleton;
    return singleton;
}

//----------------------------------------------------------------------------------------
// Member access functions
//----------------------------------------------------------------------------------------

/**
 * Returns a list with the available group-level widgets. This list is used by the user
 * interface to allow the user to build accelerometer, gyro & map widgets directly from
 * the UI.
 */
StringList Project::Model::availableGroupLevelWidgets()
{
    return StringList { tr("Dataset widgets"), tr("Accelerometer"), tr("Gyroscope"),
                        tr("GPS"), tr("Multiple data plot") };
}

/**
 * Returns a list with the available dataset-level widgets. This list is used by the user
 * interface to allow the user to build gauge, bar & compass widgets directly from the UI.
 */
StringList Project::Model::availableDatasetLevelWidgets()
{
    return StringList { tr("None"), tr("Gauge"), tr("Bar/level"), tr("Compass") };
}

/**
 * Returns the default path for saving JSON project files
 */
QString Project::Model::jsonProjectsPath() const
{
    // Get file name and path
    QString path = QString("%1/Documents/%2/JSON Projects/")
                       .arg(QDir::homePath(), qApp->applicationName());

    // Generate file path if required
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");

    return path;
}

/**
 * Returns the title of the current project
 */
QString Project::Model::title() const
{
    return m_title;
}

/**
 * Returns the data separator sequence for the current project.
 */
QString Project::Model::separator() const
{
    return m_separator;
}

/**
 * Returns the frame end sequence for the current project.
 */
QString Project::Model::frameEndSequence() const
{
    return m_frameEndSequence;
}

/**
 * Returns the frame start sequence for the current project.
 */
QString Project::Model::frameStartSequence() const
{
    return m_frameStartSequence;
}

/**
 * Returns @c true if the user modified the current project. This is
 * used to know if Serial Studio shall prompt the user to save his/her
 * modifications before closing the editor window.
 */
bool Project::Model::modified() const
{
    return m_modified;
}

/**
 * Returns the number of groups contained in the current JSON project.
 */
int Project::Model::groupCount() const
{
    return m_groups.count();
}

/**
 * Returns the full path of the current JSON project file.
 */
QString Project::Model::jsonFilePath() const
{
    return m_filePath;
}

/**
 * Returns the simplified file name of the current JSON project file.
 * This is used to change the title of the Editor window.
 */
QString Project::Model::jsonFileName() const
{
    if (!jsonFilePath().isEmpty())
    {
        auto fileInfo = QFileInfo(m_filePath);
        return fileInfo.fileName();
    }

    return tr("New Project");
}

/**
 * Checks if the current project has been modified and prompts the
 * user to save his/her changes.
 */
bool Project::Model::askSave()
{
    if (!modified())
        return true;

    auto ret = Misc::Utilities::showMessageBox(
        tr("Do you want to save your changes?"),
        tr("You have unsaved modifications in this project!"), APP_NAME,
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Cancel)
        return false;

    if (ret == QMessageBox::Discard)
        return true;

    return saveJsonFile();
}

/**
 * Validates the configuration of the current JSON project and saves the JSON
 * document on the hard disk.
 */
bool Project::Model::saveJsonFile()
{
    // Validate project title
    if (title().isEmpty())
    {
        Misc::Utilities::showMessageBox(tr("Project error"),
                                        tr("Project title cannot be empty!"));
        return false;
    }

    // Validate group titles
    for (int i = 0; i < groupCount(); ++i)
    {
        if (groupTitle(i).isEmpty())
        {
            Misc::Utilities::showMessageBox(tr("Project error - Group %1").arg(i + 1),
                                            tr("Group title cannot be empty!"));
            return false;
        }
    }

    // Validate dataset titles
    for (int i = 0; i < groupCount(); ++i)
    {
        for (int j = 0; j < datasetCount(i); ++j)
        {
            if (datasetTitle(i, j).isEmpty())
            {
                Misc::Utilities::showMessageBox(
                    tr("Project error - Group %1, Dataset %2").arg(i + 1).arg(j + 1),
                    tr("Dataset title cannot be empty!"));
                return false;
            }
        }
    }

    // Validate dataset indexes
    QVector<int> indexes;
    for (int i = 0; i < groupCount(); ++i)
    {
        for (int j = 0; j < datasetCount(i); ++j)
        {
            if (!indexes.contains(datasetIndex(i, j)))
                indexes.append(datasetIndex(i, j));

            else
            {
                auto ret = Misc::Utilities::showMessageBox(
                    tr("Warning - Group %1, Dataset %2").arg(i + 1).arg(j + 1),
                    tr("Dataset contains duplicate frame index position! Continue?"),
                    APP_NAME, QMessageBox::Yes | QMessageBox::No);

                if (ret == QMessageBox::No)
                    return false;
            }
        }
    }

    // Get file save path
    if (jsonFilePath().isEmpty())
    {
        auto path = QFileDialog::getSaveFileName(Q_NULLPTR, tr("Save JSON project"),
                                                 jsonProjectsPath(), "*.json");
        if (path.isEmpty())
            return false;

        m_filePath = path;
    }

    // Open file for writing
    QFile file(m_filePath);
    if (!file.open(QFile::WriteOnly))
    {
        Misc::Utilities::showMessageBox(tr("File open error"), file.errorString());
        return false;
    }

    // Create JSON document & add properties
    QJsonObject json;
    json.insert("title", title());
    json.insert("separator", separator());
    json.insert("frameEnd", frameEndSequence());
    json.insert("frameParser", frameParserCode());
    json.insert("frameStart", frameStartSequence());

    // Create group array
    QJsonArray groups;
    for (int i = 0; i < groupCount(); ++i)
    {
        // Create group
        QJsonObject group;
        group.insert("title", groupTitle(i));
        group.insert("widget", groupWidget(i));

        // Create dataset array
        QJsonArray datasets;
        for (int j = 0; j < datasetCount(i); ++j)
        {
            // Create dataset
            QJsonObject dataset;
            dataset.insert("led", datasetLED(i, j));
            dataset.insert("fft", datasetFftPlot(i, j));
            dataset.insert("log", datasetLogPlot(i, j));
            dataset.insert("title", datasetTitle(i, j));
            dataset.insert("tag", datasetTag(i, j));
            dataset.insert("units", datasetUnits(i, j));
            dataset.insert("graph", datasetGraph(i, j));
            dataset.insert("widget", datasetWidget(i, j));
            dataset.insert("min", datasetWidgetMin(i, j).toDouble());
            dataset.insert("max", datasetWidgetMax(i, j).toDouble());
            dataset.insert("alarm", datasetWidgetAlarm(i, j).toDouble());
            dataset.insert("fftSamples", datasetFFTSamples(i, j).toInt());
            dataset.insert("index", datasetIndex(i, j));
            dataset.insert("value", "");

            // Add dataset to array
            datasets.append(dataset);
        }

        // Add datasets to group
        group.insert("datasets", datasets);
        groups.append(group);
    }

    // Add groups array to JSON
    json.insert("groups", groups);

    // Write JSON data to file
    file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
    file.close();

    // Load JSON file to Serial Studio
    openJsonFile(file.fileName());
    JSON::Generator::instance().loadJsonMap(file.fileName());
    return true;
}

/**
 * Returns the number of datasets contained by the given @a group index.
 */
int Project::Model::datasetCount(const int group) const
{
    return getGroup(group).m_datasets.count();
}

/**
 * Returns a pointer to the group object positioned at the given @a index
 */
const JSON::Group &Project::Model::getGroup(const int index) const
{
    if (m_groups.count() > index)
        return m_groups.at(index);

    return EMPTY_GROUP;
}

/**
 * Returns a pointer to the dataset object contained by the @a group at
 * the given @a index
 */
const JSON::Dataset &Project::Model::getDataset(const int group, const int index) const
{
    return getGroup(group).getDataset(index);
}

/**
 * Returns the JavaScript code used to parse incoming frames
 */
QString Project::Model::frameParserCode() const
{
    return m_frameParserCode;
}

/**
 * Returns the title of the given @a group.
 */
QString Project::Model::groupTitle(const int group) const
{
    return getGroup(group).title();
}

/**
 * Returns the widget of the given @a group.
 */
QString Project::Model::groupWidget(const int group) const
{
    return getGroup(group).widget();
}

/**
 * Returns the widget ID of the given @a group. The widget ID is a number
 * that represents a group-level widget. The ID depends on the widget type
 * and the order of the widgets returned by the @c availableGroupLevelWidgets()
 * function.
 */
int Project::Model::groupWidgetIndex(const int group) const
{
    auto widget = groupWidget(group);

    if (widget == "accelerometer")
        return 1;

    if (widget == "gyro")
        return 2;

    if (widget == "map")
        return 3;

    if (widget == "multiplot")
        return 4;

    return 0;
}

/**
 * Returns the position in the frame that holds the value for the given
 * @a dataset (which is contained by the specified @a group).
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
int Project::Model::datasetIndex(const int group, const int dataset) const
{
    return getDataset(group, dataset).m_index;
}

/**
 * Returns @c true if Serial Studio should generate a LED with the given
 * @a dataset (which is contained by the specified @a group).
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
bool Project::Model::datasetLED(const int group, const int dataset) const
{
    return getDataset(group, dataset).led();
}

/**
 * Returns @c true if Serial Studio should graph the data of the given
 * @a dataset (which is contained by the specified @a group).
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
bool Project::Model::datasetGraph(const int group, const int dataset) const
{
    return getDataset(group, dataset).graph();
}

/**
 * Returns @c true if Serial Studio should graph the data of the given
 * @a dataset (which is contained by the specified @a group).
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
bool Project::Model::datasetFftPlot(const int group, const int dataset) const
{
    return getDataset(group, dataset).fft();
}

/**
 * Returns @c true if Serial Studio should graph the data of the given
 * @a dataset (which is contained by the specified @a group) with a
 * logarithmic scale.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
bool Project::Model::datasetLogPlot(const int group, const int dataset) const
{
    return getDataset(group, dataset).log();
}

/**
 * Returns the title of the specified dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetTitle(const int group, const int dataset) const
{
    return getDataset(group, dataset).title();
}

/**
 * Returns the tag of the specified dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetTag(const int group, const int dataset) const
{
    return getDataset(group, dataset).tag();
}

/**
 * Returns the measurement units of the specified dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetUnits(const int group, const int dataset) const
{
    return getDataset(group, dataset).units();
}

/**
 * Returns the widget string of the specified dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetWidget(const int group, const int dataset) const
{
    return getDataset(group, dataset).widget();
}

/**
 * Returns the widget ID of the specified dataset. The widget ID
 * corresponds to the list returned by the
 * @c availableDatasetLevelWidgets() function.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
int Project::Model::datasetWidgetIndex(const int group, const int dataset) const
{
    auto widget = datasetWidget(group, dataset);

    if (widget == "gauge")
        return 1;
    if (widget == "bar")
        return 2;
    if (widget == "compass")
        return 3;

    return 0;
}

/**
 * Returns the minimum widget value of the specified dataset.
 * This option is used by the bar & gauge widgets.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetWidgetMin(const int group, const int dataset) const
{
    return QString::number(getDataset(group, dataset).min());
}

/**
 * Returns the maximum widget value of the specified dataset.
 * This option is used by the bar & gauge widgets.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetWidgetMax(const int group, const int dataset) const
{
    return QString::number(getDataset(group, dataset).max());
}

/**
 * Returns the maximum FFT frequency value of the specified dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetFFTSamples(const int group, const int dataset) const
{
    return QString::number(getDataset(group, dataset).fftSamples());
}

/**
 * Returns the widget alarm value of the specified dataset.
 * This option is used by the bar & gauge widgets.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
QString Project::Model::datasetWidgetAlarm(const int group, const int dataset) const
{
    auto set = getDataset(group, dataset);

    if (set.alarm() <= set.min())
        return QString::number(set.max());

    return QString::number(set.alarm());
}

//----------------------------------------------------------------------------------------
// Public slots
//----------------------------------------------------------------------------------------

/**
 * Resets the C++ model used to represent the JSON project file.
 */
void Project::Model::newJsonFile()
{
    // Clear groups list
    m_groups.clear();

    // Reset project properties
    setTitle("");
    setSeparator("");
    setFrameParserCode("");
    setFrameEndSequence("");
    setFrameStartSequence("");

    // Update file path
    m_filePath = "";
    Q_EMIT jsonFileChanged();

    // Update UI
    Q_EMIT groupCountChanged();
    setModified(false);
}

/**
 * Prompts the user to select a JSON project file & generates the appropiate C++
 * model that represents the JSON document.
 */
void Project::Model::openJsonFile()
{
    // clang-format off
    auto path = QFileDialog::getOpenFileName(Q_NULLPTR,
                                             tr("Select JSON file"),
                                             jsonProjectsPath(),
                                             "*.json");
    // clang-format on

    // Invalid path, abort
    if (path.isEmpty())
        return;

    // Open the JSON file
    openJsonFile(path);
}

/**
 * Opens the JSON document at the given @a path & generates the appropiate C++
 * model that represents the JSON document.
 */
void Project::Model::openJsonFile(const QString &path)
{
    // Open file
    QFile file(path);
    QJsonDocument document;
    if (file.open(QFile::ReadOnly))
    {
        document = QJsonDocument::fromJson(file.readAll());
        file.close();
    }

    // Validate JSON document
    if (document.isEmpty())
        return;

    // Let the generator use the given JSON file
    if (JSON::Generator::instance().jsonMapFilepath() != path)
        JSON::Generator::instance().loadJsonMap(path);

    // Reset C++ model
    newJsonFile();

    // Update current JSON document
    m_filePath = path;
    Q_EMIT jsonFileChanged();

    // Read data from JSON document
    auto json = document.object();
    setTitle(json.value("title").toString());
    setSeparator(json.value("separator").toString());
    setFrameEndSequence(json.value("frameEnd").toString());
    setFrameParserCode(json.value("frameParser").toString());
    setFrameStartSequence(json.value("frameStart").toString());

    // Modify IO manager settings
    IO::Manager::instance().setSeparatorSequence(separator());
    IO::Manager::instance().setFinishSequence(frameEndSequence());
    IO::Manager::instance().setStartSequence(frameStartSequence());

    // Set JSON::Generator operation mode to manual
    JSON::Generator::instance().setOperationMode(JSON::Generator::kManual);

    // Read groups from JSON document
    auto groups = json.value("groups").toArray();
    for (int g = 0; g < groups.count(); ++g)
    {
        // Get JSON group data
        auto group = groups.at(g).toObject();

        // Register group with C++ model
        addGroup();
        setGroupTitle(g, group.value("title").toString());
        setGroupWidgetData(g, group.value("widget").toString());

        // Get JSON group datasets
        auto datasets = group.value("datasets").toArray();
        for (int d = 0; d < datasets.count(); ++d)
        {
            // Get dataset JSON data
            auto dataset = datasets.at(d).toObject();

            // Register dataset with C++ model
            addDataset(g);
            setDatasetLED(g, d, dataset.value("led").toBool());
            setDatasetFftPlot(g, d, dataset.value("fft").toBool());
            setDatasetLogPlot(g, d, dataset.value("log").toBool());
            setDatasetGraph(g, d, dataset.value("graph").toBool());
            setDatasetTitle(g, d, dataset.value("title").toString());
            setDatasetTag(g, d, dataset.value("tag").toString());
            setDatasetUnits(g, d, dataset.value("units").toString());
            setDatasetWidgetData(g, d, dataset.value("widget").toString());

            // Get max/min texts
            auto min = dataset.value("min").toDouble();
            auto max = dataset.value("max").toDouble();
            auto index = dataset.value("index").toInt();
            auto alarm = dataset.value("alarm").toDouble();
            auto fftSamples = dataset.value("fftSamples").toInt();
            setDatasetIndex(g, d, index);
            setDatasetWidgetMin(g, d, QString::number(min));
            setDatasetWidgetMax(g, d, QString::number(max));
            setDatasetWidgetAlarm(g, d, QString::number(alarm));
            ;
            setDatasetFFTSamples(g, d, QString::number(fftSamples));
        }
    }

    // Update UI
    Q_EMIT groupCountChanged();

    // Reset modified flag
    setModified(false);
}

/**
 * Changes the title of the JSON project file.
 */
void Project::Model::setTitle(const QString &title)
{
    if (title != m_title)
    {
        m_title = title;
        Q_EMIT titleChanged();
    }
}

/**
 * Changes the data separator sequence of the JSON project file.
 */
void Project::Model::setSeparator(const QString &separator)
{
    if (separator != m_separator)
    {
        m_separator = separator;
        Q_EMIT separatorChanged();
    }
}

/**
 * Updates the JavaScript code used to parse incoming frames
 */
void Project::Model::setFrameParserCode(const QString &code)
{
    if (code != m_frameParserCode)
    {
        // Update internal model
        m_frameParserCode = code;

        // Load default code if required
        if (m_frameParserCode.isEmpty())
            m_frameParserCode = CodeEditor::instance().defaultCode();

        // Update UI
        Q_EMIT frameParserCodeChanged();
    }
}

/**
 * Changes the frame end sequence of the JSON project file.
 */
void Project::Model::setFrameEndSequence(const QString &sequence)
{
    if (sequence != m_frameEndSequence)
    {
        m_frameEndSequence = sequence;
        Q_EMIT frameEndSequenceChanged();
    }
}

/**
 * Changes the frame start sequence of the JSON project file.
 */
void Project::Model::setFrameStartSequence(const QString &sequence)
{
    if (sequence != m_frameStartSequence)
    {
        m_frameStartSequence = sequence;
        Q_EMIT frameStartSequenceChanged();
    }
}

/**
 * Adds a new group to the C++ model that represents the JSON project file.
 */
void Project::Model::addGroup()
{
    m_groups.append(JSON::Group());
    setGroupTitle(m_groups.count() - 1, tr("New Group"));

    Q_EMIT groupCountChanged();
}

/**
 * Removes the given @a group from the C++ model that represents the JSON
 * project file.
 */
void Project::Model::deleteGroup(const int group)
{
    auto ret = Misc::Utilities::showMessageBox(
        tr("Delete group \"%1\"").arg(groupTitle(group)),
        tr("Are you sure you want to delete this group?"), APP_NAME,
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        m_groups.removeAt(group);
        Q_EMIT groupCountChanged();
    }
}

/**
 * Changes the position of the given @a group in the C++ model.
 */
void Project::Model::moveGroupUp(const int group)
{
    if (group > 0)
    {
        m_groups.move(group, group - 1);
        Q_EMIT groupOrderChanged();
    }
}

/**
 * Changes the position of the given @a group in the C++ model.
 */
void Project::Model::moveGroupDown(const int group)
{
    if (group < groupCount() - 1)
    {
        m_groups.move(group, group + 1);
        Q_EMIT groupOrderChanged();
    }
}

/**
 * Changes the group-level widget for the specified @a group.
 * If necessary, this function shall generate the appropiate datasets
 * needed to implement the widget (e.g. x,y,z for accelerometer widgets).
 */
bool Project::Model::setGroupWidget(const int group, const int widgetId)
{
    auto grp = getGroup(group);

    // Warn user if group contains existing datasets
    if (!(grp.m_datasets.isEmpty()) && widgetId != 4)
    {
        if (widgetId == 0 && grp.widget() == "multiplot")
            grp.m_widget = "";

        else
        {
            auto ret = Misc::Utilities::showMessageBox(
                tr("Are you sure you want to change the group-level widget?"),
                tr("Existing datasets for this group will be deleted"), APP_NAME,
                QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::No)
                return false;
            else
                grp.m_datasets.clear();
        }
    }

    // Accelerometer widget
    if (widgetId == 1)
    {
        // Set widget title
        grp.m_widget = "accelerometer";
        grp.m_title = tr("Accelerometer");

        // Create datasets
        JSON::Dataset x, y, z;

        // Set dataset indexes
        x.m_index = nextDatasetIndex();
        y.m_index = nextDatasetIndex() + 1;
        z.m_index = nextDatasetIndex() + 2;

        // Set measurement units
        x.m_units = "m/s²";
        y.m_units = "m/s²";
        z.m_units = "m/s²";

        // Set dataset properties
        x.m_widget = "x";
        y.m_widget = "y";
        z.m_widget = "z";
        x.m_title = tr("Accelerometer %1").arg("X");
        y.m_title = tr("Accelerometer %1").arg("Y");
        z.m_title = tr("Accelerometer %1").arg("Z");

        // Add datasets to group
        grp.m_datasets.append(x);
        grp.m_datasets.append(y);
        grp.m_datasets.append(z);
    }

    // Gyroscope widget
    else if (widgetId == 2)
    {
        // Set widget title
        grp.m_widget = "gyro";
        grp.m_title = tr("Gyroscope");

        // Create datasets
        JSON::Dataset x, y, z;

        // Set dataset indexes
        x.m_index = nextDatasetIndex();
        y.m_index = nextDatasetIndex() + 1;
        z.m_index = nextDatasetIndex() + 2;

        // Set measurement units
        x.m_units = "°";
        y.m_units = "°";
        z.m_units = "°";

        // Set dataset properties
        x.m_widget = "roll";
        y.m_widget = "pitch";
        z.m_widget = "yaw";
        x.m_title = tr("Gyro %1").arg("Roll");
        y.m_title = tr("Gyro %1").arg("Pitch");
        z.m_title = tr("Gyro %1").arg("Yaw");

        // Add datasets to group
        grp.m_datasets.append(x);
        grp.m_datasets.append(y);
        grp.m_datasets.append(z);
    }

    // Map widget
    else if (widgetId == 3)
    {
        // Set widget title
        grp.m_widget = "map";
        grp.m_title = tr("GPS");

        // Create datasets
        JSON::Dataset lat, lon, alt;

        // Set dataset indexes
        lat.m_index = nextDatasetIndex();
        lon.m_index = nextDatasetIndex() + 1;
        alt.m_index = nextDatasetIndex() + 2;

        // Set measurement units
        lat.m_units = "°";
        lon.m_units = "°";
        alt.m_units = "m";

        // Set dataset properties
        lat.m_widget = "lat";
        lon.m_widget = "lon";
        alt.m_widget = "alt";
        lat.m_title = tr("Latitude");
        lon.m_title = tr("Longitude");
        alt.m_title = tr("Altitude");

        // Add datasets to group
        grp.m_datasets.append(lat);
        grp.m_datasets.append(lon);
        grp.m_datasets.append(alt);
    }

    // Multi plot widget
    else if (widgetId == 4)
        grp.m_widget = "multiplot";

    // Replace previous group with new group
    m_groups.replace(group, grp);

    // Update UI
    Q_EMIT groupChanged(group);
    return true;
}

/**
 * Changes the @a title for the given @a group in the C++ model
 */
void Project::Model::setGroupTitle(const int group, const QString &title)
{
    // Get group
    auto grp = getGroup(group);

    // Change group values & update groups vector
    grp.m_title = title;
    m_groups.replace(group, grp);

    // Update UI
    Q_EMIT groupChanged(group);
}

/**
 * Changes the @a widget for the given @a group in the C++ model
 */
void Project::Model::setGroupWidgetData(const int group, const QString &widget)
{
    // Get group
    auto grp = getGroup(group);

    // Change group values & update groups vector
    grp.m_widget = widget;
    m_groups.replace(group, grp);

    // Update UI
    Q_EMIT groupChanged(group);
}

/**
 * Adds a new dataset to the given @a group & increments the frame
 * index counter (to avoid having datasets that pull data from the
 * same position in the MCU frame).
 */
void Project::Model::addDataset(const int group)
{
    // Get group
    auto grp = getGroup(group);

    // Change group values & update groups vector
    grp.m_datasets.append(JSON::Dataset());
    m_groups.replace(group, grp);

    // Set dataset title & index
    setDatasetIndex(group, grp.m_datasets.count() - 1, nextDatasetIndex());
    setDatasetTitle(group, grp.m_datasets.count() - 1, tr("New dataset"));

    // Update UI
    Q_EMIT groupChanged(group);
}

/**
 * Removes the given @a dataset from the specified @a group.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::deleteDataset(const int group, const int dataset)
{
    auto ret = Misc::Utilities::showMessageBox(
        tr("Delete dataset \"%1\"").arg(getDataset(group, dataset).title()),
        tr("Are you sure you want to delete this dataset?"), APP_NAME,
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        auto grp = getGroup(group);
        grp.m_datasets.removeAt(dataset);
        m_groups.replace(group, grp);
        Q_EMIT groupChanged(group);
    }
}

/**
 * Updates the @a title of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetTitle(const int group, const int dataset,
                                     const QString &title)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_title != title)
    {
        set.m_title = title;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a tag of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetTag(const int group, const int dataset, const QString &tag)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_tag != tag)
    {
        set.m_tag = tag;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the measurement @a units of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetUnits(const int group, const int dataset,
                                     const QString &units)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_units != units)
    {
        set.m_units = units;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a frameIndex of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetIndex(const int group, const int dataset,
                                     const int frameIndex)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_index != frameIndex)
    {
        set.m_index = frameIndex;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a generateLED flag of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetLED(const int group, const int dataset,
                                   const bool generateLED)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_led != generateLED)
    {
        set.m_led = generateLED;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a generateGraph flag of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetGraph(const int group, const int dataset,
                                     const bool generateGraph)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_graph != generateGraph)
    {
        set.m_graph = generateGraph;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a generateFft flag of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetFftPlot(const int group, const int dataset,
                                       const bool generateFft)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_fft != generateFft)
    {
        set.m_fft = generateFft;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a generateLog flag of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetLogPlot(const int group, const int dataset,
                                       const bool generateLog)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_log != generateLog)
    {
        set.m_log = generateLog;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a widgetId of the given @a dataset. The widget ID is dependent on
 * the order of the widgets returned by the @c availableDatasetLevelWidgets() function.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetWidget(const int group, const int dataset,
                                      const int widgetId)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Get widget string
    QString widget;
    if (widgetId == 1)
        widget = "gauge";
    else if (widgetId == 2)
        widget = "bar";
    else if (widgetId == 3)
    {
        widget = "compass";
        set.m_min = 0;
        set.m_max = 360;
    }

    // Update dataset & group
    if (set.m_widget != widget)
    {
        set.m_widget = widget;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a minimum value used by the bar & gauge widgets for the given
 * @a dataset. The value is specified in a @c QString to facilitate integration
 * with the QML user interface.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetWidgetMin(const int group, const int dataset,
                                         const QString &minimum)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_min != minimum.toDouble())
    {
        set.m_min = minimum.toDouble();
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a maximum value used by the bar & gauge widgets for the given
 * @a dataset. The value is specified in a @c QString to facilitate integration
 * with the QML user interface.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetWidgetMax(const int group, const int dataset,
                                         const QString &maximum)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_max != maximum.toDouble())
    {
        set.m_max = maximum.toDouble();
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a widget string of the given @a dataset.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetWidgetData(const int group, const int dataset,
                                          const QString &widget)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_widget != widget)
    {
        set.m_widget = widget;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a alarm value used by the bar & gauge widgets for the given
 * @a dataset. The value is specified in a @c QString to facilitate integration
 * with the QML user interface.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetWidgetAlarm(const int group, const int dataset,
                                           const QString &alarm)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Update dataset & group
    if (set.m_alarm != alarm.toDouble())
    {
        set.m_alarm = alarm.toDouble();
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a samples used for FFT plotting.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::setDatasetFFTSamples(const int group, const int dataset,
                                          const QString &samples)
{
    // Get dataset & group
    auto grp = getGroup(group);
    auto set = getDataset(group, dataset);

    // Validate FFT samples
    auto sample = samples.toInt();
    if (sample < 128)
        sample = 128;

    // Update dataset & group
    if (set.m_fftSamples != sample)
    {
        set.m_fftSamples = sample;
        grp.m_datasets.replace(dataset, set);
        m_groups.replace(group, grp);

        // Update UI
        Q_EMIT datasetChanged(group, dataset);
    }
}

/**
 * Updates the @a modified flag of the current JSON project.
 * This flag is used to know if we should ask the user to save
 * his/her modifications to the project file.
 */
void Project::Model::setModified(const bool modified)
{
    m_modified = modified;
    Q_EMIT modifiedChanged();
}

/**
 * Ensures that the JSON project file is the same as the one used by
 * the JSON Generator class.
 */
void Project::Model::onJsonLoaded()
{
    if (jsonFilePath() != JSON::Generator::instance().jsonMapFilepath())
        openJsonFile(JSON::Generator::instance().jsonMapFilepath());
}

/**
 * Sets the modified flag to @c true when the user adds/removes/moves
 * one of the groups contained in the JSON project.
 */
void Project::Model::onModelChanged()
{
    setModified(true);
}

/**
 * Sets the modified flag to @c true when the user changes the title
 * or the widget of one of the groups contained in the JSON project.
 */
void Project::Model::onGroupChanged(const int group)
{
    (void)group;
    setModified(true);
}

/**
 * Sets the modified flag to @c true when the user modifies the
 * properties of one of the datasets contained in the JSON project.
 *
 * @param group   index of the group in which the dataset belongs
 * @param dataset index of the dataset
 */
void Project::Model::onDatasetChanged(const int group, const int dataset)
{
    (void)group;
    (void)dataset;
    setModified(true);
}

/**
 * Gets the number of datasets registered in the projects and
 * adds 1 to the result.
 *
 * This function is used when registering new datasets, so that
 * the user does not need to manually specify dataset indexes.
 *
 * @param tag tag linked to dataset
 */
int Project::Model::nextDatasetIndex(const QString &tag)
{
    int maxIndex = 1;
    for (int i = 0; i < groupCount(); ++i)
    {
        for (int j = 0; j < datasetCount(i); ++j)
        {
            auto dataset = getDataset(i, j);
            if ((dataset.m_tag == tag) && (dataset.m_index >= maxIndex))
            {
                maxIndex = dataset.m_index + 1;
            }
        }
    }

    return maxIndex;
}

#ifdef SERIAL_STUDIO_INCLUDE_MOC
#    include "moc_Model.cpp"
#endif
