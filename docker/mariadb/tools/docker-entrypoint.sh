#!/bin/sh
set -e

echo "Starting MariaDB initialization..."

if [ "$1" = 'mysqld' ] && [ ! -d "/var/lib/mysql/mysql" ]; then
    echo "Initializing database..."
    mysql_install_db --user=mariadb --datadir=/var/lib/mysql --rpm
    
    echo "Starting temporary MySQL server..."
    mysqld --user=mariadb --skip-networking &
    pid=$!
    
    # Wait for MySQL to be ready
    echo "Waiting for MySQL to start..."
    while ! mysqladmin ping --silent; do 
        sleep 1
        echo "Waiting..."
    done
    
      echo "Setting up database and users..."
    # First flush privileges to enable grant tables
    mysql -e "FLUSH PRIVILEGES;"
    # Set root password and ensure proper authentication
    mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${MYSQL_ROOT_PASSWORD}';"
    mysql -e "GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION;"

    [ "$MYSQL_DATABASE" ] && mysql -e "CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;"
    # [ "$MYSQL_USER" ] && [ "$MYSQL_PASSWORD" ] && \
	mysql -e "CREATE USER '${MYSQL_USER}'@'%' IDENTIFIED BY '${MYSQL_PASSWORD}';" && \
	mysql -e "GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';"
    mysql -e "FLUSH PRIVILEGES;"
    
    echo "Stopping temporary server..."
    kill "$pid" && wait "$pid"
    echo "Database initialization completed!"
fi

echo "Starting MariaDB server..."
exec "$@"