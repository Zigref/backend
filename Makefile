run:
	g++ -std=c++23 ./main.cpp $(shell pkg-config --libs --cflags asio)

download_database:
	curl -fL "https://huggingface.co/buckets/Zigistry/Zigistry/resolve/zigistry.db" -o ./zigistry.db
	
