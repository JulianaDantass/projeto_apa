#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>

#define QTD_VIZINHANCAS 3

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregasRealizadas = 0;
	this->funcaoObjetiva = 0;
}

void Heuristica::alocarRecursos(){
	
	for(int i = 1; i < (dados->q_clientes) + 1; i++){
		clientesOrdenados.push_back(i);
	}

	veiculos = new vector <Veiculo*>(dados->q_veiculos);
}

/* Calculo responsável por realizar o calculo de um cliente a outro no grafo	*/
int Heuristica::calculaObjetivoIda(int cliente_anterior, int cliente_atual, int objetivo){

	cout << "Cliente anterior: " << cliente_anterior << endl;
	cout << "cliente atual: " << cliente_atual << endl;
	cout << "Custo da aresta: " << dados->matriz_distancias[cliente_anterior][cliente_atual] << endl;

	int funcaoObjetivo = objetivo + dados->matriz_distancias[cliente_anterior][cliente_atual];
	cout << "FuncaoObjetivo apos o calculo: " << funcaoObjetivo << endl;	
	return funcaoObjetivo;
}



void Heuristica::solucaoInicial(int indice_veiculo){

	veiculos->at(indice_veiculo) = new Veiculo(indice_veiculo, dados->capacidade, dados->q_clientes, dados->custo_veiculo);
	Veiculo* s = veiculos->at(indice_veiculo);
	s->insereCaminhoFim(0);

	int inseriu = 0;
	int funcaoObjetivo = 0;
	
	int cliente_anterior = 0;

	/* Esse loop vai pegar um cliente inicial
	 * organizar os vetores do maior para o menor em relacao e ele
	 * E pegamos o mais perto dele.
	 * Caso eu tenha realizado as entregas minimas
	 * Verificamos se vale a pena terceirizar ou não
	 * Se o custo de ida mais o custo da volta a garagem (melhor cenario possivel)
	 * for maior ou igual a terceirizacao, é melhor terceirizar	*/

	while(!clientesOrdenados.empty()){
		
		if(inseriu == 2)
			break;

		/* Ordenacao dos clientes em ordem do maior para o menor em relacao ao cliente anterior	
			* A ordenacao eh feita em nlog(n)*/
		std::sort(clientesOrdenados.begin(), clientesOrdenados.end(),[&](const int a, const int b){
				return dados->matriz_distancias[a][cliente_anterior] >= dados->matriz_distancias[b][cliente_anterior];
			});
		

		cout << "Ordenacao dos clientes em relacao a: " << cliente_anterior << endl;

		for(int i = 0; i < clientesOrdenados.size(); i++){
			cout << clientesOrdenados[i] << " ";
		}
		cout << endl;

		
		
		int cliente_indice = clientesOrdenados.back(); // Retirada do mais pŕoximo	

		if(entregasRealizadas >= dados->minimo_entregas){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
			
			/* Calculo da terceirizao de ida e volta supondo o melhor caso q é direto para garagem	*/
			int funcaoObjetivoCalculoIda = calculaObjetivoIda(s->getUltimoCliente(), cliente_indice, s->getObjetivo());// calculo do custo de ida
			funcaoObjetivoCalculoIda += dados->matriz_distancias[cliente_indice][0] + dados->custo_veiculo;
			cout << "Custo da rota: " << funcaoObjetivoCalculoIda << endl;
			cout << "Custo de terceiriza: " << custo_terceirizacao_cliente << endl;
			if(custo_terceirizacao_cliente <= funcaoObjetivoCalculoIda){
				cout << "Cliente " << cliente_indice << " foi tercerizado" << endl;
				clientesTerceirizados.push_back(cliente_indice);
				this->funcaoObjetiva += custo_terceirizacao_cliente;
				clientesOrdenados.pop_back();
				continue;
			}
		}
			
		/* Verifica se o veiculo tem capacidade para suprir a demanda	*/

		int demanda_cliente = dados->demandas[cliente_indice]; 
		int capacidade = s->getCapacidade();
		if(demanda_cliente <= capacidade){
			
			int novaCapacidade = capacidade - demanda_cliente;
		
			funcaoObjetivo = calculaObjetivoIda(s->getUltimoCliente(), cliente_indice, s->getObjetivo()); // Calculo da Ida
			s->setCliente(cliente_indice, s->getUltimoCliente()); // Seta o cliente como sendo o proximo na rota
			s->insereCaminhoFim(cliente_indice); // Insere no fim do caminho do vetor 
			s->setObjetivo(funcaoObjetivo);     // Seta a nova funcao objetivo
			s->setCapacidade(novaCapacidade);  // Seta nova capacidade
			inseriu++; // Variavel que indica se ja foi inserido na rota
			entregasRealizadas++; // Armaneza quantas entregas ja foi realizada
			clientesOrdenados.pop_back(); // Retira o cliente, pois ja foi alocado
			cliente_anterior = cliente_indice;	 // Atualiza o cliente de referencia
			//getchar();	
		}
		
	}
	
	funcaoObjetivo = calculaObjetivoIda(s->getUltimoCliente(), 0, s->getObjetivo());
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
		
	for(int i = 0; i < dados->q_veiculos; i++){
				
		int quantiaClientes = clientesOrdenados.size();
		
		/* Verificando se ainda existe cliente para suprir	*/
		if(clientesOrdenados.size() == 0)
			break;

		cout << "Solucao inicial para o veiculo: " << i << endl;
		cout << "-----------------Gerando Solucao Inicial----------------------------------" << endl;
		solucaoInicial(i); //Constroi a solução inicial
		cout << endl << "------------------------------------------------------------------" << endl;
		
		/* Quer dizer que nao foi construido uma solucao inicial para o veiculo
		 * seja porque a terceirizacao foi melhor */
		if((veiculos->at(i)->getCaminhoInicial())->size() == 0)
			continue;

		cout << "Clientes apos a solucao Inicial: ";

		for(int i = 0; i < clientesOrdenados.size(); i++){

			cout << clientesOrdenados[i] << " ";
		}
		cout << endl;

		Veiculo* s = veiculos->at(i);
		
		s->printaCaminhoTotal(0);
	//	getchar();
		
		/* O loop começa do ultimo cliente no vetor
		 * Se houver capacidade para aquele cliente
		 * calculamos o menor custo da insercao mais barata
		 * se for realizado a quantia minima de entregas
		 * temos que começar a verificar se o custo da rota real
		 * é maior ou igual ao custo da terceirizacao, porque se for
		 * é melhor terceirizar	*/

		for(int v = clientesOrdenados.size() - 1; v >= 0; v--){
			
			/* Se o veiculo não tiver capacidade, paramos o codigo	*/
			if(s->getCapacidade() <= 0)
				break;

			int cliente = clientesOrdenados[v];
			
			for(int i = 0; i < clientesOrdenados.size(); i++){

				cout << clientesOrdenados[i] << " ";
			}
			
			
			/* Se a demanda do cliente for maior do que a capacidade, então pulamos para o proximo cliente	*/
			int demanda_cliente = dados->demandas[cliente];
			cout << "Demanda do cliente " << cliente << " " << demanda_cliente << endl;
			if(demanda_cliente > s->getCapacidade()){
				
				cout << "Nao eh possivel testar o cliente: " << cliente << " pois sua demanda eh maior do que a capacidade" << endl;
				continue;
			}
			
			
			/* O while é responsável por percorrer a arvore
			 * e armazenar o melhor vértice para inserir o cliente atual	*/
			vector < int> *caminhoTotal = veiculos->at(i)->getCaminhoTotal();
			int melhorCliente_a = 0;
			int melhorCliente_b = 0;
			int custo = 0;
			int melhorCusto = 999999;

			int clienteA = 0;
			while(1){
				int clienteB = (*caminhoTotal)[clienteA];
				cout << "Teste de insercao do cliente " << cliente << " Entre os clientes " << clienteA << " " << clienteB << endl;
				cout << "Custo do rota " << clienteA << " e " << cliente << " :" << dados->matriz_distancias[clienteA][cliente] << endl;
				cout << "Custo da rota " << clienteB << " e " << cliente << " :" << dados->matriz_distancias[clienteB][cliente] << endl;
				cout << "Custo da antiga rota: " << dados->matriz_distancias[clienteA][clienteB] << endl;
				custo = dados->matriz_distancias[clienteA][cliente] + dados->matriz_distancias[clienteB][cliente] - dados->matriz_distancias[clienteA][clienteB];
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
				//getchar();
			}
			
			/* Calculo do custo real do cliente
			 * que é ir do melhorCLiente_A até o cliente atual +
			 * ir do cliente atual ate o melhorCliente_b*/
			int custoDeEntradaCliente = melhorCusto + dados->matriz_distancias[melhorCliente_a][melhorCliente_b];
			
			/* Se o custo da terceirização for menor igual a esse custo a gente terceiriza	*/
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente];
			if(custo_terceirizacao_cliente <= custoDeEntradaCliente and entregasRealizadas >= dados->minimo_entregas){
				cout << "O cliente tem custo de terceirizacao melhor, pois vale: " << custo_terceirizacao_cliente << endl;
				/* Trocamos de posicao no vetor para conseguir deletar em O(1)	*/
				int ultimo_cliente = clientesOrdenados[clientesOrdenados.size()  - 1];
				clientesOrdenados[clientesOrdenados.size() - 1] = clientesOrdenados[v];
				clientesOrdenados[v] = ultimo_cliente;	
				clientesOrdenados.pop_back();
				/*--------------------------------------------------------------*/
				this->funcaoObjetiva += custo_terceirizacao_cliente;
				clientesTerceirizados.push_back(cliente);
				continue;

			}

			if(melhorCusto != 999999){
				cout << "-------------------------------Iniciando Insercao---------------------------------" << endl;
				s->setCliente(cliente, melhorCliente_a); // Seta o pai de melhor_Cliente_a como cliente
				s->setCliente(melhorCliente_b, cliente); // Seta o pai de cliente como melhorCliente)b

				cout << "O melhor custo foi de: " << melhorCusto << endl;
				cout << "A insercado do cliente: " << cliente << " entre " << melhorCliente_a << " " << melhorCliente_b << endl;

				int funcaoObjetivo = s->getObjetivo() + melhorCusto;
				s->setObjetivo(funcaoObjetivo);
				cout << "Valor da nova funcao objetivo: " << funcaoObjetivo << endl;
				int novaCapacidade = s->getCapacidade() - demanda_cliente;
				s->setCapacidade(novaCapacidade);
				cout << "Capacidade do veiculo apos insercao: " << s->getCapacidade() << endl;
				
				/* Troca a posicao com o ultimo e da o pop_back	*/
				int ultimo_cliente = clientesOrdenados[clientesOrdenados.size() - 1];
				clientesOrdenados[clientesOrdenados.size() - 1] = clientesOrdenados[v];
				clientesOrdenados[v] = ultimo_cliente;
				clientesOrdenados.pop_back();
				/*-------------------------------------------*/
				entregasRealizadas++;
				cout << "Novo caminho: ";
				s->printaCaminhoTotal(0);
				cout << endl;
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
			//	getchar();
			}
		}
		s->printaCaminhoTotal(0);
		cout << endl;
		this->funcaoObjetiva += s->getObjetivo();

	//	getchar();

	}

	cout << "Custo total apos o guloso: " << this->funcaoObjetiva << endl;

	
}

void Heuristica::solve(){
	
	alocarRecursos();
	auto inicio = std::chrono::high_resolution_clock::now();
	insercaoMaisBarata();	
	auto resultado = std::chrono::high_resolution_clock::now() - inicio;
	long long microsecond = std::chrono::duration_cast<std::chrono::microseconds>(resultado).count();
	cout << "Tempo(ms): " << microsecond << endl;
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
