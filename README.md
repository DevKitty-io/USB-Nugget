# RubberNugget-Arduino

### Beta v1.0 Feature List
- [x] Directory Traversal
- [x] Defaults / Checks
- [x] File IO
- [x] DuckyScript Parsing
- [x] Web Access

### Priority Features v1.0 release
- [x] Single level navigation
- [ ] Responsive Web UI 
- [ ] Web close button
- [ ] duckyscript alias colors / min delay
- [ ] file name truncation
- [ ] update graphics - arrow indicators, nugget

### Future Features
- [ ] DuckyScript Aliasing / Colors
- [ ] Proper FS mounting / USB
- [ ] Overhaul / Implement OOP Graphic Library
- [ ] Scrolling full file path
- [ ] Web portal password config
- [ ] Graphic differentiation - files vs folders
- [ ] Edit web files from file system

## Bugs
### Critical
- [ ] File names collide w/ Nugget graphic
- [x] Recursive payload selection w/ no exit clause
- [ ] Responsive web interface
- [x] Can't access web UI after running payload
- [x] web UI shows doesn't subfolder files - follows OS/Type paradigm

### Meh
- [ ] Folders named `*txt` trigger payloadRun
- [ ] Files named `BACK` trigger backwards navigation
- [ ] Shouldn't run empty files
- [ ] Keypress debouncing / key features
- [ ] Proper input sanitation for filepaths
