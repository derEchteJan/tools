#!/bin/bash

chown -R www-data:www-data $1
find $1 -type f -print0 | xargs -0 chmod 666
find $1 -type d -print0 | xargs -0 chmod 775
chmod 775 $1