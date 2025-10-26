.PHONY: env dev client server test clean

env:
	conda env update -f environment.yml || conda env create -f environment.yml

dev:
	./scripts/local_dev.sh

client:
	cd client && pnpm run dev

server:
	python3 server/run_dev.py
	@echo "TODO: start server runtime when wired"

test:
	pytest -q || true

clean:
	rm -rf dist .pytest_cache
