#include "solucao.h"




Solucao::Solucao(int veiculo, int capacidade){

	this->veiculo = veiculo;
	this->capacidade = capacidade;
	this->funcao_objetivo = 0;

}


void Solucao::insereCaminhoFim(int cliente){
	
	this->caminho.push_back(cliente);

}

int Solucao::getUltimoCliente(){

	return this->caminho.back();
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
