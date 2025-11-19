#!/bin/sh


if [ -f /webserv/htdocs//wp-config.php ]; then
	echo "WordPress is already installed."
	exec "$@"
fi

echo "Installing WordPress..."

echo "wp core download --allow-root --path=/webserv/htdocs/"
wp core download --allow-root --path=/webserv/htdocs/

echo "wp config create --allow-root"
wp config create --allow-root \
	--dbname="$MYSQL_DATABASE" \
	--dbuser="$MYSQL_USER" \
	--dbpass="$MYSQL_PASSWORD" \
	--dbhost=$DB_HOST \
	--path=/webserv/htdocs/

echo "wp core install --allow-root"
wp core install --allow-root \
	--url="$DOMAIN_NAME" \
	--title="$WP_TITLE" \
	--admin_user="$WP_ADMIN" \
	--admin_password="$WP_ADMIN_PASSWORD" \
	--admin_email="$WP_ADMIN_EMAIL" \
	--path=/webserv/htdocs/

echo "wp user create --allow-root"
wp user create --allow-root \
	"$WP_USER" \
	"$WP_USER_EMAIL" \
	--role=author \
	--user_pass="$WP_USER_PASSWORD" \
	--path=/webserv/htdocs/

echo "wp theme install --allow-root"
wp theme install --allow-root \
	"$WP_THEME" \
	--activate \
	--path=/webserv/htdocs/

echo "WordPress installation completed."

# chown -R wordpress:wordpress /webserv/htdocs/

exec "$@"