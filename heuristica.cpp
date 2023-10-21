#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>

using namespace std;
#define QTD_VIZINHANCAS 3

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregasRealizadas = 0;
	this->funcaoObjetivo = 0;
	this->clientesAtendidos = 0;

	for(int i = 1; i < (dados->q_clientes) + 1; i++){
		clientesOrdenados.push_back(i);
	}

	//veiculos.resize(dados->q_veiculos+1);
	
	// for(Veiculo *s : *veiculos){
	// 	s = NULL;
	// }
}


/* Calculo responsável por realizar o calculo de um cliente a outro no grafo	*/
int Heuristica::custoIda(int cliente_anterior, int cliente_atual, int objetivo){

	cout << "Cliente anterior: " << cliente_anterior << endl;
	cout << "cliente atual: " << cliente_atual << endl;
	cout << "Custo da aresta: " << dados->matriz_distancias[cliente_anterior][cliente_atual] << endl;

	int funcaoObjetivo = objetivo + dados->matriz_distancias[cliente_anterior][cliente_atual];
	cout << "FuncaoObjetivo apos o calculo: " << funcaoObjetivo << endl;	
	return funcaoObjetivo;
}



Veiculo Heuristica::solucaoInicial(int indice_veiculo){

	Veiculo veiculo;

	veiculo.editaParametros(indice_veiculo, dados->capacidade, dados->q_clientes, dados->custo_veiculo);


	veiculo.setCliente(0, 0); // Começa a rota 0->0
	int inseriu = 0;
	int funcaoObjetivo = 0;
	int cliente_anterior = 0;

	/* Esse loop vai pegar um cliente inicial aleatoriamente
	 * Caso eu tenha realizado as entregas minimas
	 * Verificamos se vale a pena terceirizar ou não
	 * Se o custo de ida mais o custo da volta a garagem (melhor cenario possivel)
	 * for maior ou igual a terceirizacao, é melhor terceirizar	
	 * Sempre tentamos criar uma rota, se a gente insere uma vez quebramos o loop*/
		
	while(!clientesOrdenados.empty()){	
		if(inseriu == 1)
			break;

		srand((unsigned)time(NULL));
		int n = rand() % clientesOrdenados.size(); // Gera um indice aleatorio
		int cliente_indice = clientesOrdenados[n]; // Retira um cliente aleatorio
		
		
		cout << "indice escolhido: " << n << endl;
		cout << "Cliente escolhido: " << cliente_indice << endl;
		/*
		cout << "Clientes antes da escolha: ";
		for(int  i = 0; i < clientesOrdenados.size(); i++){
			cout << clientesOrdenados[i] << " ";
		}
		*/

		cout << endl;
		if(entregasRealizadas >= dados->minimo_entregas){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
					
			// Calculo da terceirizao de ida e volta supondo o melhor caso q é direto para garagem	
			int custoIdaTotal = custoIda(cliente_anterior, cliente_indice, veiculos[indice_veiculo].getObjetivo());// calculo do custo de ida
				
			/*
				Verificamos se estamos inserindo o primeiro cliente
				* caso sim verificamos se vale a pena pagar o custo do veiculo mais o custo da distancia de ida ao cliente
				* ate a garagem, caso seja o segundo cliente nao precisamos mais considerar o custo do veiculo pois sabemos que ele será utilizado*/

			int custoPrimeiroCliente = (inseriu == 0 ? dados->matriz_distancias[cliente_indice][0] + dados->custo_veiculo : dados->matriz_distancias[cliente_indice][0]); 
			custoIdaTotal += custoPrimeiroCliente;
			cout << "Custo da rota: " << custoIdaTotal << endl;
			cout << "Custo de terceiriza: " << custo_terceirizacao_cliente << endl;
			if(custo_terceirizacao_cliente <= custoIdaTotal){
				cout << "Cliente " << cliente_indice << " foi tercerizado" << endl;
				clientesTerceirizados.push_back(cliente_indice); // Indica o cliente terceirizado
				this->funcaoObjetivo += custo_terceirizacao_cliente; // Adiciona a terceirizacao
				/*Excluir o cliente gerado */
				int ultimo_cliente = clientesOrdenados[clientesOrdenados.size() - 1]; //Armazena o ultimo cliente
				clientesOrdenados[clientesOrdenados.size() - 1] = cliente_indice; // O cliente escolhido aleatoriamente vai para a posicao final
				clientesOrdenados[n] = ultimo_cliente; // Atualiza a posicao atual com o ultimo cliente
				clientesOrdenados.pop_back(); // Retira o cliente, pois ja foi alocado
				/* Fim da eliminacao do cliente */
				clientesAtendidos++; // Diz que o cliente foi atendido
			}
		}
		else{	
				
			cout << "here" << endl;
			/* Verifica se o veiculo tem capacidade para suprir a demanda	*/
			int demanda_cliente = dados->demandas[cliente_indice]; 
			int capacidade = veiculo.getCapacidade();
			if(demanda_cliente <= capacidade){
					
				int novaCapacidade = capacidade - demanda_cliente;
					
				funcaoObjetivo = custoIda(cliente_anterior, cliente_indice, veiculo.getObjetivo());

				veiculo.setCliente(cliente_indice, cliente_anterior); // Seta o cliente como sendo o proximo na rota
				veiculo.setObjetivo(funcaoObjetivo);     // Seta a nova funcao objetivo
				veiculo.setCapacidade(novaCapacidade);  // Seta nova capacidade
				entregasRealizadas++; // Armaneza quantas entregas ja foi realizada
				/*Excluir o cliente gerado */
				int ultimo_cliente = clientesOrdenados[clientesOrdenados.size() - 1]; //Armazena o ultimo cliente
				clientesOrdenados[clientesOrdenados.size() - 1] = cliente_indice; // O cliente escolhido aleatoriamente vai para a posicao final
				clientesOrdenados[n] = ultimo_cliente; // Atualiza a posicao atual com o ultimo cliente
				clientesOrdenados.pop_back(); // Retira o cliente, pois ja foi alocado
				/* Fim da eliminacao do cliente */
				clientesAtendidos++;
				inseriu++;
				cliente_anterior = cliente_indice;
					
			}
			
		}
		/*
		cout << "Clientes apos escolha: ";
		for(int  i = 0; i < clientesOrdenados.size(); i++){
			cout << clientesOrdenados[i] << " ";
		}
		*/
		
	}
	
	
	
	cout << "Capacidade do veiculo após solucao inicial: " << veiculo.getCapacidade() << endl;
	/* Se o ultimo cliente for zero, é porque nao foi adicionado ninguem na solucao inicial	*/
	
	funcaoObjetivo = custoIda(cliente_anterior, 0, veiculo.getObjetivo());
	veiculo.setCliente(0, cliente_anterior);
	veiculo.setObjetivo(funcaoObjetivo);
	
	return veiculo;
	
}



/* A insercao do mais proximo
 * será realizada com um veiculo por vez
 * pegamos inicialmente uma solucao inicial formada por 2 vertices + deposito
 * e calculamos a insercao mais barata*/

void Heuristica::insercaoMaisBarata(){
		
	for(int i = 0; i < dados->q_veiculos; i++){
				
		int quantiaClientes = clientesOrdenados.size();
		
		/* Verificando se ainda existe cliente para suprir	*/
		if(clientesOrdenados.size() == 0)
			break;

		cout << "Solucao inicial para o veiculo: " << i << endl;
		cout << "-----------------Gerando Solucao Inicial----------------------------------" << endl;
		Veiculo veiculo = solucaoInicial(i); //Constroi a solução inicial
		cout << endl << "------------------------------------------------------------------" << endl;
		
		/* Quer dizer que nao foi construido uma solucao inicial para o veiculo
		 * seja porque a terceirizacao foi melhor */
		 if(veiculo.getProxCliente(0) == 0){
		 	continue;
		 }
		
		veiculos.push_back(veiculo);

		veiculos[i].setCustoVeiculo();
		cout << "Custo total apos primeira solucao: " << veiculos[i].getObjetivo() << endl;
		
		cout << "Clientes apos a solucao Inicial: ";

		for(int i = 0; i < clientesOrdenados.size(); i++){

			cout << clientesOrdenados[i] << " ";
		}
		cout << endl;
		
		
		veiculos[i].printaCaminhoTotal(0);
		
		/* O loop começa do ultimo cliente no vetor
		 * Se houver capacidade para aquele cliente
		 * calculamos o menor custo da insercao mais barata
		 * se for realizado a quantia minima de entregas
		 * temos que começar a verificar se o custo da rota real
		 * é maior ou igual ao custo da terceirizacao, porque se for
		 * é melhor terceirizar	*/

		for(int v = clientesOrdenados.size() - 1; v >= 0; v--){
			
			/* Se o veiculo não tiver capacidade, paramos o codigo	*/
			if( veiculos[i].getCapacidade() <= 0)
				break;

			int cliente = clientesOrdenados[v];
			
			for(int i = 0; i < clientesOrdenados.size(); i++){

				cout << clientesOrdenados[i] << " ";
			}
			
			/* Se a demanda do cliente for maior do que a capacidade, então pulamos para o proximo cliente	*/
			int demanda_cliente = dados->demandas[cliente];
			cout << "Demanda do cliente " << cliente << " " << demanda_cliente << endl;
			if(demanda_cliente >  veiculos[i].getCapacidade()){
				
				cout << "Nao eh possivel testar o cliente: " << cliente << " pois sua demanda eh maior do que a capacidade" << endl;
				continue;
			}
			
			
			/* O while é responsável por percorrer a arvore
			 * e armazenar o melhor vértice para inserir o cliente atual
			 * Começamos  o cliente A sendo zero que é a garagem
			 * o caminho está como uma árvore logo vetor[a] = b, então 
			 * o nó a possui o nó b como pai, paramos quando o pai for a garagem que vale zero*/
			vector < int> *caminhoTotal = veiculos[i].getCaminhoTotal();
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
			}
			
			/* Calculo do custo real do cliente
			 * que é ir do melhorCLiente_A até o cliente atual +
			 * ir do cliente atual ate o melhorCliente_b
			 * se esse calculo for maior ou igual ao custo de terceirizar
			 * iremos calaramente terceirizar*/
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
				this->funcaoObjetivo += custo_terceirizacao_cliente;
				clientesTerceirizados.push_back(cliente);
				clientesAtendidos++;
				continue;

			}

			if(melhorCusto != 999999){
				cout << "-------------------------------Iniciando Insercao---------------------------------" << endl;
				 veiculos[i].setCliente(cliente, melhorCliente_a); // Seta o pai de melhor_Cliente_a como cliente
				 veiculos[i].setCliente(melhorCliente_b, cliente); // Seta o pai de cliente como melhorCliente)b

				cout << "O melhor custo foi de: " << melhorCusto << endl;
				cout << "A insercado do cliente: " << cliente << " entre " << melhorCliente_a << " " << melhorCliente_b << endl;

				int funcaoObjetivo =  veiculos[i].getObjetivo() + melhorCusto;
				 veiculos[i].setObjetivo(funcaoObjetivo);
				cout << "Valor da nova funcao objetivo: " << funcaoObjetivo << endl;
				int novaCapacidade =  veiculos[i].getCapacidade() - demanda_cliente;
				 veiculos[i].setCapacidade(novaCapacidade);
				cout << "Capacidade do veiculo apos insercao: " <<  veiculos[i].getCapacidade() << endl;
				
				/* Troca a posicao com o ultimo e da o pop_back	*/
				int ultimo_cliente = clientesOrdenados[clientesOrdenados.size() - 1];
				clientesOrdenados[clientesOrdenados.size() - 1] = clientesOrdenados[v];
				clientesOrdenados[v] = ultimo_cliente;
				clientesOrdenados.pop_back();
				/*-------------------------------------------*/
				entregasRealizadas++;
				cout << "Novo caminho: ";
				 veiculos[i].printaCaminhoTotal(0);
				cout << endl;
				clientesAtendidos++;
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
			//	getchar();
			}
		}
		 veiculos[i].printaCaminhoTotal(0);
		cout << endl;
		this->funcaoObjetivo +=  veiculos[i].getObjetivo();

	//	getchar();

	}
	
	 for(int  i = 0; i < veiculos.size(); i++){
		
	 	cout << "Objetivo do veiculo: " << veiculos[i].getObjetivo() << endl;
	 	cout << "Caminho do veiculo( " << i << "):";	
	 	veiculos[i].printaCaminhoTotal(0);
	 	cout << endl;
	 }
	
	 cout << "Clientes terceirizados: ";
	 for(int i = 0; i < clientesTerceirizados.size(); i++){

	 	cout << clientesTerceirizados[i] << " ";
	 }
	 cout << endl;
	cout << "Custo total apos o guloso: " << this->funcaoObjetivo << endl;
	
	// if(dados->q_clientes == clientesAtendidos)
	// 	cout << "Todos os clientes foram atendidos" << endl;
	
}

void Heuristica::resolve(){

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
