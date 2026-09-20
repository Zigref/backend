FROM --platform=amd64 amd64/alpine:latest AS builder

RUN apk add --no-cache make sqlite-dev g++ asio-dev

WORKDIR /app
COPY . .

RUN make
RUN strip build/server

FROM --platform=amd64 amd64/alpine:latest

RUN apk add --no-cache wget ca-certificates sqlite-libs libstdc++

WORKDIR /app
COPY --from=builder /app/build/server ./build/server

EXPOSE 8080

CMD ["./build/server"]