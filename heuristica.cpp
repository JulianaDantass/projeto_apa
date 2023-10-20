#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>

#define QTD_VIZINHANCAS 3

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregasRealizadas = 0;
	this->funcaoObjetiva = 0;
	this->clientesAtendidos = 0;

	for(int i = 1; i < (dados->q_clientes) + 1; i++){
		clientesOrdenados.push_back(i);
	}

	veiculos = new vector <Veiculo*>(dados->q_veiculos);
	
	for(Veiculo *s : *veiculos){
		s = NULL;
	}
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



int Heuristica::solucaoInicial(int indice_veiculo){

	veiculos->at(indice_veiculo) = new Veiculo(indice_veiculo, dados->capacidade, dados->q_clientes, dados->custo_veiculo);
	Veiculo* s = veiculos->at(indice_veiculo);

	s->setCliente(0, 0); // Começa a rota 0->0
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
		
		if(inseriu == 2 or clientesOrdenados.back() == 0)
			break;

		/* Ordenacao dos clientes em ordem do maior para o menor em relacao ao cliente anterior	
			* A ordenacao eh feita em nlog(n)*/
		std::sort(clientesOrdenados.begin(), clientesOrdenados.end(),[&](const int a, const int b){
				return dados->matriz_distancias[a][cliente_anterior] <= dados->matriz_distancias[b][cliente_anterior];
			});
		
		
		
		cout << "Ordenacao dos clientes em relacao a: " << cliente_anterior << endl;

		for(int i = 0; i < clientesOrdenados.size(); i++){
			cout << clientesOrdenados[i] << " ";
		}
		cout << endl;
		
		
		
		int cliente_indice = clientesOrdenados.back(); // Retirada do mais pŕoximo	
		
	
		if(entregasRealizadas >= dados->minimo_entregas and clientesOrdenados.size() == 1){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
			
			// Calculo da terceirizao de ida e volta supondo o melhor caso q é direto para garagem	
			int custoIdaTotal = custoIda(cliente_anterior, cliente_indice, s->getObjetivo());// calculo do custo de ida
			
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
				this->funcaoObjetiva += custo_terceirizacao_cliente; // Adiciona a terceirizacao
				clientesOrdenados.pop_back(); // Retira dos clientes
				clientesAtendidos++; // Diz que o cliente foi atendido
				continue;
			}
		}
		
		
		/* Verifica se o veiculo tem capacidade para suprir a demanda	*/

		int demanda_cliente = dados->demandas[cliente_indice]; 
		int capacidade = s->getCapacidade();
		if(demanda_cliente <= capacidade){
			
			int novaCapacidade = capacidade - demanda_cliente;
			
			funcaoObjetivo = custoIda(cliente_anterior, cliente_indice, s->getObjetivo());

			s->setCliente(cliente_indice, cliente_anterior); // Seta o cliente como sendo o proximo na rota
			s->setObjetivo(funcaoObjetivo);     // Seta a nova funcao objetivo
			s->setCapacidade(novaCapacidade);  // Seta nova capacidade
			inseriu++; // Variavel que indica se ja foi inserido na rota
			entregasRealizadas++; // Armaneza quantas entregas ja foi realizada
			clientesOrdenados.pop_back(); // Retira o cliente, pois ja foi alocado
			cliente_anterior = cliente_indice;	 // Atualiza o cliente de referencia
			clientesAtendidos++;
			//getchar();	
		}
		
	}
	
	
	cout << "Capacidade do veiculo após solucao inicial: " << s->getCapacidade() << endl;
	/* Se o ultimo cliente for zero, é porque nao foi adicionado ninguem na solucao inicial	*/
	if(s->getProxCliente(cliente_anterior) != 0){
		funcaoObjetivo = custoIda(cliente_anterior, 0, s->getObjetivo());
		s->setCliente(0, cliente_anterior);
		s->insereCaminhoFim(0);
		s->setObjetivo(funcaoObjetivo);
		return 1;
	}else{
		return -1;
	}
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
		int resultado = solucaoInicial(i); //Constroi a solução inicial
		cout << endl << "------------------------------------------------------------------" << endl;
		
		/* Quer dizer que nao foi construido uma solucao inicial para o veiculo
		 * seja porque a terceirizacao foi melhor */
		if(resultado == -1){
			delete veiculos->at(i);
			veiculos->at(i) = NULL;
			continue;
		}
		
		veiculos->at(i)->setCustoVeiculo();
		cout << "Custo total apos primeira solucao: " << veiculos->at(i)->getObjetivo() << endl;
		
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
			 * e armazenar o melhor vértice para inserir o cliente atual
			 * Começamos  o cliente A sendo zero que é a garagem
			 * o caminho está como uma árvore logo vetor[a] = b, então 
			 * o nó a possui o nó b como pai, paramos quando o pai for a garagem que vale zero*/
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
				this->funcaoObjetiva += custo_terceirizacao_cliente;
				clientesTerceirizados.push_back(cliente);
				clientesAtendidos++;
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
				clientesAtendidos++;
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
			//	getchar();
			}
		}
		s->printaCaminhoTotal(0);
		cout << endl;
		this->funcaoObjetiva += s->getObjetivo();

	//	getchar();

	}
	
	for(int  i = 0; i < veiculos->size(); i++){

		Veiculo* s = veiculos->at(i);
		if(s == NULL)
			continue;
		
		cout << "Objetivo do veiculo: " << s->getObjetivo() << endl;
		cout << "Caminho do veiculo( " << i << "):";	
		s->printaCaminhoTotal(0);
		cout << endl;
	}
	
	cout << "Clientes terceirizados: ";
	for(int i = 0; i < clientesTerceirizados.size(); i++){

		cout << clientesTerceirizados[i] << " ";
	}
	cout << endl;
	cout << "Custo total apos o guloso: " << this->funcaoObjetiva << endl;
	
	if(dados->q_clientes == clientesAtendidos)
		cout << "Todos os clientes foram atendidos" << endl;
	
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
