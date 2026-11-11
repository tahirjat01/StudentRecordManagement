# Student Record Management System (C++ / Qt 6)

A GUI-based semester project implementing the supplied SW112 OEL specification.

## Features
- Six labeled fields: Name, Roll Number, Father's Name, Caste, District, GPA.
- Add, Next, Update, Delete and New/Add Mode actions.
- CSV text-file persistence with quoted-field escaping.
- Roll Number is the unique identifier (case-insensitive duplicate check).
- Required-field and GPA validation (0.00–4.00).
- Confirmation dialogs for add/update/delete and delete cancellation.
- Record table, status bar, and double-click-to-view enhancement.
- Initial state: Add enabled; Update and Delete disabled.

## Requirements
- C++17 compiler
- CMake 3.16+
- Qt 6 Widgets development package

## Build (Windows, Qt Creator)
1. Install Qt 6 (including the MinGW or MSVC kit) and CMake.
2. Open this folder's `CMakeLists.txt` in Qt Creator.
3. Select a configured Qt 6 kit, configure, then Build and Run.

## Build (command line)
```bash
cmake -S . -B build
cmake --build build --config Release
```
Run the generated `StudentRecordManager` executable. On Windows it will be in
`build/Release/` for a multi-configuration generator or `build/` otherwise.

## Data file
The application reads and writes `students.csv` beside the executable. If it does
not exist, it is created on the first successful Add. Back up this file before
moving or deleting the application. The first line is a header.

CSV columns:
`Name,RollNumber,FatherName,Caste,District,GPA`

## Important behavior
- **Next** cycles through records and wraps to the first after the last.
- Selecting a row by double-click also opens that record.
- **New / Add Mode** clears the form and re-enables Add.
- A duplicate roll number is rejected. During Update, the current student's
  existing roll number is allowed.
- A record is only changed in memory if saving succeeds; failed writes are
  reported to the user.
- The parser handles commas and quotation marks inside text fields. It expects
  one CSV record per physical line; do not enter line breaks in a field.

## Suggested manual test
See `docs/TEST_CASES.md`. The supplied CSV contains two demonstration records;
remove or edit them before using real student data.

## Submission checklist
- Build and run using Qt 6 on your own computer.
- Replace the design-preview image in the report with a genuine screenshot of
  the running application.
- Test all cases and update the results table with actual outcomes.
- Create a GitHub repository, push this folder, and add its public URL to the
  report.
- Record a short demonstration, upload it to Google Drive with public link
  access, and add that URL to the report. These external uploads require your
  own accounts and have not been performed by this package.

## Presentation
`Project_Presentation.pptx` contains a short slide deck. `docs/DEMO_VIDEO_SCRIPT.md` provides a suggested 3–4 minute narration and demo sequence.
