#include "heuristica.h"
#include <algorithm>
#include <iostream>


Heuristica::Heuristica(int clientes, int qVeiculos, int capacidade, int minimo_entregas, int custo_veiculo, 
				vector < int> *demanda, vector <int> * custo_terceirizacao, vector <vector<int>>* matriz_distancia){
	
	this->clientes = clientes;
	this->capacidade = capacidade;
	this->qVeiculos = qVeiculos;
	this->minimo_entregas = minimo_entregas;
	this->custo_veiculo = custo_veiculo;
	this->demanda = demanda;
	this->custo_terceirizacao = custo_terceirizacao;
	this->matriz_distancia = matriz_distancia;
	
	for(int i = 0; i < clientes; i++){

		clientesOrdenados.push_back(i);
	}

	solucoes = new vector < Solucao * >(qVeiculos);

	
}

int Heuristica::calculaObjetivo(int cliente_anterior, int cliente_atual, int objetivo){
	cout << "Cliente anterior: " << cliente_anterior << endl;
	cout << "cliente atual: " << cliente_atual << endl;
	cout << "Custo da aresta: " << (*matriz_distancia)[cliente_anterior][cliente_atual] << endl;

	int funcaoObjetivo = objetivo + (*matriz_distancia)[cliente_anterior][cliente_atual];
	cout << "FuncaoObjetivo apos o calculo: " << funcaoObjetivo << endl;	
	return funcaoObjetivo;
}



void Heuristica::solucaoInicial(int indice_solucao){

	solucoes->at(indice_solucao) = new Solucao(indice_solucao, capacidade);
	Solucao* s = solucoes->at(indice_solucao);
	s->insereCaminhoFim(0);
	int inseriu = 0;
	int funcaoObjetivo = 0;
	
	/* Geralmente vai funcionar em O(2) no pior caso será O(n)
	 * Porém a complexidade justa é O(2)	*/

	for(int j = 0; j < clientes; j++){
		int cliente_indice = clientesOrdenados.back();
		int demanda_cliente = this->demanda->at(cliente_indice);
		int capacidade = s->getCapacidade();

		if(demanda_cliente <= capacidade){
			int novaCapacidade = capacidade - demanda_cliente;
			funcaoObjetivo = calculaObjetivo(s->getUltimoCliente(), cliente_indice, s->getObjetivo());
			
			s->insereCaminhoFim(cliente_indice);
			s->setObjetivo(funcaoObjetivo);
			s->setCapacidade(novaCapacidade);
			inseriu++;
			clientesOrdenados.pop_back();
			
			getchar();	
		}
		
		if(inseriu == 2)
			break;
	}
	
	funcaoObjetivo = calculaObjetivo(s->getUltimoCliente(), 0, s->getObjetivo());
	s->insereCaminhoFim(0);
	s->setObjetivo(funcaoObjetivo);
	cout << "Capacidade do veiculo após solucao inicial: " << s->getCapacidade() << endl;

}



/* A insercao do mais proximo
 * será realizada inicalmente um veiculo por vez
 * pegamos inicialmente uma solucao inicial formada por 2 vertices mais o deposito
 * e fazemos isso para todo veiculo	*/

void Heuristica::insercaoMaisBarata(){
	
	solucaoInicial(0);
	vector < int >*caminhoInicial = solucoes->at(0)->getCaminho();

	for(int i = 0; i < caminhoInicial->size(); i++){

		cout << caminhoInicial->at(i) << " ";
	}
	cout << endl;
	cout << "Objetivo Inicial: " << solucoes->at(0)->getObjetivo() << endl;
	getchar();


}
void Heuristica::solve(){
	
	/* Ordenacao dos clientes em ordem do maior para o menor em relacao a distancia do deposito	
	 * A ordenacao eh feita em nlog(n)*/

	std::sort(clientesOrdenados.begin(), clientesOrdenados.end(),[&](const int a, const int b){
				return (*matriz_distancia)[a][0] <= (*matriz_distancia)[b][0];
			});
	

	cout << "Ordenacao dos clientes em relacao ao deposito: ";

	for(int i = 0; i < clientesOrdenados.size(); i++){
		cout << clientesOrdenados[i] << " ";
	}
	cout << endl;

	cout << "here" << endl;	
	insercaoMaisBarata();
		
	

}

Heuristica::~Heuristica(){
	

}
