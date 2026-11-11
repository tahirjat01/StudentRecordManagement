#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QDoubleValidator>
#include <QRegularExpression>
#include <QStatusBar>
#include <QCloseEvent>
#include <QVector>
#include <QHeaderView>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QFileInfo>
#include <QCoreApplication>

struct Student {
    QString name, rollNumber, fatherName, caste, district;
    double gpa = 0.0;
};

class StudentWindow final : public QMainWindow {
    Q_OBJECT
public:
    StudentWindow() {
        setWindowTitle("Student Record Management System");
        resize(900, 650);
        buildInterface();
        dataFile = QDir(QCoreApplication::applicationDirPath()).filePath("students.csv");
        loadRecords();
        setAddMode();
        statusBar()->showMessage("Ready. Data file: " + QDir::toNativeSeparators(dataFile));
    }

private:
    QLineEdit *nameEdit{}, *rollEdit{}, *fatherEdit{}, *casteEdit{}, *districtEdit{}, *gpaEdit{};
    QPushButton *addButton{}, *updateButton{}, *deleteButton{}, *nextButton{}, *newButton{};
    QLabel *modeLabel{};
    QTableWidget *table{};
    QVector<Student> students;
    QString dataFile;
    int currentIndex = -1;
    bool browseMode = false;

    void buildInterface() {
        auto *central = new QWidget(this);
        auto *root = new QVBoxLayout(central);
        auto *heading = new QLabel("STUDENT RECORD MANAGER");
        heading->setObjectName("heading");
        heading->setAlignment(Qt::AlignCenter);
        root->addWidget(heading);

        auto *formBox = new QGroupBox("Student Information");
        auto *form = new QFormLayout(formBox);
        nameEdit = new QLineEdit; nameEdit->setPlaceholderText("Enter student's full name");
        rollEdit = new QLineEdit; rollEdit->setPlaceholderText("Unique roll number");
        fatherEdit = new QLineEdit; fatherEdit->setPlaceholderText("Enter father's name");
        casteEdit = new QLineEdit; casteEdit->setPlaceholderText("Enter caste");
        districtEdit = new QLineEdit; districtEdit->setPlaceholderText("Enter district");
        gpaEdit = new QLineEdit; gpaEdit->setPlaceholderText("0.00 to 4.00");
        auto *gpaValidator = new QDoubleValidator(0.0, 4.0, 2, gpaEdit);
        gpaValidator->setNotation(QDoubleValidator::StandardNotation);
        gpaEdit->setValidator(gpaValidator);
        for (auto *edit : {nameEdit, rollEdit, fatherEdit, casteEdit, districtEdit, gpaEdit}) {
            edit->setMinimumHeight(36);
            edit->setClearButtonEnabled(true);
        }
        form->addRow("Student Name *", nameEdit);
        form->addRow("Roll Number *", rollEdit);
        form->addRow("Father's Name *", fatherEdit);
        form->addRow("Caste *", casteEdit);
        form->addRow("District *", districtEdit);
        form->addRow("GPA (0–4) *", gpaEdit);
        root->addWidget(formBox);

        auto *actions = new QHBoxLayout;
        addButton = new QPushButton("Add Record");
        updateButton = new QPushButton("Update");
        deleteButton = new QPushButton("Delete");
        nextButton = new QPushButton("Next Record");
        newButton = new QPushButton("New / Add Mode");
        for (auto *b : {addButton, updateButton, deleteButton, nextButton, newButton}) {
            b->setMinimumHeight(42);
            actions->addWidget(b);
        }
        root->addLayout(actions);

        modeLabel = new QLabel("Mode: Add a new student");
        modeLabel->setObjectName("modeLabel");
        root->addWidget(modeLabel);

        table = new QTableWidget(0, 6);
        table->setHorizontalHeaderLabels({"Name", "Roll Number", "Father's Name", "Caste", "District", "GPA"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        root->addWidget(table);

        auto *hint = new QLabel("Tip: Roll Number must be unique. Use “New / Add Mode” to enter another student.");
        hint->setWordWrap(true);
        hint->setStyleSheet("color:#536579;");
        root->addWidget(hint);

        setCentralWidget(central);
        setStyleSheet(
            "QMainWindow{background:#f3f6fb;} QLabel#heading{font-size:23px;font-weight:700;color:#17365d;padding:12px;}"
            "QGroupBox{font-weight:600;border:1px solid #cbd5e1;border-radius:8px;margin-top:10px;padding:12px;background:white;}"
            "QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 5px;color:#24476b;}"
            "QLineEdit{border:1px solid #cbd5e1;border-radius:5px;padding:7px;background:#fff;}"
            "QPushButton{border:0;border-radius:6px;padding:8px 12px;background:#245b91;color:white;font-weight:600;}"
            "QPushButton:hover{background:#17436f;} QPushButton:disabled{background:#aeb9c5;color:#eef2f6;}"
            "QLabel#modeLabel{font-weight:600;color:#245b91;padding:6px;}"
            "QTableWidget{background:white;border:1px solid #cbd5e1;}"
        );

        connect(addButton, &QPushButton::clicked, this, &StudentWindow::addStudent);
        connect(updateButton, &QPushButton::clicked, this, &StudentWindow::updateStudent);
        connect(deleteButton, &QPushButton::clicked, this, &StudentWindow::deleteStudent);
        connect(nextButton, &QPushButton::clicked, this, &StudentWindow::nextStudent);
        connect(newButton, &QPushButton::clicked, this, &StudentWindow::setAddMode);
        connect(table, &QTableWidget::cellDoubleClicked, this, [this](int row, int) {
            if (row >= 0 && row < students.size()) showStudent(row);
        });
    }

    static QString csvEscape(const QString &value) {
        QString v = value;
        v.replace("\"", "\"\"");
        if (v.contains(',') || v.contains('"') || v.contains('\n') || v.contains('\r'))
            return "\"" + v + "\"";
        return v;
    }

    static QStringList parseCsvLine(const QString &line) {
        QStringList fields;
        QString field;
        bool quoted = false;
        for (qsizetype i = 0; i < line.size(); ++i) {
            const QChar ch = line[i];
            if (ch == '"') {
                if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"'; ++i;
                } else quoted = !quoted;
            } else if (ch == ',' && !quoted) {
                fields << field; field.clear();
            } else field += ch;
        }
        fields << field;
        return fields;
    }

    bool saveRecords() {
        QFile file(dataFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "File Error", "Could not write the data file:\n" + file.errorString());
            return false;
        }
        QTextStream out(&file);
        out << "Name,RollNumber,FatherName,Caste,District,GPA\n";
        for (const Student &s : students) {
            out << csvEscape(s.name) << ',' << csvEscape(s.rollNumber) << ','
                << csvEscape(s.fatherName) << ',' << csvEscape(s.caste) << ','
                << csvEscape(s.district) << ',' << QString::number(s.gpa, 'f', 2) << '\n';
        }
        return true;
    }

    void loadRecords() {
        students.clear();
        QFile file(dataFile);
        if (!file.exists()) { refreshTable(); return; }
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "File Error", "Could not read the data file:\n" + file.errorString());
            return;
        }
        QTextStream in(&file);
        bool firstLine = true;
        while (!in.atEnd()) {
            const QString line = in.readLine();
            if (firstLine) { firstLine = false; if (line.startsWith("Name,RollNumber")) continue; }
            if (line.trimmed().isEmpty()) continue;
            const QStringList f = parseCsvLine(line);
            if (f.size() != 6) continue;
            bool ok = false;
            const double gpa = f[5].toDouble(&ok);
            if (!ok || gpa < 0 || gpa > 4) continue;
            students.push_back({f[0], f[1], f[2], f[3], f[4], gpa});
        }
        refreshTable();
    }

    void refreshTable() {
        table->setRowCount(students.size());
        for (int r = 0; r < students.size(); ++r) {
            const Student &s = students[r];
            const QStringList values{s.name, s.rollNumber, s.fatherName, s.caste, s.district, QString::number(s.gpa, 'f', 2)};
            for (int c = 0; c < values.size(); ++c) table->setItem(r, c, new QTableWidgetItem(values[c]));
        }
        nextButton->setEnabled(!students.isEmpty());
    }

    bool validateForm(Student &s) {
        s.name = nameEdit->text().trimmed();
        s.rollNumber = rollEdit->text().trimmed();
        s.fatherName = fatherEdit->text().trimmed();
        s.caste = casteEdit->text().trimmed();
        s.district = districtEdit->text().trimmed();
        const QString gpaText = gpaEdit->text().trimmed();
        if (s.name.isEmpty() || s.rollNumber.isEmpty() || s.fatherName.isEmpty() ||
            s.caste.isEmpty() || s.district.isEmpty() || gpaText.isEmpty()) {
            QMessageBox::warning(this, "Validation", "Please complete all six fields.");
            return false;
        }
        bool ok = false;
        s.gpa = gpaText.toDouble(&ok);
        if (!ok || s.gpa < 0.0 || s.gpa > 4.0) {
            QMessageBox::warning(this, "Validation", "GPA must be a number between 0.00 and 4.00.");
            return false;
        }
        return true;
    }

    int findByRoll(const QString &roll) const {
        for (int i = 0; i < students.size(); ++i)
            if (students[i].rollNumber.compare(roll, Qt::CaseInsensitive) == 0) return i;
        return -1;
    }

    void clearFields() {
        for (auto *edit : {nameEdit, rollEdit, fatherEdit, casteEdit, districtEdit, gpaEdit}) edit->clear();
        table->clearSelection();
    }

    void setAddMode() {
        browseMode = false;
        currentIndex = -1;
        clearFields();
        addButton->setEnabled(true);
        updateButton->setEnabled(false);
        deleteButton->setEnabled(false);
        modeLabel->setText("Mode: Add a new student");
        statusBar()->showMessage("Enter student details and click Add Record.");
    }

    void showStudent(int index) {
        if (index < 0 || index >= students.size()) return;
        currentIndex = index;
        browseMode = true;
        const Student &s = students[index];
        nameEdit->setText(s.name); rollEdit->setText(s.rollNumber);
        fatherEdit->setText(s.fatherName); casteEdit->setText(s.caste);
        districtEdit->setText(s.district); gpaEdit->setText(QString::number(s.gpa, 'f', 2));
        table->selectRow(index);
        addButton->setEnabled(false);
        updateButton->setEnabled(true);
        deleteButton->setEnabled(true);
        modeLabel->setText(QString("Mode: Viewing record %1 of %2").arg(index + 1).arg(students.size()));
    }

private slots:
    void addStudent() {
        Student s;
        if (!validateForm(s)) return;
        if (findByRoll(s.rollNumber) != -1) {
            QMessageBox::warning(this, "Duplicate Roll Number", "A student with this roll number already exists.");
            return;
        }
        students.push_back(s);
        if (!saveRecords()) { students.removeLast(); return; }
        refreshTable();
        QMessageBox::information(this, "Saved", "Student record added successfully.");
        setAddMode();
        statusBar()->showMessage("Record added. Total records: " + QString::number(students.size()));
    }

    void nextStudent() {
        if (students.isEmpty()) return;
        const int next = (currentIndex + 1) % students.size();
        showStudent(next);
        statusBar()->showMessage("Showing record " + QString::number(next + 1) + " of " + QString::number(students.size()));
    }

    void updateStudent() {
        if (currentIndex < 0 || currentIndex >= students.size()) return;
        Student s;
        if (!validateForm(s)) return;
        const int duplicate = findByRoll(s.rollNumber);
        if (duplicate != -1 && duplicate != currentIndex) {
            QMessageBox::warning(this, "Duplicate Roll Number", "Another student already uses this roll number.");
            return;
        }
        const Student old = students[currentIndex];
        students[currentIndex] = s;
        if (!saveRecords()) { students[currentIndex] = old; return; }
        refreshTable();
        showStudent(currentIndex);
        QMessageBox::information(this, "Updated", "Student record updated successfully.");
    }

    void deleteStudent() {
        if (currentIndex < 0 || currentIndex >= students.size()) return;
        const QString roll = students[currentIndex].rollNumber;
        if (QMessageBox::question(this, "Confirm Delete",
            "Delete the record for roll number " + roll + "?",
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
        const int removedIndex = currentIndex;
        students.removeAt(currentIndex);
        if (!saveRecords()) { loadRecords(); setAddMode(); return; }
        refreshTable();
        if (!students.isEmpty()) showStudent(qMin(removedIndex, students.size() - 1));
        else setAddMode();
        QMessageBox::information(this, "Deleted", "Student record deleted successfully.");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    StudentWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
