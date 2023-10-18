#include "veiculo.h"
#include <iostream>
using namespace std;




Veiculo::Veiculo(int veiculo, int capacidade, int qClientes){

	this->veiculo = veiculo;
	this->capacidade = capacidade;
	this->funcao_objetivo = 0;
	
	//O vetor caminhoTotal vai indicar o caminho percorrido pelo veiculo
	//Como iniciamos da garagem caminhoTotal[0] = 1 por exemplo indica que
	//Do ponto 0 q é a garagem vamos para o indice caminhoTotal[1] = 2 caminhoTotal[2] = 3
	//caminho total[3] = 0, logo o caminho do veiculo seria 0 -> 1 -> 2 -> 3 -> 0
	for(int i = 0; i < qClientes; i++){

		this->caminhoTotal.push_back(-1);
	}

}

void Veiculo::setCliente(int proximo_cliente, int clienteAnterior){
	this->caminhoTotal[clienteAnterior] = proximo_cliente;
	cout << "Cliente anterior: " << clienteAnterior << " vai para " << proximo_cliente << endl;

}


int Veiculo::printaCaminhoTotal(int pontoInicial){
	int cliente = this->caminhoTotal[pontoInicial];
	cout << pontoInicial << " -> ";

	if (cliente == 0){
		cout << cliente << endl;
		return 1;
	}else{
		int proxCliente = printaCaminhoTotal(cliente);
		return proxCliente;
	}

}

void Veiculo::insereCaminhoFim(int cliente){
	
	this->caminho.push_back(cliente);

}

int Veiculo::getUltimoCliente(){

	return this->caminho.back();
}

vector < int> * Veiculo::getCaminhoInicial(){

	return &this->caminho;
}

void Veiculo::setObjetivo(int valor){

	this->funcao_objetivo = valor;
}

int Veiculo::getObjetivo(){

	return this->funcao_objetivo;
}

int Veiculo::getCapacidade(){

	return this->capacidade;
}

void Veiculo::setCapacidade(int valor){

	this->capacidade = valor;
}