#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>
#include <climits>

using namespace std;

#define QTD_VIZINHANCAS 5
#define QTD_PERTURBACAO 3
#define ILS_EXECUCOES 50

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregas_realizadas = 0;
	this->funcao_objetivo = 0;
	this->clientesAtendidos = 0;
	this->melhor_solucao.funcao_objetivo = INT_MAX; // Inicializa como infinito

	for(int i = 1; i < (dados->q_clientes) + 1; i++){
		clientes_disponiveis.push_back(i);
	}
}



Veiculo Heuristica::solucaoInicial(int indice_veiculo){

	Veiculo veiculo;

	veiculo.edita_parametros(indice_veiculo, dados->capacidade, dados->q_clientes, dados->custo_veiculo);


	veiculo.set_cliente(0, 0); // Começa a rota 0->0
	int inseriu = 0;
	int funcao_objetivo = 0;
	int cliente_anterior = 0;

	/* Esse loop vai pegar um cliente inicial aleatoriamente
	 * Caso eu tenha realizado as entregas minimas
	 * Verificamos se vale a pena terceirizar ou não
	 * Se o custo de ida mais o custo da volta a garagem (melhor cenario possivel)
	 * for maior ou igual a terceirizacao, é melhor terceirizar	
	 * Sempre tentamos criar uma rota, se a gente insere uma vez quebramos o loop*/
		
	while(!clientes_disponiveis.empty()){	
		if(inseriu == 1)
			break;

		srand((unsigned)time(NULL));
		int n = rand() % clientes_disponiveis.size(); // Gera um indice aleatorio
		int cliente_indice = clientes_disponiveis[n]; // Retira um cliente aleatorio
				
		/* Verifica se o veiculo tem capacidade para suprir a demanda	*/
		
		
		if(entregas_realizadas >= dados->minimo_entregas){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
			int custoIda = 2 * dados->matriz_distancias[cliente_indice][0];

			if(custo_terceirizacao_cliente <= custoIda){
				clientes_terceirizados.push_back(cliente_indice);
				this->funcao_objetivo += custo_terceirizacao_cliente;
				swap(clientes_disponiveis[n], clientes_disponiveis[clientes_disponiveis.size() - 1]);
				clientes_disponiveis.pop_back();
				continue;

			}
		}
		
		int demanda_cliente = dados->demandas[cliente_indice]; 
		int capacidade = veiculo.get_capacidade_disp();
		if(demanda_cliente <= capacidade){
					
			int novaCapacidade = capacidade - demanda_cliente;
					
			funcao_objetivo = veiculo.get_objetivo() + dados->matriz_distancias[cliente_anterior][cliente_indice];
			veiculo.set_cliente(cliente_indice, cliente_anterior); // Seta o cliente como sendo o proximo na rota
			veiculo.set_objetivo(funcao_objetivo);     // Seta a nova funcao objetivo
			veiculo.set_capacidade_disp(novaCapacidade);  // Seta nova capacidade
			entregas_realizadas++; // Armaneza quantas entregas ja foi realizada
			/*Excluir o cliente gerado */
			swap(clientes_disponiveis[n], clientes_disponiveis[clientes_disponiveis.size() - 1]);
			clientes_disponiveis.pop_back();
			/* Fim da eliminacao do cliente */
			inseriu++;
			cliente_anterior = cliente_indice;
			veiculo.incrementa_clientes();
					
		}
			
		
	}
	
	
	
	cout << "Capacidade do veiculo após solucao inicial: " << veiculo.get_capacidade_disp() << endl;
	
	
	funcao_objetivo = veiculo.get_objetivo() + dados->matriz_distancias[cliente_anterior][0];
	veiculo.set_cliente(0, cliente_anterior);
	veiculo.set_objetivo(funcao_objetivo);
	
	return veiculo;
	
}



/* A insercao do mais proximo
 * será realizada com um veiculo por vez
 * pegamos inicialmente uma solucao inicial formada por 2 vertices + deposito
 * e calculamos a insercao mais barata*/

void Heuristica::insercaoMaisBarata(){
		
	for(int i = 0; i < dados->q_veiculos; i++){
				
		/* Verificando se ainda existe cliente para suprir	*/
		if(clientes_disponiveis.size() == 0)
			break;

		cout << "Solucao inicial para o veiculo: " << i << endl;
		cout << "-----------------Gerando Solucao Inicial----------------------------------" << endl;
		Veiculo veiculo = solucaoInicial(i); //Constroi a solução inicial
		cout << endl << "------------------------------------------------------------------" << endl;
		
		/* Quer dizer que nao foi construido uma solucao inicial para o veiculo
		 * seja porque a terceirizacao foi melhor */
		 if(veiculo.get_prox_cliente(0) == 0){
		 	continue;
		 }
		
		veiculos.push_back(veiculo);  
		veiculos[i].adiciona_custo_veiculo(); //Seta o custo do veiculo na função objetivo dele
 
	  
		veiculos[i].printa_caminho_total(0);
		
		/* O loop começa do ultimo cliente no vetor
		 * Se houver capacidade para aquele cliente
		 * calculamos o menor custo da insercao mais barata
		 * se for realizado a quantia minima de entregas
		 * temos que começar a verificar se o custo da rota real
		 * é maior ou igual ao custo da terceirizacao, porque se for
		 * é melhor terceirizar	*/

		for(int v = clientes_disponiveis.size() - 1; v >= 0; v--){
			
			/* Se o veiculo não tiver capacidade, paramos o codigo	*/
			if( veiculos[i].get_capacidade_disp() <= 0)
				break;

			int cliente = clientes_disponiveis[v];
			
			for(int i = 0; i < clientes_disponiveis.size(); i++){

				cout << clientes_disponiveis[i] << " ";
			}
			
			/* Se a demanda do cliente for maior do que a capacidade, então pulamos para o proximo cliente	*/
			int demanda_cliente = dados->demandas[cliente];
			//cout << "Demanda do cliente " << cliente << " " << demanda_cliente << endl;
			if(demanda_cliente >  veiculos[i].get_capacidade_disp()){
				
				//cout << "Nao eh possivel testar o cliente: " << cliente << " pois sua demanda eh maior do que a capacidade" << endl;
				continue;
			}
			
			
			/* O while é responsável por percorrer a arvore
			 * e armazenar o melhor vértice para inserir o cliente atual
			 * Começamos  o cliente A sendo zero que é a garagem
			 * o caminho está como uma árvore logo vetor[a] = b, então 
			 * o nó a possui o nó b como pai, paramos quando o pai for a garagem que vale zero*/
			vector < int> *caminhoTotal = veiculos[i].get_caminho_total();
			int melhorCliente_a = 0;
			int melhorCliente_b = 0;
			int custo = 0;
			int melhorCusto = INT_MAX;
			bool inseriu = false;

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
					inseriu = true;
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

			/* Se o custo da terceirização for menor igual a esse custo a gente terceiriza	*/
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente];
			if(custo_terceirizacao_cliente < melhorCusto and entregas_realizadas >= dados->minimo_entregas){

				cout << "O cliente tem custo de terceirizacao melhor, pois vale: " << custo_terceirizacao_cliente << endl;
				
				swap(clientes_disponiveis[v], clientes_disponiveis[clientes_disponiveis.size() - 1]);
				clientes_disponiveis.pop_back();
				this->funcao_objetivo += custo_terceirizacao_cliente;
				clientes_terceirizados.push_back(cliente);
				continue;

			}

			if(inseriu){
				cout << "-------------------------------Iniciando Insercao---------------------------------" << endl;
				veiculos[i].set_cliente(cliente, melhorCliente_a); // Seta o pai de melhor_Cliente_a como cliente
				veiculos[i].set_cliente(melhorCliente_b, cliente); // Seta o pai de cliente como melhorCliente)b

				//cout << "O melhor custo foi de: " << melhorCusto << endl;
				//cout << "A insercado do cliente: " << cliente << " entre " << melhorCliente_a << " " << melhorCliente_b << endl;

				int funcao_objetivo =  veiculos[i].get_objetivo() + melhorCusto;
				veiculos[i].set_objetivo(funcao_objetivo);
				//cout << "Valor da nova funcao objetivo: " << funcao_objetivo << endl;
				int novaCapacidade =  veiculos[i].get_capacidade_disp() - demanda_cliente;
				veiculos[i].set_capacidade_disp(novaCapacidade);
				//cout << "Capacidade do veiculo apos insercao: " <<  veiculos[i].get_capacidade_disp() << endl;

				/* Troca a posicao com o ultimo e da o pop_back	*/
				swap(clientes_disponiveis[v], clientes_disponiveis[clientes_disponiveis.size() - 1]);
				clientes_disponiveis.pop_back();

				entregas_realizadas++;
				cout << "Novo caminho: ";
				veiculos[i].printa_caminho_total(0);
				cout << endl;
				veiculos[i].incrementa_clientes();
				cout << "-------------------------------Fim Insercao--------------------------------------" << endl;
			//	getchar();
			}
		}
		veiculos[i].printa_caminho_total(0);
		cout << endl;
		this->funcao_objetivo +=  veiculos[i].get_objetivo();

	//	getchar();

	}
	
	cout << "Foram realizadas a quantia de entregas: " << entregas_realizadas << endl;
	cout << "Sobraram os clientes: " << clientes_disponiveis.size() << endl;
	
	/* Caso sobre algum cliente, devemos terceirizar pois todos os veiculos estão preenchidos	*/
	for(int  i = 0; i < clientes_disponiveis.size(); i++){
		int indice_cliente = clientes_disponiveis[i];
		clientes_terceirizados.push_back(indice_cliente);
		this->funcao_objetivo += dados->custo_terceirizacao[indice_cliente];
	}


	 for(int  i = 0; i < veiculos.size(); i++){
		
	   	cout << "Objetivo do veiculo: " << veiculos[i].get_objetivo() << endl;
	   	cout << "Caminho do veiculo( " << i << "):";
	   	veiculos[i].printa_caminho_total(0);

	   	cout << endl;
	 	cout << "Quantia de clientes atendidos pelo veiculo: " << veiculos[i].get_quantia_clientes() << endl;
	 	cout << "Capacidade atual do veiculo: " << veiculos[i].get_capacidade_disp() << endl;
     }
	
	   cout << "Clientes terceirizados: ";
	for(int i = 0; i < clientes_terceirizados.size(); i++){

	   	cout << clientes_terceirizados[i] << " ";
	   }
	 cout << endl;

	 cout << "Custo total apos o guloso: " << this->funcao_objetivo << endl;
	cout << "saiu do guloso" << endl;
	getchar();
}

void Heuristica::ILS(){

	auto inicio = std::chrono::high_resolution_clock::now();
	insercaoMaisBarata();	
	VND();

	cout << "Funcao objetivo apos VND: " << this->funcao_objetivo << endl;
	
	/* Vamos executar o ILS 10 vezes	*/
	for(int i = 0; i < ILS_EXECUCOES; i++) {
		
		perturbacao();
		cout << "here" << endl;
		VND();

		if(this->funcao_objetivo < this->melhor_solucao.funcao_objetivo){
			this->melhor_solucao.veiculos = this->veiculos;
			this->melhor_solucao.clientes_terceirizados = this->clientes_terceirizados;
			this->melhor_solucao.funcao_objetivo = this->funcao_objetivo;
			cout << "Atualizou a melhor solucao" << endl;
		}
		cout << "Funcao Objetivo apos VND: " << this->melhor_solucao.funcao_objetivo << endl;
		// getchar();
	}


	auto resultado = std::chrono::high_resolution_clock::now() - inicio;
	long long millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(resultado).count();
	cout << "Solucao: " << this->melhor_solucao.funcao_objetivo << endl;
	cout << "Tempo(s): " << millisecond/1000.0 << endl;
}

void Heuristica::VND(){

	int contador = 0; 
	bool melhorou_solucao = 0;
	while(contador < QTD_VIZINHANCAS){

		switch(contador){
			case 0:
				cout << "entrou aqui" << endl;
				melhorou_solucao = reinsertion();
				cout << "saiu do reinsertion" << endl;
				break;
			case 1: 
				melhorou_solucao = swapEntreRotas();
				break;
			case 2:
				cout << "entrou terceirizacao" << endl;
				melhorou_solucao = terceirizacao();
				cout << "saiu terceirizao" << endl;
				break;
			case 3:
				melhorou_solucao = crossover();
				break;
			case 4:
				melhorou_solucao = desterceirizacao();
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
		std::vector<int> *ptr_rota = veiculos[v].get_caminho_total();


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

			 cout << "mellhor i: " << melhor_i << " " << "melhor j: " << melhor_j << endl;
			 cout << "melhor delta: " << melhor_delta << endl;

			if(melhor_i > melhor_j){

				veiculos[v].set_cliente(rota[melhor_j+1], rota[melhor_j-1]);      // o antecessor do melhor j vai apontar pro sucessor de j
				veiculos[v].set_cliente(rota[melhor_j], rota[melhor_i-1]);           //o melhorj vai apontar pro melhor i
				veiculos[v].set_cliente(rota[melhor_i], rota[melhor_j]);         //o melhor j vai apntar pro sucessor de i                                  
				
			}else{

				veiculos[v].set_cliente(rota[melhor_j], rota[melhor_i-1]);      
				veiculos[v].set_cliente(rota[melhor_i], rota[melhor_j]);          
				veiculos[v].set_cliente(rota[melhor_j+1], rota[melhor_j-1]);        
				                           
			}
			
			veiculos[v].set_objetivo( veiculos[v].get_objetivo() + melhor_delta );  //atualizando a funcao objetivo do veiculo
			funcao_objetivo = funcao_objetivo + melhor_delta;                       //atualizando a funcao objetivo geral

			houve_melhoria_rotas = 1;     //corfirmando que houve melhoria em alguma rota
			getchar();
			// cout << "rota depois: ";
			// veiculos[v].printa_caminho_total(0);
      	}

	}

	return houve_melhoria_rotas; //retorna se ouve melhoria para o VND
}

bool Heuristica::swapEntreRotas(){
	
	if(veiculos.size() == 1)
		return 0;

	bool houve_melhoria_rotas = 0;
	std::vector<vector<int>> rotas; //vector de vector que vai guardar as rotas de cada veiculo
	for (int i = 0; i < veiculos.size(); i++)
	{
		std::vector<int> *ptr_rota = veiculos[i].get_caminho_total();
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
					if((veiculos[i].get_capacidade_disp() + dados->demandas[rotas[i][k]] - dados->demandas[rotas[j][l]]) >= 0 && 
					(veiculos[j].get_capacidade_disp() + dados->demandas[rotas[j][l]] - dados->demandas[rotas[i][k]]) >= 0){ //verifica se a troca não excede a capacidade dos veiculos
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

			int capacidade_i, capacidade_j;
			capacidade_i = veiculos[melhor_i].get_capacidade_disp() + dados->demandas[rotas[melhor_i][melhor_k]] - dados->demandas[rotas[melhor_j][melhor_l]];
			capacidade_j = veiculos[melhor_j].get_capacidade_disp() + dados->demandas[rotas[melhor_j][melhor_l]] - dados->demandas[rotas[melhor_i][melhor_k]];
			veiculos[melhor_i].set_capacidade_disp(capacidade_i); 
			veiculos[melhor_j].set_capacidade_disp(capacidade_j); //seta as novas capacidades de cada veiculo

			int delta_i, delta_j;
			delta_i = -dados->matriz_distancias[rotas[melhor_i][melhor_k-1]][rotas[melhor_i][melhor_k]] -dados->matriz_distancias[rotas[melhor_i][melhor_k]][rotas[melhor_i][melhor_k+1]] 
							+dados->matriz_distancias[rotas[melhor_i][melhor_k-1]][rotas[melhor_j][melhor_l]] +dados->matriz_distancias[rotas[melhor_j][melhor_l]][rotas[melhor_i][melhor_k+1]]; //calcula o delta da troca

			delta_j = -dados->matriz_distancias[rotas[melhor_j][melhor_l-1]][rotas[melhor_j][melhor_l]] -dados->matriz_distancias[rotas[melhor_j][melhor_l]][rotas[melhor_j][melhor_l+1]] 
							+dados->matriz_distancias[rotas[melhor_j][melhor_l-1]][rotas[melhor_i][melhor_k]] +dados->matriz_distancias[rotas[melhor_i][melhor_k]][rotas[melhor_j][melhor_l+1]]; //calcula o delta da troca

			veiculos[melhor_i].set_objetivo(veiculos[melhor_i].get_objetivo() + delta_i);
			veiculos[melhor_j].set_objetivo(veiculos[melhor_j].get_objetivo() + delta_j); //seta o novo valor da funcao objetivo de cada veiculo

			int aux;
			aux = rotas[melhor_i][melhor_k];
			rotas[melhor_i][melhor_k] = rotas[melhor_j][melhor_l];
			rotas[melhor_j][melhor_l] = aux; //faz a troca dos clientes entre as rotas

			veiculos[melhor_i].set_cliente(rotas[melhor_i][melhor_k], rotas[melhor_i][melhor_k-1]);
			veiculos[melhor_i].set_cliente(rotas[melhor_i][melhor_k+1], rotas[melhor_i][melhor_k]);
			veiculos[melhor_j].set_cliente(rotas[melhor_j][melhor_l], rotas[melhor_j][melhor_l-1]);
			veiculos[melhor_j].set_cliente(rotas[melhor_j][melhor_l+1], rotas[melhor_j][melhor_l]); //substitui os clientes visitados de cada veiculo

			funcao_objetivo = funcao_objetivo + melhor_delta; // atualiza a funcao objetivo geral

			houve_melhoria_rotas = 1;
		}
	}


	if(houve_melhoria_rotas)
		cout << "Custo total apos o swap entre rotas: " << this->funcao_objetivo << endl;

	return houve_melhoria_rotas;
}

bool Heuristica::terceirizacao(){
	
	if(dados->minimo_entregas <= entregas_realizadas)
		return 0;
	
	bool houve_melhoria_rotas = 0;
	vector<int> indice_v_retirados;       //vai guardar os indices dos veiculos a serem retirados

	for(int v = 0; v < veiculos.size(); v++){

		vector<int> rota;
		vector<int> *ptr_rota = veiculos[v].get_caminho_total();


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

		veiculos[v].printa_caminho_total(0);

		for(int i = 1; i < rota.size()-1; i++){

			delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[i]][rota[i+1]]
					+dados->matriz_distancias[rota[i-1]][rota[i+1]] + dados->custo_terceirizacao[rota[i]];

			if(rota.size() == 3){   //so existe 1 cliente no veiculo entao se terceirizar, o veiculo deixa de existir
				delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[i]][rota[i+1]]
					    -dados->custo_veiculo + dados->custo_terceirizacao[rota[i]];
			}


			if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao

				melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
				melhor_i = i;          //guarda o indice i da melhor troca
			}
		}

		if (melhor_delta < 0){
			
			entregas_realizadas--;

			clientes_terceirizados.push_back(rota[melhor_i]);        //adicionando o cliente na lsita de terceirizados

			if(rota.size() == 3){
				indice_v_retirados.push_back(v);
				funcao_objetivo = funcao_objetivo + melhor_delta;
			}else{

				veiculos[v].set_cliente(rota[melhor_i+1], rota[melhor_i-1]); 
				veiculos[v].set_cliente(-1, rota[melhor_i]);            	 	//o cliente que foi terceirizado, volta a apontar para -1
			
				veiculos[v].set_capacidade_disp(veiculos[v].get_capacidade_disp() + dados->demandas[rota[melhor_i]]);
				veiculos[v].set_objetivo( veiculos[v].get_objetivo() + melhor_delta - dados->custo_terceirizacao[rota[melhor_i]]);  //atualizando a funcao objetivo do veiculo
				funcao_objetivo = funcao_objetivo + melhor_delta;                       //atualizando a funcao objetivo geral
			}

			houve_melhoria_rotas = 1;
      	}
	}

	for(int i = indice_v_retirados.size() - 1; i >= 0; --i){            //retirando os veiculos com indices maiores primeiro no vetor veiculos
		veiculos.erase(veiculos.begin() + indice_v_retirados[i]);       //retirando os veiculos
	}
	//getchar();

	return houve_melhoria_rotas;
}


bool Heuristica::desterceirizacao(){

	bool houve_melhoria_rotas = 0;

	if(clientes_terceirizados.size() == 0){       //se nao tiver nenhum cliente a ser desterceirizado
		return houve_melhoria_rotas;                
	}

	for(int v = 0; v < veiculos.size(); v++){

		std::vector<int> rota;
		std::vector<int> *ptr_rota = veiculos[v].get_caminho_total();

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

		for(int i = 0; i < clientes_terceirizados.size(); i++){

			if(veiculos[v].get_capacidade_disp() >= dados->demandas[clientes_terceirizados[i]]){  

				for(int j = 1; j < rota.size(); j++){    //vendo a melhor posicao pra o cliente ser inserido na rota

					delta = -dados->custo_terceirizacao[clientes_terceirizados[i]] -dados->matriz_distancias[rota[j-1]][rota[j]]
							+dados->matriz_distancias[rota[j-1]][clientes_terceirizados[i]] 
							+dados->matriz_distancias[clientes_terceirizados[i]][rota[j]];


					if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao
						melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
						melhor_i = i;          
						melhor_j = j;

					}
				}
			}
		}

		if (melhor_delta < 0){
			
			entregas_realizadas++;
			veiculos[v].set_cliente(clientes_terceirizados[melhor_i], rota[melhor_j-1]);
			veiculos[v].set_cliente(rota[melhor_j], clientes_terceirizados[melhor_i]);
			veiculos[v].set_capacidade_disp(veiculos[v].get_capacidade_disp() - dados->demandas[clientes_terceirizados[melhor_i]]);  //atualizando a capacidade

			swap(clientes_terceirizados[melhor_i], clientes_terceirizados[clientes_terceirizados.size()-1]);  //dando swap para retirar cliente em O(1)
			clientes_terceirizados.pop_back();        //retirando o cliente
			
			veiculos[v].set_objetivo( veiculos[v].get_objetivo() + melhor_delta + dados->custo_terceirizacao[clientes_terceirizados[melhor_i]]);  //atualizando a funcao objetivo do veiculo
			funcao_objetivo = funcao_objetivo + melhor_delta;                       //atualizando a funcao objetivo geral
			
			houve_melhoria_rotas = 1;
      	}
	}

	return houve_melhoria_rotas;
}
	

bool Heuristica::crossover(){
	bool houve_melhoria_rotas = 0;
	
	if(veiculos.size() == 1)
		return 0;

	std::vector<vector<int>> rotas; //vector de vector que vai guardar as rotas de cada veiculo
	vector<int> demanda_parcial;
	for (int i = 0; i < veiculos.size(); i++)
	{
		vector<int> *ptr_rota = veiculos[i].get_caminho_total();
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
			if(veiculos[i].get_capacidade_disp() + demanda_parcial[i] - demanda_parcial[j] >= 0 &&
			   veiculos[j].get_capacidade_disp() + demanda_parcial[j] - demanda_parcial[i] >= 0){//verifica se a troca não excede a capacidade dos veiculos
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
				veiculos[melhor_i].set_cliente(rotas[melhor_i][o], rotas[melhor_i][o-1]);
			}
			
			for (int p = 1; p < rotas[melhor_j].size(); p++)
			{
				veiculos[melhor_j].set_cliente(rotas[melhor_j][p], rotas[melhor_j][p-1]);
			}

			int delta_i, delta_j;
			delta_i = -dados->matriz_distancias [rotas[melhor_i][indice_meio_i-1]] [rotas[melhor_i][indice_meio_i]]
						+dados->matriz_distancias [rotas[melhor_i][indice_meio_i-1]] [rotas[melhor_j][indice_meio_j]];
			
			delta_j = -dados->matriz_distancias [rotas[melhor_j][indice_meio_j-1]] [rotas[melhor_j][indice_meio_j]]
						+dados->matriz_distancias [rotas[melhor_j][indice_meio_j-1]] [rotas[melhor_i][indice_meio_i]];

			veiculos[melhor_i].set_objetivo(veiculos[melhor_i].get_objetivo() + delta_i);
			veiculos[melhor_j].set_objetivo(veiculos[melhor_j].get_objetivo() + delta_j);

			veiculos[melhor_i].set_capacidade_disp(veiculos[melhor_i].get_capacidade_disp() + demanda_parcial[melhor_i] - demanda_parcial[melhor_j]);
			veiculos[melhor_j].set_capacidade_disp(veiculos[melhor_j].get_capacidade_disp() + demanda_parcial[melhor_j] - demanda_parcial[melhor_i]);

			funcao_objetivo = funcao_objetivo + melhor_delta;
			houve_melhoria_rotas = 1;
		}
	}
	if(houve_melhoria_rotas)
		cout << "Custo total apos o crossover: " << this->funcao_objetivo << endl;
	
	return houve_melhoria_rotas;
}


void Heuristica::perturbacao(){
	
	/* Vamos pegar 2 veiculos aleatorios enquanto fazemos shift entre rotas 1 a 1*/	
	srand((unsigned)time(NULL));
	
	int perturbacoes_realizadas = 0;
	cout << "entrou na perturbacao" << endl;
	while(perturbacoes_realizadas < QTD_PERTURBACAO){
		if(veiculos.size() == 1)
			break;
		int indice_primeiro_veiculo = rand() % veiculos.size(); // Primeiro veiculo aleatorio
		/* Devemos garantir que o segundo indice seja diferente do primeiro	*/
		int indice_segundo_veiculo = indice_primeiro_veiculo;
		while(indice_segundo_veiculo == indice_primeiro_veiculo){
			indice_segundo_veiculo = rand() % veiculos.size();
		}
		
		
		/* Vetores de cada rota referente ao seu veiculo	*/
		vector < int > clientes_i;
		vector < int > clientes_j;

		/* Vale pagar o custo de inserir em vetores o caminho total	*/
		int cliente_anterior_i = 0;
		int cliente_anterior_j = 0;
		int criou_rotas = 0;

		while(criou_rotas < 2){
			if(criou_rotas == 0){

				clientes_i.push_back(cliente_anterior_i);
				int cliente_atual_i = veiculos[indice_primeiro_veiculo].get_prox_cliente(cliente_anterior_i);
					
				if(cliente_atual_i == 0){
					criou_rotas++;
					continue;
				}
				
				cliente_anterior_i = cliente_atual_i;
			}
			else{
				clientes_j.push_back(cliente_anterior_j);
				int cliente_atual_j = veiculos[indice_segundo_veiculo].get_prox_cliente(cliente_anterior_j);
				if(cliente_atual_j == 0){

					criou_rotas++;
					continue;
				}
				cliente_anterior_j = cliente_atual_j;
			}
		}
		
		// cout << "Rota i antes  da perturbacao: ";

		// for(int i = 0 ; i < clientes_i.size(); i++){
		// 	cout << clientes_i[i] << " ";
		// }
		// cout << endl;
		
		// cout << "Rota j antes da perturbacao: ";
		// for(int j = 0; j < clientes_j.size(); j++){
		// 	cout << clientes_j[j] << " ";
		// }
		// cout << endl;

		/* Precisamos agora de duas posicoes aleatorias da rota para fazer o swap
		 * porem precisamos garantir que funcione por conta da capacidade de cada veiculo	*/
		int operacao_realizada = 0;

		while(operacao_realizada < 5){
			
			/* indices que terão seu proximo cliente alterado, caso o veiculo possua capacidade*/
			int i =  rand() % clientes_i.size();
			int j =  rand() % clientes_j.size();
			int prox_cliente_i = veiculos[indice_primeiro_veiculo].get_prox_cliente(clientes_i[i]);
			int prox_cliente_j = veiculos[indice_segundo_veiculo].get_prox_cliente(clientes_j[j]);
			
			//cout << "Sera realiza o swap entre: " << prox_cliente_i << " e " << prox_cliente_j << endl;
			int prox_final_i = veiculos[indice_primeiro_veiculo].get_prox_cliente(prox_cliente_i);
			int prox_final_j = veiculos[indice_segundo_veiculo].get_prox_cliente(prox_cliente_j);
			/* A nova capacidade dos veiculos será determinada por
			 * vamos retirar a capacidade do cliente anterior e adicionar a capacidade do novo cliente
			 * caso esse calculo seja maior ou igual a zero é porque podemos adicionar o cliente*/
			int nova_capacidade_i = veiculos[indice_primeiro_veiculo].get_capacidade_disp() + dados->demandas[prox_cliente_i]
				- dados->demandas[prox_cliente_j];
			int nova_capacidade_j = veiculos[indice_segundo_veiculo].get_capacidade_disp()
				+ dados->demandas[prox_cliente_j] - dados->demandas[prox_cliente_i];
			
			cout << "oi" << endl;
			/* Vamos adicionar o prox_cliente_j em prox_cliente_i	*/

			if(nova_capacidade_i >= 0 and nova_capacidade_j >= 0){
				veiculos[indice_primeiro_veiculo].set_cliente(prox_cliente_j, clientes_i[i]);
				veiculos[indice_primeiro_veiculo].set_cliente(prox_final_i, prox_cliente_j);
				int funcao_objetivo = veiculos[indice_primeiro_veiculo].get_objetivo() - dados->matriz_distancias[clientes_i[i]][prox_cliente_i]
					- dados->matriz_distancias[prox_cliente_i][prox_final_i] + dados->matriz_distancias[clientes_i[i]][prox_cliente_j]
					+ dados->matriz_distancias[prox_cliente_j][prox_final_i];
			

				this->funcao_objetivo = this->funcao_objetivo - veiculos[indice_primeiro_veiculo].get_objetivo()
					+ funcao_objetivo;
				veiculos[indice_primeiro_veiculo].set_objetivo(funcao_objetivo);


				// cout << "Capacidade nova do veiculo i: " << nova_capacidade_i << endl;
				// cout << "Capacidade nova do veiculo j: " << nova_capacidade_j << endl;

				veiculos[indice_segundo_veiculo].set_cliente(prox_cliente_i, clientes_j[j]);
				veiculos[indice_segundo_veiculo].set_cliente(prox_final_j, prox_cliente_i);
				funcao_objetivo = veiculos[indice_segundo_veiculo].get_objetivo() - dados->matriz_distancias[clientes_j[j]][prox_cliente_j]
					- dados->matriz_distancias[prox_cliente_j][prox_final_j] + dados->matriz_distancias[clientes_j[j]][prox_cliente_i]
					+ dados->matriz_distancias[prox_cliente_i][prox_final_j];

				this->funcao_objetivo = this->funcao_objetivo - veiculos[indice_segundo_veiculo].get_objetivo()
						+ funcao_objetivo;
				veiculos[indice_segundo_veiculo].set_objetivo(funcao_objetivo);

				veiculos[indice_primeiro_veiculo].set_capacidade_disp(nova_capacidade_i);
				veiculos[indice_segundo_veiculo].set_capacidade_disp(nova_capacidade_j);
				cout << "Nova objetivo veiculo i: " << veiculos[indice_primeiro_veiculo].get_objetivo() << endl;
				cout << "Nova objetivo veiculo j: " << veiculos[indice_segundo_veiculo].get_objetivo() << endl;
				break;
			}

			operacao_realizada++;
		}
		// cout << "Rota i apos perturbacao: ";
		// veiculos[indice_primeiro_veiculo].printa_caminho_total(0);
		// cout << endl;
		// cout << "Rota j apos perturbacao: ";
		// veiculos[indice_segundo_veiculo].printa_caminho_total(0);
		perturbacoes_realizadas++;
		//getchar();
	}
	
	//cout << "Funcao Objetivo apos perturbaca: " << this->funcao_objetivo << endl;
	/*
	for(int i = 0; i < veiculos.size(); i++){

		cout << "Funcao Objetivo do veiculo i: " << veiculos[i].get_objetivo() << endl;
		cout << "Caminho do veiculo i: ";
		veiculos[i].printa_caminho_total(0);
	}
	
	cout << endl;
	cout << "Clientes terceirizados: ";
	for(int j = 0; j < clientes_terceirizados.size(); j++){

		cout << clientes_terceirizados[j] << " ";
	}
	cout << endl;
	getchar();
	*/

}

Heuristica::~Heuristica(){
	
}
