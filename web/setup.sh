sudo apt install -y apache2

apt-get install -y libapache2-mod-php

sudo a2enmod php8.3

sudo systemctl reload apache2

sudo mkdir -p /var/www/html
sudo chmod 777 /var/www/html
sudo chmod 666 /var/www/html/*