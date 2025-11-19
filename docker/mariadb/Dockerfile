
FROM alpine:3.20

RUN apk add --no-cache \
	mariadb \
	mariadb-client && \
	rm -rf /var/cache/apk/*

# ARG HOST_UID

RUN addgroup -S mariadb && \		
	adduser -S mariadb -G mariadb

RUN mkdir -p /var/run/mysqld && \
	chown -R mariadb:mariadb /var/run/mysqld && \
	chown -R mariadb:mariadb /var/lib/mysql && \
	chmod -R 750 /var/lib/mysql

COPY ./tools/docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
COPY ./conf/my.cnf /etc/my.cnf
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

WORKDIR /var/lib/mysql

USER mariadb

ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]

CMD ["mysqld", "--datadir=/var/lib/mysql", "--user=mariadb"]
HEALTHCHECK --interval=5s --timeout=3s --start-period=5s --retries=1 CMD mysqladmin ping -h localhost || exit 1
