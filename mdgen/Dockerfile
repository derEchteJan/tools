FROM php:7.2-apache

EXPOSE 80

COPY ./html/ /var/www/html/
COPY ./build/generator /usr/bin

RUN chown -R www-data:www-data /var/www/html