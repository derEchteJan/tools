### Markdown Website Generator

Usage:

In bash, load bashrc file for build commands like so:

```
#!/bin/bash

. bashrc        # or 'source bashrc'

build && test   # compiles the generator binary and runs it on local test files

compose         # composes docker container image and starts it
```

**TODOs:**

 - logging function and verbosity level
 - tables: parse header separator line
 - add docker volume to keep persistent data
 - add functionality to export user created data as downloadable / uploadable tar archive
 - split search string by space
 - git gud