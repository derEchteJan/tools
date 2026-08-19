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

 - rework scripts as modules, create a common module for util functions
   - On back-navigation read specific url parameter to present Toast on arrival + provide functions to set that same parameter
   - could also be a cookie
   - add overlay structure to overview.html
 - edit.js: prevent upload when no file selected in form, set file name from input
 - fix / test overlay on other webengines than firefox
 - force refresh when navigating back from edit page, remove temporary random url param "reload=.."
 - implement verbose url parameter in all php calls, input cleanup
 - debug url parameter for frontend
 - anchor ro/rw port, ro/rwm subdomain, page name and other constants in javascript for redirects
   - config should also be avail for php context
   - provide a page specific bashrc file in document root
 - overlay for pick-path on edit page and upload page
 - upload/create should only create files ending with .md
 - tables: parse header separator line
 - add functionality to export user created data as downloadable / uploadable tar archive
   - partially implemented with alias 'copy-out'
 - split search string by space
 - syntax for markdown elements to override css class selector
 - git gud