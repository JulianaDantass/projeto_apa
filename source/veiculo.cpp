#include "veiculo.h"
#include <iostream>
using namespace std;

Veiculo::Veiculo(){

	this->veiculo = -1;
	this->capacidade_disponivel = -1;
	this->custo = -1;
	this->funcao_objetivo = 0;
	this->qtd_clientes = 0;
}

void Veiculo::edita_parametros(int veiculo, int capacidade_disponivel, int qClientes, int custoVeiculo){

	this->veiculo = veiculo;
	this->capacidade_disponivel = capacidade_disponivel;
	this->custo = custoVeiculo;
	this->funcao_objetivo = 0;
	//O vetor caminhoTotal vai indicar o caminho percorrido pelo veiculo
	//Como iniciamos da garagem caminhoTotal[0] = 1 por exemplo indica que
	//Do ponto 0 q é a garagem vamos para o indice caminhoTotal[1] = 2 caminhoTotal[2] = 3
	//caminho total[3] = 0, logo o caminho do veiculo seria 0 -> 1 -> 2 -> 3 -> 0
	for(int i = 0; i < qClientes + 1; i++){

		this->caminho_total.push_back(-1);
	}

}

void Veiculo::set_cliente(int proximo_cliente, int cliente_anterior){

	this->caminho_total[cliente_anterior] = proximo_cliente;
	//cout << "Cliente anterior: " << clienteAnterior << " vai para " << proximo_cliente << endl;
}

void Veiculo::incrementa_clientes(){

	this->qtd_clientes++;
}

void Veiculo::decrementa_clientes(){

	this->qtd_clientes--;
}

void Veiculo::set_quantia_clientes(int qtd_clientes){

	this->qtd_clientes = qtd_clientes;
}

int Veiculo::get_quantia_clientes(){

	return this->qtd_clientes;
}

int Veiculo::get_prox_cliente(int cliente_anterior){

	return this->caminho_total[cliente_anterior];
}

void Veiculo::adiciona_custo_veiculo(){

	this->funcao_objetivo += custo;
}

int Veiculo::printa_caminho_total(int ponto_inicial){
	
	int cliente = this->caminho_total[ponto_inicial];
	cout << ponto_inicial << " -> ";

	if (cliente == 0){
		cout << cliente << endl;
		return 1;
	}else{
		int prox_cliente = printa_caminho_total(cliente);
		return prox_cliente;
	}

}

void Veiculo::set_objetivo(int valor){

	this->funcao_objetivo = valor;
}

int Veiculo::get_objetivo(){

	return this->funcao_objetivo;
}

int Veiculo::get_capacidade_disp(){

	return this->capacidade_disponivel;
}

void Veiculo::set_capacidade_disp(int valor){

	this->capacidade_disponivel = valor;
}
