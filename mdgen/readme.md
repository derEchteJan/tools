### Markdown Website Generator

Usage:

In bash, load bashrc file for build commands like so:

```
#!/bin/bash

. bashrc          # or 'source bashrc'

build             # compiles the generator binary

container-build   # builds container image

run               # creates and runs container
```

**TODOs:**

 - implement verbose url parameter in all php calls, input cleanup
 - upload/create needs to resepect pages subdir
 - anchor ro/rw port and/or ro/rw subdomain in javascript for redirects
 - upload/create should not create files not ending with .md
 - serialize page title
 - tables: parse header separator line
 - add functionality to export user created data as downloadable / uploadable tar archive
   - partially implemented with alias 'copy-out'
 - split search string by space
 - git gud