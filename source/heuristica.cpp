#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>

using namespace std;
#define QTD_VIZINHANCAS 3

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregasRealizadas = 0;
	this->funcaoObjetivo = 0;
	this->clientesAtendidos = 0;

	for(int i = 1; i < (dados->q_clientes) + 1; i++){
		clientesDisponiveis.push_back(i);
	}
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
		
	while(!clientesDisponiveis.empty()){	
		if(inseriu == 1)
			break;

		srand((unsigned)time(NULL));
		int n = rand() % clientesDisponiveis.size(); // Gera um indice aleatorio
		int cliente_indice = clientesDisponiveis[n]; // Retira um cliente aleatorio
				
		/* Verifica se o veiculo tem capacidade para suprir a demanda	*/

		int demanda_cliente = dados->demandas[cliente_indice]; 
		int capacidade = veiculo.getCapacidade();
		if(demanda_cliente <= capacidade){
					
			int novaCapacidade = capacidade - demanda_cliente;
					
			funcaoObjetivo = veiculo.getObjetivo() + dados->matriz_distancias[cliente_anterior][cliente_indice];
			veiculo.setCliente(cliente_indice, cliente_anterior); // Seta o cliente como sendo o proximo na rota
			veiculo.setObjetivo(funcaoObjetivo);     // Seta a nova funcao objetivo
			veiculo.setCapacidade(novaCapacidade);  // Seta nova capacidade
			entregasRealizadas++; // Armaneza quantas entregas ja foi realizada
			/*Excluir o cliente gerado */
			int ultimo_cliente = clientesDisponiveis[clientesDisponiveis.size() - 1]; //Armazena o ultimo cliente
			clientesDisponiveis[clientesDisponiveis.size() - 1] = cliente_indice; // O cliente escolhido aleatoriamente vai para a posicao final
			clientesDisponiveis[n] = ultimo_cliente; // Atualiza a posicao atual com o ultimo cliente
			clientesDisponiveis.pop_back(); // Retira o cliente, pois ja foi alocado
			/* Fim da eliminacao do cliente */
			clientesAtendidos++;
			inseriu++;
			cliente_anterior = cliente_indice;
			veiculo.incrementa_clientes();
					
		}
			
		
	}
	
	
	
	cout << "Capacidade do veiculo após solucao inicial: " << veiculo.getCapacidade() << endl;
	
	
	funcaoObjetivo = veiculo.getObjetivo() + dados->matriz_distancias[cliente_anterior][0];
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
				
		/* Verificando se ainda existe cliente para suprir	*/
		if(clientesDisponiveis.size() == 0)
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
		veiculos[i].setCustoVeiculo(); //Seta o custo do veiculo na função objetivo dele
 
	  
		veiculos[i].printaCaminhoTotal(0);
		
		/* O loop começa do ultimo cliente no vetor
		 * Se houver capacidade para aquele cliente
		 * calculamos o menor custo da insercao mais barata
		 * se for realizado a quantia minima de entregas
		 * temos que começar a verificar se o custo da rota real
		 * é maior ou igual ao custo da terceirizacao, porque se for
		 * é melhor terceirizar	*/

		for(int v = clientesDisponiveis.size() - 1; v >= 0; v--){
			
			/* Se o veiculo não tiver capacidade, paramos o codigo	*/
			if( veiculos[i].getCapacidade() <= 0)
				break;

			int cliente = clientesDisponiveis[v];
			
			for(int i = 0; i < clientesDisponiveis.size(); i++){

				cout << clientesDisponiveis[i] << " ";
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
				int ultimo_cliente = clientesDisponiveis[clientesDisponiveis.size()  - 1];
				clientesDisponiveis[clientesDisponiveis.size() - 1] = clientesDisponiveis[v];
				clientesDisponiveis[v] = ultimo_cliente;	
				clientesDisponiveis.pop_back();
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
				int ultimo_cliente = clientesDisponiveis[clientesDisponiveis.size() - 1];
				clientesDisponiveis[clientesDisponiveis.size() - 1] = clientesDisponiveis[v];
				clientesDisponiveis[v] = ultimo_cliente;
				clientesDisponiveis.pop_back();
				/*-------------------------------------------*/
				entregasRealizadas++;
				cout << "Novo caminho: ";
				 veiculos[i].printaCaminhoTotal(0);
				cout << endl;
				clientesAtendidos++;
				veiculos[i].incrementa_clientes();
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
			//	getchar();
			}
		}
		 veiculos[i].printaCaminhoTotal(0);
		cout << endl;
		this->funcaoObjetivo +=  veiculos[i].getObjetivo();

	//	getchar();

	}
	
	cout << "Sobraram os clientes: " << clientesDisponiveis.size() << endl;
	
	/* Caso sobre algum cliente, devemos terceirizar pois todos os veiculos estão preenchidos	*/
	for(int  i = 0; i < clientesDisponiveis.size(); i++){
		int indice_cliente = clientesDisponiveis[i];
		clientesTerceirizados.push_back(indice_cliente);
		this->funcaoObjetivo += dados->custo_terceirizacao[indice_cliente];
	}


	for(int  i = 0; i < veiculos.size(); i++){
		
	  	cout << "Objetivo do veiculo: " << veiculos[i].getObjetivo() << endl;
	  	cout << "Caminho do veiculo( " << i << "):";
	  	veiculos[i].printaCaminhoTotal(0);

	  	cout << endl;
		cout << "Quantia de clientes atendidos pelo veiculo: " << veiculos[i].get_quantia_clientes() << endl;
    }
	
	  cout << "Clientes terceirizados: ";
	  for(int i = 0; i < clientesTerceirizados.size(); i++){

	  	cout << clientesTerceirizados[i] << " ";
	  }
	cout << endl;

	cout << "Custo total apos o guloso: " << this->funcaoObjetivo << endl;
	
}

void Heuristica::resolve(){

	auto inicio = std::chrono::high_resolution_clock::now();
	insercaoMaisBarata();	
	VND();
	auto resultado = std::chrono::high_resolution_clock::now() - inicio;
	long long millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(resultado).count();
	cout << "Solucao: " << funcaoObjetivo << endl;
	cout << "Tempo(s): " << millisecond/1000.0 << endl;
}

void Heuristica::VND(){

	int contador = 0; 
	bool melhorou_solucao = 0;
	while(contador < QTD_VIZINHANCAS){

		switch(contador){
			case 0:
				// melhorou_solucao = reinsertion();
				break;
			case 1: 
				melhorou_solucao = swapEntreRotas();
				break;
			case 2:
				// melhorou_solucao = terceirizacao();
				break;
			case 3:
				// melhorou_solucao = crossover();
				break;
		}

		if(melhorou_solucao){            //se a vizinhanca melhorar a solucao
			contador = 0;
			melhorou_solucao = 0;

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
	bool houve_melhoria_rotas = false;

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

	int melhor_i, melhor_j, melhor_k, melhor_l;
	int melhor_delta, delta;
	for (int i = 0; i < rotas.size()-1; i++)
	{	
		melhor_delta = 0;
		for (int j = i+1; j < rotas.size(); j++)
		{
			for (int k = 1; k < rotas[i].size()-1; k++)
			{
				for(int l = 1; l< rotas[j].size()-1; l++){
					if(veiculos[i].getCapacidade() + dados->demandas[rotas[i][k]] - dados->demandas[rotas[j][l]] >= 0 && 
					veiculos[j].getCapacidade() + dados->demandas[rotas[j][l]] - dados->demandas[rotas[i][k]] >= 0){ //verifica se a troca não excede a capacidade dos veiculos
						delta = -dados->matriz_distancias[rotas[i][k-1]][rotas[i][k]] -dados->matriz_distancias[rotas[i][k]][rotas[i][k+1]] 
							-dados->matriz_distancias[rotas[j][l-1]][rotas[j][l]] -dados->matriz_distancias[rotas[j][l]][rotas[j][l+1]] 
							+dados->matriz_distancias[rotas[i][k-1]][rotas[j][l]] +dados->matriz_distancias[rotas[j][l]][rotas[i][k+1]] 
							+dados->matriz_distancias[rotas[j][l-1]][rotas[i][k]] +dados->matriz_distancias[rotas[i][k]][rotas[j][l+1]]; //calcula o delta da troca

						if(delta < melhor_delta){ // verifica se houve melhora na solucao
							melhor_delta = delta;
							melhor_j = j;
							melhor_k = k;
							melhor_l = l;
							melhor_i = i;
						}
					}
				}
			}
		}
		if(melhor_delta<0){

			int delta_i, delta_j;
			delta_i = -dados->matriz_distancias[rotas[melhor_i][melhor_k-1]][rotas[melhor_i][melhor_k]] -dados->matriz_distancias[rotas[melhor_i][melhor_k]][rotas[melhor_i][melhor_k+1]] 
							+dados->matriz_distancias[rotas[melhor_i][melhor_k-1]][rotas[melhor_j][melhor_l]] +dados->matriz_distancias[rotas[melhor_j][melhor_l]][rotas[melhor_i][melhor_k+1]]; //calcula o delta da troca

			delta_j = -dados->matriz_distancias[rotas[melhor_j][melhor_l-1]][rotas[melhor_j][melhor_l]] -dados->matriz_distancias[rotas[melhor_j][melhor_l]][rotas[melhor_j][melhor_l+1]] 
							+dados->matriz_distancias[rotas[melhor_j][melhor_l-1]][rotas[melhor_i][melhor_k]] +dados->matriz_distancias[rotas[melhor_i][melhor_k]][rotas[melhor_j][melhor_l+1]]; //calcula o delta da troca

			int aux;
			aux = rotas[melhor_i][melhor_k];
			rotas[melhor_i][melhor_k] = rotas[melhor_j][melhor_l];
			rotas[melhor_j][melhor_l] = aux; //faz a troca dos clientes entre as rotas

			veiculos[melhor_i].setCliente(rotas[melhor_i][melhor_k], rotas[melhor_i][melhor_k-1]);
			veiculos[melhor_i].setCliente(rotas[melhor_i][melhor_k+1], rotas[melhor_i][melhor_k]);
			veiculos[melhor_j].setCliente(rotas[melhor_j][melhor_l], rotas[melhor_j][melhor_l-1]);
			veiculos[melhor_j].setCliente(rotas[melhor_j][melhor_l+1], rotas[melhor_j][melhor_l]); //substitui os clientes visitados de cada veiculo
			veiculos[melhor_i].setObjetivo(veiculos[melhor_i].getObjetivo() + delta_i);
			veiculos[melhor_j].setObjetivo(veiculos[melhor_j].getObjetivo() + delta_j); //seta o novo valor da funcao objetivo de cada veiculo
			veiculos[melhor_i].setCapacidade(veiculos[melhor_i].getCapacidade() + dados->demandas[rotas[melhor_i][melhor_k]] - dados->demandas[rotas[melhor_j][melhor_l]]); 
			veiculos[melhor_l].setCapacidade(veiculos[melhor_l].getCapacidade() + dados->demandas[rotas[melhor_j][melhor_l]]
												- dados->demandas[rotas[melhor_i][melhor_k]]); //seta as novas capacidades de cada veiculo

			funcaoObjetivo = funcaoObjetivo + melhor_delta; // atualiza a funcao objetivo geral


			houve_melhoria_rotas = true;
		}
	}

	if(houve_melhoria_rotas)
		cout << "Custo total apos o swap entre rotas: " << this->funcaoObjetivo << endl;

	return houve_melhoria_rotas;
}

bool Heuristica::terceirizacao(){

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
		
		double delta, 
		melhor_delta = 0;         //vai guardar a diferenca de valor em relacao a solucao original        
		int melhor_i;       	  //vao guardar os indices do melhor cliente a ser terceirizado em cada rota


		for(int i = 1; i < rota.size()-2; i++){

			delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[i]][rota[i+1]]
					+dados->matriz_distancias[rota[i-1]][rota[i+1]] + dados->custo_terceirizacao[rota[i]];


			if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao

				
				melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
				melhor_i = i;          //guarda o indice i da melhor troca
			}
		}

		if (melhor_delta < 0){

			veiculos[v].setCliente(rota[melhor_i+1], rota[melhor_i-1]); 
			veiculos[v].setCliente(-1, rota[melhor_i]);            	 	//o cliente que foi terceirizado, volta a apontar para -1
			clientesTerceirizados.push_back(rota[melhor_i]);            //adicionando o cliente na lsita de terceirizados
			
			veiculos[v].setCapacidade(veiculos[v].getCapacidade() + dados->demandas[rota[melhor_i]]);
			veiculos[v].setObjetivo( veiculos[v].getObjetivo() + melhor_delta - dados->custo_terceirizacao[rota[melhor_i]]);  //atualizando a funcao objetivo do veiculo
			funcaoObjetivo = funcaoObjetivo + melhor_delta;                       //atualizando a funcao objetivo geral

			houve_melhoria_rotas = 1;
      	}
	}

	return houve_melhoria_rotas;
}


bool Heuristica::desterceirizacao(){

	bool houve_melhoria_rotas = 0;

	if(clientesTerceirizados.size() == 0){       //se nao tiver nenhum cliente a ser desterceirizado
		return houve_melhoria_rotas;                
	}

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
		
		double delta, 
		melhor_delta = 0;         //vai guardar a diferenca de valor em relacao a solucao original        
		int melhor_i, melhor_j;    //vao guardar os indices do melhor cliente a ser desterceirizado e a posicao que deve ser inserido na rota

		for(int i = 0; i < clientesTerceirizados.size(); i++){

			if(veiculos[v].getCapacidade() >= dados->demandas[clientesTerceirizados[i]]){  

				for(int j = 1; j < rota.size(); j++){    //vendo a melhor posicao pra o cliente ser inserido na rota

					delta = -dados->custo_terceirizacao[clientesTerceirizados[i]] -dados->matriz_distancias[rota[j-1]][rota[j]]
							+dados->matriz_distancias[rota[j-1]][clientesTerceirizados[i]] 
							+dados->matriz_distancias[clientesTerceirizados[i]][rota[j]];


					if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao
						melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
						melhor_i = i;          
						melhor_j = j;

					}
				}
			}
		}

		if (melhor_delta < 0){

			veiculos[v].setCliente(clientesTerceirizados[melhor_i], rota[melhor_j-1]);
			veiculos[v].setCliente(rota[melhor_j], clientesTerceirizados[melhor_i]);
			veiculos[v].setCapacidade(veiculos[v].getCapacidade() - dados->demandas[clientesTerceirizados[melhor_i]]);  //atualizando a capacidade

			swap(clientesTerceirizados[melhor_i], clientesTerceirizados[clientesTerceirizados.size()-1]);  //dando swap para retirar cliente em O(1)
			clientesTerceirizados.pop_back();        //retirando o cliente
			
			veiculos[v].setObjetivo( veiculos[v].getObjetivo() + melhor_delta + dados->custo_terceirizacao[clientesTerceirizados[melhor_i]]);  //atualizando a funcao objetivo do veiculo
			funcaoObjetivo = funcaoObjetivo + melhor_delta;                       //atualizando a funcao objetivo geral
			
			houve_melhoria_rotas = 1;
      	}
	}

	return houve_melhoria_rotas;
}
	

bool Heuristica::crossover(){
	bool houve_melhoria_rotas = 0;

	std::vector<vector<int>> rotas; //vector de vector que vai guardar as rotas de cada veiculo
	vector<int> demanda_parcial;
	for (int i = 0; i < veiculos.size(); i++)
	{
		vector<int> *ptr_rota = veiculos[i].getCaminhoTotal();
		vector<int> rota;

		int ponto_atual = 0;
		int demanda_metade = 0;
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
		for(int j = rotas[i].size()/2; j < rotas[i].size(); j++){
			demanda_metade += dados->demandas[rotas[i][j]];
		}
		demanda_parcial.push_back(demanda_metade);          
	}
		

	int melhor_i, melhor_j;
	int delta, melhor_delta;
	int indice_meio_i, indice_meio_j;
	for (int i = 0; i < rotas.size()-1; i++)
	{
		melhor_delta = 0;
		delta = 0;
		indice_meio_i = rotas[i].size()/2;
		for (int j = i+1; j < rotas.size(); j++)
		{
			indice_meio_j = rotas[j].size() / 2;
			if(veiculos[i].getCapacidade() + demanda_parcial[i] - demanda_parcial[j] >= 0 &&
			   veiculos[j].getCapacidade() + demanda_parcial[j] - demanda_parcial[i] >= 0){//verifica se a troca não excede a capacidade dos veiculos
				delta = -dados->matriz_distancias [rotas[i][indice_meio_i-1]] [rotas[i][indice_meio_i]]
						-dados->matriz_distancias [rotas[j][indice_meio_j-1]] [rotas[j][indice_meio_j]]
						+dados->matriz_distancias [rotas[i][indice_meio_i-1]] [rotas[j][indice_meio_j]]
						+dados->matriz_distancias [rotas[j][indice_meio_j-1]] [rotas[i][indice_meio_i]];
				
				
				if(delta < melhor_delta){ // verifica se houve melhora na solucao
					melhor_delta = delta;
					melhor_i = i;
					melhor_j = j;
				}
			}
		}

		if(melhor_delta < 0){
			int tamanho_i = rotas[melhor_i].size();
			int tamanho_j = rotas[melhor_j].size();
			rotas.push_back(rotas[melhor_i]);
			
			for (int k = tamanho_i; k > tamanho_i/2; k--)
			{
				rotas[melhor_i].pop_back();
			}

			for (int l = tamanho_j/2; l < tamanho_j; l++)
			{
				rotas[melhor_i].push_back(rotas[melhor_j][l]);
			}

			for (int m = tamanho_j; m > tamanho_j/2; m--)
			{
				rotas[melhor_j].pop_back();
			}

			for (int n = tamanho_i/2; n < tamanho_i; n++)
			{
				rotas[melhor_j].push_back(rotas[rotas.size()-1][n]);
			}		

			rotas.pop_back();

			for (int o = 1; o < rotas[melhor_i].size(); o++)
			{
				veiculos[melhor_i].setCliente(rotas[melhor_i][o], rotas[melhor_i][o-1]);
			}
			
			for (int p = 1; p < rotas[melhor_j].size(); p++)
			{
				veiculos[melhor_j].setCliente(rotas[melhor_j][p], rotas[melhor_j][p-1]);
			}

			int delta_i, delta_j;
			delta_i = -dados->matriz_distancias [rotas[melhor_i][indice_meio_i-1]] [rotas[melhor_i][indice_meio_i]]
						+dados->matriz_distancias [rotas[melhor_i][indice_meio_i-1]] [rotas[melhor_j][indice_meio_j]];
			
			delta_j = -dados->matriz_distancias [rotas[melhor_j][indice_meio_j-1]] [rotas[melhor_j][indice_meio_j]]
						+dados->matriz_distancias [rotas[melhor_j][indice_meio_j-1]] [rotas[melhor_i][indice_meio_i]];

			veiculos[melhor_i].setObjetivo(veiculos[melhor_i].getObjetivo() + delta_i);
			veiculos[melhor_j].setObjetivo(veiculos[melhor_j].getObjetivo() + delta_j);

			veiculos[melhor_i].setCapacidade(veiculos[melhor_i].getCapacidade() + demanda_parcial[melhor_i] - demanda_parcial[melhor_j]);
			veiculos[melhor_j].setCapacidade(veiculos[melhor_j].getCapacidade() + demanda_parcial[melhor_j] - demanda_parcial[melhor_i]);

			funcaoObjetivo = funcaoObjetivo + melhor_delta;
			houve_melhoria_rotas = 1;
		}
	}
	if(houve_melhoria_rotas)
		cout << "Custo total apos o crossover: " << this->funcaoObjetivo << endl;
	
	return houve_melhoria_rotas;
}

Heuristica::~Heuristica(){
	
}
