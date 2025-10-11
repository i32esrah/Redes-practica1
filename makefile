
all: saluda Servidor Cliente

saluda:
	@echo "Compilando el archivo Servidor y Cliente"

Servidor: servidor.cpp  servidor.hpp
	g++ servidor.cpp -o servidor

Cliente: cliente.cpp
	g++ cliente.cpp -o cliente