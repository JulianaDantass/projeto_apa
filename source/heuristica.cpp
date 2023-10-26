#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 

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
}


/* Calculo responsável por realizar o calculo de um cliente a outro no grafo	*/
int Heuristica::custoIda(int cliente_anterior, int cliente_atual, int objetivo){

	//cout << "Cliente anterior: " << cliente_anterior << endl;
	//cout << "cliente atual: " << cliente_atual << endl;
	//cout << "Custo da aresta: " << dados->matriz_distancias[cliente_anterior][cliente_atual] << endl;

	int funcaoObjetivo = objetivo + dados->matriz_distancias[cliente_anterior][cliente_atual];
	//cout << "FuncaoObjetivo apos o calculo: " << funcaoObjetivo << endl;	
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
		/*
		if(entregasRealizadas >= dados->minimo_entregas){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
					
			// Calculo da terceirizao de ida e volta supondo o melhor caso q é direto para garagem	
			int custoIdaTotal = custoIda(cliente_anterior, cliente_indice, veiculos[indice_veiculo].getObjetivo());// calculo do custo de ida
				
			
				Verificamos se estamos inserindo o primeiro cliente
				* caso sim verificamos se vale a pena pagar o custo do veiculo mais o custo da distancia de ida ao cliente
				* ate a garagem, caso seja o segundo cliente nao precisamos mais considerar o custo do veiculo pois sabemos que ele será utilizado

			int custoPrimeiroCliente = (inseriu == 0 ? dados->matriz_distancias[cliente_indice][0] + dados->custo_veiculo : dados->matriz_distancias[cliente_indice][0]); 
			custoIdaTotal += custoPrimeiroCliente;
			cout << "Custo da rota: " << custoIdaTotal << endl;
			cout << "Custo de terceiriza: " << custo_terceirizacao_cliente << endl;
			if(custo_terceirizacao_cliente <= custoIdaTotal){
				cout << "Cliente " << cliente_indice << " foi tercerizado" << endl;
				clientesTerceirizados.push_back(cliente_indice); // Indica o cliente terceirizado
				this->funcaoObjetivo += custo_terceirizacao_cliente; // Adiciona a terceirizacao
				Excluir o cliente gerado */
				//int ultimo_cliente = clientesOrdenados[clientesOrdenados.size() - 1]; //Armazena o ultimo cliente
				//clientesOrdenados[clientesOrdenados.size() - 1] = cliente_indice; // O cliente escolhido aleatoriamente vai para a posicao final
				//clientesOrdenados[n] = ultimo_cliente; // Atualiza a posicao atual com o ultimo cliente
				//clientesOrdenados.pop_back(); // Retira o cliente, pois ja foi alocado
				/* Fim da eliminacao do cliente 
				clientesAtendidos++; // Diz que o cliente foi atendido
			}
		}
		*/
				
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
		// cout << "Custo total apos primeira solucao: " << veiculos[i].getObjetivo() << endl;
		// cout << "Clientes apos a solucao Inicial: ";

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
			//cout << "Demanda do cliente " << cliente << " " << demanda_cliente << endl;
			if(demanda_cliente >  veiculos[i].getCapacidade()){
				
				//cout << "Nao eh possivel testar o cliente: " << cliente << " pois sua demanda eh maior do que a capacidade" << endl;
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
			 * se esse calculo for maior que o  custo de terceirizar
			 * iremos calaramente terceirizar
			 */

			//int custoDeEntradaCliente = melhorCusto + dados->matriz_distancias[melhorCliente_a][melhorCliente_b];
			/* Se o custo da terceirização for menor igual a esse custo a gente terceiriza	*/
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente];
			if(custo_terceirizacao_cliente < melhorCusto and entregasRealizadas >= dados->minimo_entregas){

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

				//cout << "O melhor custo foi de: " << melhorCusto << endl;
				//cout << "A insercado do cliente: " << cliente << " entre " << melhorCliente_a << " " << melhorCliente_b << endl;

				int funcaoObjetivo =  veiculos[i].getObjetivo() + melhorCusto;
				 veiculos[i].setObjetivo(funcaoObjetivo);
				//cout << "Valor da nova funcao objetivo: " << funcaoObjetivo << endl;
				int novaCapacidade =  veiculos[i].getCapacidade() - demanda_cliente;
				 veiculos[i].setCapacidade(novaCapacidade);
				//cout << "Capacidade do veiculo apos insercao: " <<  veiculos[i].getCapacidade() << endl;
				
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
	
	cout << "Sobraram os clientes: " << clientesOrdenados.size() << endl;
	
	/* Caso sobre algum cliente, devemos terceirizar pois todos os veiculos estão preenchidos	*/
	for(int  i = 0; i < clientesOrdenados.size(); i++){
		int indice_cliente = clientesOrdenados[i];
		clientesTerceirizados.push_back(indice_cliente);
		this->funcaoObjetivo += dados->custo_terceirizacao[indice_cliente];
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
	VND();
	auto resultado = std::chrono::high_resolution_clock::now() - inicio;
	long long millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(resultado).count();
	cout << "Tempo(ms): " << millisecond << endl;
}

void Heuristica::VND(){

	int contador = 0; 
	bool melhorou_solucao = 0;
	while(contador < QTD_VIZINHANCAS){

		switch(contador){
			case 0:
				melhorou_solucao = reinsertion();
				break;
			case 1: 
				//reinsertion();
				break;
			case 2:
				//swap_entre_rotas();
				break;
		}

		if(melhorou_solucao){            //se a vizinhanca melhorar a solucao
			contador = 0;

		}else{
			contador++;
		}
	}

}

bool Heuristica::reinsertion(){

	bool houve_melhoria_rotas = 0;

	for(int v = 0; v < veiculos.size(); v++){

		std::vector<int> rota;
		std::vector<int> *ptr_rota = veiculos[v].getCaminhoTotal();


		int ponto_rota = 0;
		rota.push_back(0);
		while(1){ //colocando a rota num vector com APENAS os vertices visitidos de fato pelo veiculo para analisar os movimentos

			int cliente = (*ptr_rota)[ponto_rota];
			rota.push_back(cliente);

			if (cliente == 0){
				break;
			}else{
				ponto_rota = cliente;
			}
		}          

		cout << "\n\nVeiculo " << v << endl;
		cout << "rota antes do movimento: ";
		veiculos[v].printaCaminhoTotal(0);
		
		double delta, 
		melhor_delta = 0;         //vai guardar a diferenca de valor em relacao a solucao original        
		int melhor_i, melhor_j;       //vao guardar os indices de melhor troca

		for(int i = 1; i < rota.size(); i++){
			for(int j = 1; j < rota.size()-1; j++){

				if(i == j or i == j+1){
					continue;

				}else if (i > j){

					delta = -dados->matriz_distancias[rota[j-1]][rota[j]] -dados->matriz_distancias[rota[j]][rota[j+1]] 
							-dados->matriz_distancias[rota[i-1]][rota[i]] +dados->matriz_distancias[rota[j-1]][rota[j+1]] 
							+dados->matriz_distancias[rota[i]][rota[j]] +dados->matriz_distancias[rota[j]][rota[i-1]]; 

				}else if(i == j-1){

					delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[j]][rota[j+1]]
							+dados->matriz_distancias[rota[i-1]][rota[j]] +dados->matriz_distancias[rota[j-1]][rota[j+1]];

				}else if(i < j){
					
					delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[j-1]][rota[j]] 
							-dados->matriz_distancias[rota[j]][rota[j+1]] +dados->matriz_distancias[rota[i-1]][rota[j]] 
							+dados->matriz_distancias[rota[j]][rota[i]] +dados->matriz_distancias[rota[j-1]][rota[j+1]];
				}
				
				if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao

					melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
					melhor_i = i;          //guarda o indice i da melhor troca
					melhor_j = j;          //guarda o indice j da melhor troca
				}
			}
		}
	
		if (melhor_delta < 0){

			// cout << "mellhor i: " << melhor_i << " " << "melhor j: " << melhor_j << endl;
			// cout << "melhor delta: " << melhor_delta << endl;

			if(melhor_i > melhor_j){

				veiculos[v].setCliente(rota[melhor_j+1], rota[melhor_j-1]);      // o antecessor do melhor j vai apontar pro sucessor de j
				veiculos[v].setCliente(rota[melhor_j], rota[melhor_i-1]);           //o melhorj vai apontar pro melhor i
				veiculos[v].setCliente(rota[melhor_i], rota[melhor_j]);         //o melhor j vai apntar pro sucessor de i


				rota.insert(rota.begin() + melhor_i, rota[melhor_j]);                  
          		rota.erase(rota.begin() + melhor_j);                                   
				
			}else{

				veiculos[v].setCliente(rota[melhor_j], rota[melhor_i-1]);      
				veiculos[v].setCliente(rota[melhor_i], rota[melhor_j]);          
				veiculos[v].setCliente(rota[melhor_j+1], rota[melhor_j-1]);        
				
				rota.insert(rota.begin() + melhor_i, rota[melhor_j]);               
          		rota.erase(rota.begin() + melhor_j+1);                               
			}
			
			veiculos[v].setObjetivo( veiculos[v].getObjetivo() + melhor_delta );  //atualizando a funcao objetivo do veiculo
			funcaoObjetivo = funcaoObjetivo + melhor_delta;                       //atualizando a funcao objetivo geral

			houve_melhoria_rotas = 1;     //corfirmando se ouve melhoria em alguma rota
			
			// cout << "rota depois: ";
			// veiculos[v].printaCaminhoTotal(0);
      	}

	}

	return houve_melhoria_rotas; //retorna se ouve melhoria para o VND
}

bool Heuristica::swapEntreRotas(){
	bool houve_melhoria_rotas = 0;

	std::vector<vector<int>> rotas; //vector de vector que vai guardar as rotas de cada veiculo
	for (int i = 0; i < veiculos.size(); i++)
	{
		std::vector<int> *ptr_rota = veiculos[i].getCaminhoTotal();
		std::vector<int> rota;

		int ponto_rota = 0;
		rota.push_back(0);
		while(1){ //colocando a rota num vector com APENAS os vertices visitidos de fato pelo veiculo para analisar os movimentos

			int cliente = (*ptr_rota)[ponto_rota];
			rota.push_back(cliente);

			if (cliente == 0){
				break;
			}else{
				ponto_rota = cliente;
			}
		}          
		rotas.push_back(rota); //adicionando a rota do veiculo i no vector de rotas
	}

	int melhor_j, melhor_k, melhor_l;
	int melhor_delta, delta;
	for (int i = 0; i < rotas.size()-1; i++)
	{	
		melhor_delta = 0;
		for (int j = i+1; j < rotas.size(); j++)
		{
			for (int k = 1; k < rotas[i].size()-1; k++)
			{
				for(int l = 1; l< rotas[j].size()-1; l++){
					if(veiculos[i].getCapacidade() - dados->demandas[rotas[i][k]] + dados->demandas[rotas[j][l]] < dados->capacidade && 
					veiculos[j].getCapacidade() - dados->demandas[rotas[j][l]] + dados->demandas[rotas[i][k]] < dados->capacidade){ //verifica se a troca não excede a capacidade dos veiculos
						delta = -dados->matriz_distancias[rotas[i][k-1]][rotas[i][k]] -dados->matriz_distancias[rotas[i][k]][rotas[i][k+1]] 
							-dados->matriz_distancias[rotas[j][l-1]][rotas[j][l]] -dados->matriz_distancias[rotas[j][l]][rotas[j][l+1]] 
							+dados->matriz_distancias[rotas[i][k-1]][rotas[j][l]] +dados->matriz_distancias[rotas[j][l]][rotas[i][k+1]] 
							+dados->matriz_distancias[rotas[j][l-1]][rotas[i][k]] +dados->matriz_distancias[rotas[i][k]][rotas[j][l+1]]; //calcula o delta da troca

						if(delta < melhor_delta){ // verifica se houve melhora na solucao
							melhor_delta = delta;
							melhor_j = j;
							melhor_k = k;
							melhor_l = l;
						}
					}
				}
			}
		}
		if(melhor_delta<0){
			veiculos[i].setCliente(rotas[melhor_j][melhor_l], rotas[i][melhor_k-1]);
			veiculos[i].setCliente(rotas[i][melhor_k+1], rotas[melhor_j][melhor_l]);
			veiculos[melhor_j].setCliente(rotas[i][melhor_k], rotas[melhor_j][melhor_l-1]);
			veiculos[melhor_j].setCliente(rotas[melhor_j][melhor_l+1], rotas[i][melhor_k]); //substitui os clientes visitados de cada veiculo
			veiculos[i].setObjetivo(veiculos[i].getObjetivo() + melhor_delta);
			veiculos[melhor_j].setObjetivo(veiculos[melhor_j].getObjetivo() + melhor_delta); //seta o novo valor da funcao objetivo de cada veiculo
			veiculos[i].setCapacidade(veiculos[i].getCapacidade() - dados->demandas[rotas[i][melhor_k]] + dados->demandas[rotas[melhor_j][melhor_l]]); 
			veiculos[melhor_l].setCapacidade(veiculos[melhor_l].getCapacidade() - dados->demandas[rotas[melhor_j][melhor_l]]
											 + dados->demandas[rotas[i][melhor_k]]); //seta as novas capacidades de cada veiculo
			funcaoObjetivo = funcaoObjetivo + melhor_delta; // atualiza a funcao objetivo geral
			houve_melhoria_rotas = 1;
		}
	}
	if(houve_melhoria_rotas)
		cout << "Custo total apos o swap: " << this->funcaoObjetivo << endl;
	
	return houve_melhoria_rotas;
}


Heuristica::~Heuristica(){
	
}
