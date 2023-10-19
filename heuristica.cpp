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
	
	for(int i = 1; i < clientes + 1; i++){

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



void Heuristica::solucaoInicial(int indice_veiculo, vector < bool > &clientesQueue){

	veiculos->at(indice_veiculo) = new Veiculo(indice_veiculo, capacidade, clientes);
	Veiculo* s = veiculos->at(indice_veiculo);
	s->insereCaminhoFim(0);

	int inseriu = 0;
	int funcaoObjetivo = 0;
	
	/* Geralmente vai funcionar em O(2) no pior caso será O(n)
	 * Porém a complexidade justa é O(2)	*/
	int cliente_anterior = 0;

	for(int j = 1; j < clientes + 1; j++){
		
		// Vamos pegar os mais distantes entre si
		// Como o cliente anterior inicalmente eh o deposito
		// pegamos o primeiro mais distante ao deposito e assim por diante
		

		/* Ordenacao dos clientes em ordem do maior para o menor em relacao a distancia do deposito	
			* A ordenacao eh feita em nlog(n)*/

		std::sort(clientesOrdenados.begin(), clientesOrdenados.end(),[&](const int a, const int b){
				return (*matriz_distancia)[a][cliente_anterior] <= (*matriz_distancia)[b][cliente_anterior];
			});
		

		cout << "Ordenacao dos clientes em relacao a: " << cliente_anterior << endl;

		for(int i = 0; i < clientesOrdenados.size(); i++){
			cout << clientesOrdenados[i] << " ";
		}
		cout << endl;


		int cliente_indice = clientesOrdenados.back();


		int demanda_cliente = this->demanda->at(cliente_indice - 1); // Damos -1 pois o vetor de demanda começa em zero
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
			cliente_anterior = cliente_indice;	
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
	
	vector < bool > clientesQueue;

	for(int i = 0; i < clientesOrdenados.size(); i++){
			clientesQueue.push_back(true);
	}


	for(int i = 0; i < qVeiculos; i++){
			
		
		cout << "Solucao inicial para o veiculo: " << i << endl;
		cout << "-----------------Gerando Solucao Inicial----------------------------------" << endl;
		solucaoInicial(i, clientesQueue); //Constroi a solução inicial
		cout << endl << "------------------------------------------------------------------" << endl;
			
		int quantiaClientes = clientesOrdenados.size();
		Veiculo* s = veiculos->at(i);
		
		s->printaCaminhoTotal(0);
		getchar();
		cout << "Quantia clientes: " << quantiaClientes << endl;

		for(int v = 0; v < quantiaClientes; v++){
			int cliente = clientesOrdenados[v];
			
			// Se um cliente já foi inserido na rota passamos para outro cliente
			if(!clientesQueue[v])
				continue;
			
			if(s->getCapacidade() <= 0)
				continue;

			vector < int> *caminhoTotal = veiculos->at(i)->getCaminhoTotal();
			int demanda_cliente = this->demanda->at(cliente - 1);
			
			cout << "Demanda do cliente " << cliente << " " << demanda_cliente << endl;
			if(demanda_cliente > s->getCapacidade()){

				cout << "Nao eh possivel testar o cliente: " << cliente << " pois sua demanda eh maior do que a capacidade" << endl;
				continue;
			}
			
			int melhorCliente_a = 0;
			int melhorCliente_b = 0;
			int custo = 0;
			int melhorCusto = 999999;

			int clienteA = 0;
			while(1){
				int clienteB = (*caminhoTotal)[clienteA];
				cout << "Teste de insercao do cliente " << cliente << " Entre os clientes " << clienteA << " " << clienteB << endl;
				cout << "Custo do rota " << clienteA << " e " << cliente << " :" << (*matriz_distancia)[clienteA][cliente] << endl;
				cout << "Custo da rota " << clienteB << " e " << cliente << " :" << (*matriz_distancia)[clienteB][cliente] << endl;
				cout << "Custo da antiga rota: " << (*matriz_distancia)[clienteA][clienteB] << endl;
				custo = (*matriz_distancia)[clienteA][cliente] + (*matriz_distancia)[clienteB][cliente] - (*matriz_distancia)[clienteA][clienteB];
				cout << "Custo de insercao: " << custo << endl;
				if (custo < melhorCusto){

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

			if(melhorCusto != 999999){
				cout << "-------------------------------Iniciando Insercao---------------------------------" << endl;
				s->setCliente(cliente, melhorCliente_a);
				s->setCliente(melhorCliente_b, cliente);
				cout << "O melhor custo foi de: " << melhorCusto << endl;
				cout << "A insercado do cliente: " << cliente << " entre " << melhorCliente_a << " " << melhorCliente_b << endl;

				int funcaoObjetivo = s->getObjetivo() + melhorCusto; // O melhor custo vai ser negativo
				s->setObjetivo(funcaoObjetivo);
				int novaCapacidade = s->getCapacidade() - demanda_cliente;
				s->setCapacidade(novaCapacidade);
				cout << "Capacidade do veiculo apos insercao: " << s->getCapacidade() << endl;
				clientesQueue[v] = false;
				cout << "Novo caminho: ";
				s->printaCaminhoTotal(0);
				cout << endl;
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
				getchar();
			}
		}
		s->printaCaminhoTotal(0);

		getchar();

	}



}
void Heuristica::solve(){
	

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
