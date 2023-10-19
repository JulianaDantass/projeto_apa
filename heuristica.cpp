#include "heuristica.h"
#include <algorithm>
#include <iostream>

#define QTD_VIZINHANCAS 3

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

	veiculos = new vector < Veiculo * >(qVeiculos);

	
}

int Heuristica::calculaObjetivo(int cliente_anterior, int cliente_atual, int objetivo){
	cout << "Cliente anterior: " << cliente_anterior << endl;
	cout << "cliente atual: " << cliente_atual << endl;
	cout << "Custo da aresta: " << (*matriz_distancia)[cliente_anterior][cliente_atual] << endl;

	int funcaoObjetivo = objetivo + (*matriz_distancia)[cliente_anterior][cliente_atual];
	cout << "FuncaoObjetivo apos o calculo: " << funcaoObjetivo << endl;	
	return funcaoObjetivo;
}



void Heuristica::solucaoInicial(int indice_veiculo){

	veiculos->at(indice_veiculo) = new Veiculo(indice_veiculo, capacidade, clientes);
	Veiculo* s = veiculos->at(indice_veiculo);
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
			
			s->setCliente(cliente_indice, s->getUltimoCliente());
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
	s->setCliente(0, s->getUltimoCliente());
	s->insereCaminhoFim(0);
	s->setObjetivo(funcaoObjetivo);
	cout << "Capacidade do veiculo após solucao inicial: " << s->getCapacidade() << endl;

}



/* A insercao do mais proximo
 * será realizada inicalmente um veiculo por vez
 * pegamos inicialmente uma solucao inicial formada por 2 vertices mais o deposito
 * e fazemos isso para todo veiculo	*/

void Heuristica::insercaoMaisBarata(){
	
	for(int i = 0; i < qVeiculos; i++){
		
		solucaoInicial(i); //Constroi a solução inicial
		

		// For responsável para verificar qual o menor custo de insercao
		int quantiaClientes = clientesOrdenados.size();
		int capacidadeVeiculo = veiculos->at(i)->getCapacidade();
		Veiculo* s = veiculos->at(i);
		
		s->printaCaminhoTotal(0);
		getchar();
		if (capacidadeVeiculo <=0)
			continue;
		
		cout << "Quantia clientes: " << quantiaClientes << endl;

		for(int v = 1; v < quantiaClientes; v++){
			int cliente = clientesOrdenados[v];
			
			vector < int> *caminhoTotal = veiculos->at(i)->getCaminhoTotal();

			
			int melhorCliente_a = 0;
			int melhorCliente_b = 0;
			int custo = 0;
			int melhorCusto = 0;

			int clienteA = 0;
			while(1){
				int clienteB = (*caminhoTotal)[clienteA];
				cout << "Teste de insercao do cliente " << cliente << " Entre os clientes " << clienteA << " " << clienteB << endl;
				custo = (*matriz_distancia)[clienteA][cliente] + (*matriz_distancia)[clienteB][cliente] - (*matriz_distancia)[clienteA][clienteB];
				cout << "Custo de: " << custo << endl;
				if (custo < 0 and custo < melhorCusto){

					melhorCusto = custo;
					melhorCliente_a = clienteA;
					melhorCliente_b = clienteB;
				}
				if (clienteB == 0){
					break;
				}
				clienteA = clienteB;
				getchar();
			}

			if(melhorCusto != 0){
				//SetCliente(proximoCliente, clienteAnterior)
				s->setCliente(cliente, melhorCliente_a);
				s->setCliente(melhorCliente_b, cliente);
			
				int funcaoObjetivo = s->getObjetivo() + melhorCusto; // O melhor custo vai ser negativo
				s->setObjetivo(funcaoObjetivo);
			}
		}
		s->printaCaminhoTotal(0);

		getchar();

	}



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

// void Heuristica::VND(){

// 	for(int i = 0; i < qtd_rotas; i++){

// 		int contador = 0; 
// 		double solucao_vizinhanca = solucao_original;
// 		while(contador < QTD_VIZINHANCAS){

// 			switch(contador){
// 				case 0:
// 					swap();
// 				case 1: 
// 					reinsertion();
// 				case 2:
// 					swap_entre_rotas();
// 			}

// 			if(solucao_vizinhanca < solucao_original){            //se a vizinhanca melhorar a solucao
// 				solucao_original = solucao_vizinhanca;
// 				contador = 0;

// 			}else{
// 				contador++;
// 			}
// 		}
// 	}	
// }

Heuristica::~Heuristica(){
	

}
