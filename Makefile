run:
	g++ -std=c++23 ./main.cpp $(shell pkg-config --libs --cflags asio sqlite3)

download_database:
	curl -fL "https://huggingface.co/buckets/Zigistry/Zigistry/resolve/zigistry.db" -o ./zigistry.db
	
