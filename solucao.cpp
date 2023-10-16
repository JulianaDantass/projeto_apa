#include "solucao.h"




Solucao::Solucao(int veiculo, int capacidade){

	this->veiculo = veiculo;
	this->capacidade = capacidade;
}


void Solucao::insereCaminhoFim(int cliente){

	this->caminho.push_back(cliente);

}

vector < int> * Solucao::getCaminho(){

	return &this->caminho;
}

void Solucao::setObjetivo(int valor){

	this->funcao_objetivo = valor;
}

int Solucao::getObjetivo(){

	return this->funcao_objetivo;
}

int Solucao::getCapacidade(){

	return this->capacidade;
}

void Solucao::setCapacidade(int valor){

	this->capacidade = valor;
}
