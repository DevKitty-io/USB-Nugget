# RubberNugget USB Attack Tool
The RubberNugget is a USB attack tool that lets you hack computers in seconds with keystroke injection!  

## Features

### Beta v1.0 Feature List
- [x] Directory Traversal
- [x] Defaults / Checks
- [x] File IO
- [x] DuckyScript Parsing
- [x] Web Access

### Priority Features v1.0 release
- [x] Single level navigation
- [x] Responsive Web UI 
- [x] Web close button
- [x] duckyscript min delay
- [x] file name truncation
- [x] update graphics - arrow indicators, nugget

### Future Features
- [ ] DuckyScript Aliasing
- [ ] Proper FS mounting / USB
- [ ] Overhaul / Implement OOP Graphic Library
- [ ] Scrolling full file path
- [ ] Web portal password config
- [ ] Graphic differentiation - files vs folders
- [ ] Edit web files from file system

## Bugs
### Critical
- [x] File names collide w/ Nugget graphic
- [x] Recursive payload selection w/ no exit clause
- [x] Responsive web interface
- [x] Can't access web UI after running payload
- [x] web UI shows doesn't subfolder files - follows OS/Type paradigm
- [x] crash after exit payload / clear screen delaying

### Meh / Edge Cases
- [ ] Folders named `*txt` trigger payloadRun
- [ ] Files named `BACK` trigger backwards navigation
- [ ] Files parsed as folder for extensions that arent `.txt`
- [ ] Shouldn't run empty files
- [ ] Keypress debouncing / key features
- [ ] Proper input sanitation for filepaths
- [ ] Can't rename files / folders
