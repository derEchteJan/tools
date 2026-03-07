FROM php:7.2-apache

EXPOSE 80

COPY ./html/ /var/www/html/
COPY ./source /var/mdgen/source

RUN g++ /var/mdgen/source/*.cpp -D LIVE_PATHS -o /var/mdgen/generator
RUN chown -R www-data:www-data /var/www/html