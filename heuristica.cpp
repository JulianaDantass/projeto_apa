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
	
	for(int i = 1; i < clientes + 1; i++){

		clientesOrdenados.push_back(i);
	}

	solucoes = new vector < Solucao * >(qVeiculos);

	
}

void Heuristica::solve(){
	
	/* Ordenacao dos clientes em ordem do maior para o menor em relacao a distancia do deposito	
	 * A ordenacao eh feita em nlog(n)*/

	std::sort(clientesOrdenados.begin(), clientesOrdenados.end(),[&](const int a, const int b){
				return (*matriz_distancia)[a][0] <= (*matriz_distancia)[b][0];
			});


	
		
	
	/* Inicializa as soluções para cada veiculo, para cada veiculo
	 * pegamos os dois primeiros pontos mais distantes em relacao ao deposito*/
	for(int i = 0; i < qVeiculos; i++){
		
		solucoes->at(i) = new Solucao(i, capacidade);
		int capacidade = solucoes->at(i)->getCapacidade(); 
		int inseriu = 0;
		/* Tanto back quanto pop_back são O(1) assim como a funcao de inserir no caminho
		 * geralmente essa funcao é executada em O(2) ao menos que existe um cliente
		 * com a demanda maior do que a capacidade do veiculo na insercao inicial,
		 * entao no pior caso eh O(n)*/
		for(int j = 0; j < clientes + 1; j++){

			int cliente_indice = clientesOrdenados.back();
			int demanda_cliente = this->demanda->at(cliente_indice);
			if (demanda_cliente <= capacidade){
				solucoes->at(i)->insereCaminhoFim(cliente_indice);
				int nova_capacidade = capacidade - demanda_cliente;
				solucoes->at(i)->setCapacidade(nova_capacidade);
				clientesOrdenados.pop_back();
				inseriu++;
			}

			if(inseriu == 2){
				break;
			}
		}
	}
	
	for(int i = 0; i < qVeiculos; i++){

		vector < int > caminho = *(solucoes->at(i)->getCaminho());

		cout << "Caminho do veiculo " <<  i << endl;

		for(int i = 0; i < caminho.size(); i++){

			cout << caminho[i] << " ";
		}
		cout << endl;
	}
	getchar();

	/* Vamos paralelizar a insercao do mais proximo
	 * Temos que fazer o calculo para todos os veiculos
	 * e armazenar a solucao na melhor rota possivel dos veiculos*/
	/* Inicialmente vamos ordenar os clientes em ordem nao crescente
	 * em relacao ao deposito para construir a solucao inicial */
	for(int i = 1; i < clientes + 1; i++){
		
	}
}

Heuristica::~Heuristica(){
	

}
