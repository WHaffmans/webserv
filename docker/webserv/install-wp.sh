#!/bin/sh


if [ -f /var/www/html/wp-config.php ]; then
	echo "WordPress is already installed."
	exec "$@"
fi

echo "Installing WordPress..."

echo "wp core download --allow-root --path=/var/www/html"
wp core download --allow-root --path=/var/www/html

echo "wp config create --allow-root"
wp config create --allow-root \
	--dbname=$MYSQL_DATABASE \
	--dbuser=$MYSQL_USER \
	--dbpass=$MYSQL_PASSWORD \
	--dbhost=$DB_HOST \
	--path=/var/www/html

echo "wp core install --allow-root"
wp core install --allow-root \
	--url=$DOMAIN_NAME \
	--title=$WP_TITLE \
	--admin_user=$WP_ADMIN \
	--admin_password=$WP_ADMIN_PASSWORD \
	--admin_email=$WP_ADMIN_EMAIL \
	--path=/var/www/html

echo "wp user create --allow-root"
wp user create --allow-root \
	$WP_USER \
	$WP_USER_EMAIL \
	--role=author \
	--user_pass=$WP_USER_PASSWORD \
	--path=/var/www/html

echo "wp theme install --allow-root"
wp theme install --allow-root \
	$WP_THEME \
	--activate \
	--path=/var/www/html

echo "WordPress installation completed."

chown -R wordpress:wordpress /var/www/html

exec "$@"