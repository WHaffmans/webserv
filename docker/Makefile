all: build up

# URL of the repo to clone inside the webserv builder image
GIT_URL := https://github.com/WHaffmans/webserv.git
# Query the remote HEAD commit so builds are invalidated when upstream changes
GIT_COMMIT := $(shell git rev-parse HEAD)

build:
	@echo "Building images (GIT_COMMIT=$(GIT_COMMIT))"
	docker compose build --build-arg GIT_COMMIT=$(GIT_COMMIT)

nocache:
	@echo "Building images without cache (GIT_COMMIT=$(GIT_COMMIT))"
	docker compose build --no-cache --build-arg GIT_COMMIT=$(GIT_COMMIT)


up:
	docker compose up

upd:
	docker compose up -d

down:
	docker compose down

logs:
	docker compose logs -f

ps:
	docker compose ps

clean:
	docker compose down --rmi all --volumes --remove-orphans
	@echo "Removing files from host volumes using an ephemeral container (no sudo)..."
	# remove everything in data/wordpress except .gitignore using a container
	@docker run --rm -v $(shell pwd)/data/wordpress:/data:rw alpine:3.18 sh -c "if [ -d /data ]; then cd /data && find . -maxdepth 1 ! -name '.' ! -name '.gitignore' -exec rm -rf '{}' +; fi"

rebuild: down build up

re: down clean build up

.PHONY: all build up upd down logs ps clean rebuild webserv re nocache