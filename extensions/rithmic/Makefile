.PHONY: proto clean build run

PROTO_DIR := proto
OUT_DIR := api

proto:
	protoc --go_out=$(OUT_DIR) --go_opt=paths=source_relative \
	       --go-grpc_out=$(OUT_DIR) --go-grpc_opt=paths=source_relative \
	       $(PROTO_DIR)/*.proto

build:
	go build -o rithmic-feed main.go

run:
	go run main.go

clean:
	rm -f rithmic-feed
