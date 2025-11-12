#!/bin/sh
set -e

if [ "$1" = 'mysqld' ] && [ ! -d "/var/lib/mysql/mysql" ]; then
	mysql_install_db --user=mariadb --datadir=/var/lib/mysql --rpm
	mysqld --user=mariadb --skip-networking &
	pid=$!
	while ! mysqladmin ping --silent; do sleep 1; done
	mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${MYSQL_ROOT_PASSWORD}';"
	[ "$MYSQL_DATABASE" ] && mysql -e "CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;"
	[ "$MYSQL_USER" ] && [ "$MYSQL_PASSWORD" ] && \
		mysql -e "CREATE USER '${MYSQL_USER}'@'%' IDENTIFIED BY '${MYSQL_PASSWORD}';" && \
		mysql -e "GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';"
	mysql -e "FLUSH PRIVILEGES;"
	# [ -f "/usr/bin/local/init.sql" ] && mysql "${MYSQL_DATABASE}" < /usr/bin/local/init.sql
	kill "$pid" && wait "$pid"
fi

exec "$@"
