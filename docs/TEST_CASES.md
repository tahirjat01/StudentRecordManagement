# Test Cases

| ID | Test action | Expected result |
|---|---|---|
| TC-01 | Launch with no CSV file | Empty form; Add enabled; Update/Delete disabled; Next disabled |
| TC-02 | Add a complete valid record | Confirmation; record appears in table and CSV |
| TC-03 | Add with one or more empty fields | Validation warning; no record saved |
| TC-04 | Add duplicate Roll Number | Duplicate warning; no duplicate saved |
| TC-05 | Enter GPA below 0 or above 4 | Input/validation rejects invalid GPA |
| TC-06 | Click Next with records present | Next record is displayed; Add disabled; Update/Delete enabled |
| TC-07 | Update a displayed record | Same roll-number record is modified in CSV and table |
| TC-08 | Update using another record's Roll Number | Duplicate warning; original data remains |
| TC-09 | Delete and choose No | Record remains unchanged |
| TC-10 | Delete and choose Yes | Record is removed from CSV and table |
| TC-11 | Restart application | Previously saved records load from CSV |
| TC-12 | Use commas or quotes in a text field | CSV value is quoted/escaped and read back |
| TC-13 | Click New / Add Mode | Form clears; Add enabled; Update/Delete disabled |
| TC-14 | Double-click a table row | Selected record loads into the form |

**Execution note:** These are planned manual tests, not claims of tests already executed.
Record Pass/Fail and evidence after running the application with Qt 6.
