FROM httpd

EXPOSE 80
EXPOSE 81

# Copy webfiles

COPY ./html/ /usr/local/apache2/htdocs/
RUN chown -R www-data:www-data /usr/local/apache2/htdocs

# Compile and copy generator

#COPY ./source /var/mdgen/source
#RUN g++ /var/mdgen/source/*.cpp -D LIVE_PATHS -o /var/mdgen/generator

# Install apache php module

RUN apt-get update
RUN apt-get install -y php libapache2-mod-php

# Copy custom apache config

COPY ./config/httpd.conf /usr/local/apache2/conf/
