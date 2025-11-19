all: build up logs

build:
	docker-compose build

rebuild:
	docker-compose build --no-cache

up:
	docker-compose up -d

down:
	docker-compose down

logs:
	docker-compose logs -f

re: fclean up

clean: down
	docker-compose rm -f -v

fclean: clean
	@find data/wordpress -mindepth 1 ! -name '.gitignore' -delete

.PHONY: all build up down re clean fclean logs rebuild