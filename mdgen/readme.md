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

 - edit.js: prevent upload when no file selected in form, set file name from input
 - fix / test overlay on other webengines than firefox
 - force refresh when navigating back from edit page, remove temporary random url param "reload=.."
 - implement verbose url parameter in all php calls, input cleanup
 - debug url parameter for frontend
 - upload/create needs to resepect pages subdir
 - anchor ro/rw port, ro/rwm subdomain, page name and other constants in javascript for redirects
   - config should also be avail for php context
 - overlay for pick-path on edit page and upload page
 - upload/create should only create files ending with .md
 - separate mkdir.php route instead of creating any missing directories recursively in upload.php
    - upload should only create dir/.attachments if its missing to prevent accidents
 - tables: parse header separator line
 - dark/light mode switch button
 - add functionality to export user created data as downloadable / uploadable tar archive
   - partially implemented with alias 'copy-out'
 - split search string by space
 - syntax for markdown elements to override css class selector
 - git gud