all: build up

# URL of the repo to clone inside the webserv builder image
GIT_URL := https://github.com/WHaffmans/webserv.git
# Query the remote HEAD commit so builds are invalidated when upstream changes
GIT_COMMIT ?= $(shell git rev-parse HEAD)

DC := docker-compose

build:
	$(DC) build --build-arg GIT_COMMIT=$(GIT_COMMIT)

nocache:
	$(DC) build --no-cache --build-arg GIT_COMMIT=$(GIT_COMMIT)

up:
	$(DC) up

down:
	$(DC) down

logs:
	$(DC) logs -f

re: fclean up

clean:
	@echo "$(gub)Cleaning up Docker containers...$(reset)"
	$(DC) down --rmi local --volumes --remove-orphans
	@echo "$(gub)Removing Docker networks...$(reset)"
	@docker network prune -f
	@echo "$(gub)Removing Docker volumes...$(reset)"
	@docker volume prune -f
	@echo "$(gub)Removing Docker images...$(reset)"
	@docker rmi -f $(shell docker images -q)

fclean: clean
	@find data/wordpress -mindepth 1 ! -name '.gitignore' -delete

.PHONY: all build up down re clean fclean logs rebuild

green:=$(shell tput setaf 2)
bold:=$(shell tput bold)
uncerline:=$(shell tput smul)
gub:=$(green)$(underline)$(bold)
reset:=$(shell tput sgr0)