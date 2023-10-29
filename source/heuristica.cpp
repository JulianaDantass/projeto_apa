#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>
#include <climits>
#include <fstream>
#include <iomanip>

using namespace std;

#define QTD_VIZINHANCAS 5
#define QTD_PERTURBACAO 3
#define ILS_EXECUCOES 50

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	this->entregas_realizadas = 0;
	this->funcao_objetivo = 0;
	this->melhor_solucao.funcao_objetivo = INT_MAX; // Inicializa como infinito

}


Veiculo Heuristica::solucaoInicial(int indice_veiculo){

	Veiculo veiculo;

	veiculo.edita_parametros(indice_veiculo, dados->capacidade, dados->q_clientes, dados->custo_veiculo);


	veiculo.set_cliente(0, 0); // Começa a rota 0->0
	
	
	int inseriu = 0; // Variável de controle para quebrar o loop
	int funcao_objetivo = 0; // Variável para armazenar temporariamente o valor da função objetivo
	int cliente_anterior = 0; // Armazena o cliente anterior da rota

	
	/* O loop será realizado enquanto eu tiver clientes 
		Ou sejá ter inserido um cliente em uma rota
		Vamos pegar o cliente com a menor demanda e inserir na rota
		como solução inicial, caso o algoritmo já tenha realizado
		todas as entregas mínimas a intenção será apenas terceirizar*/
		
	while(!clientes_ordenados.empty()){	
		if(inseriu == 1)
			break;

		Cliente cliente = clientes_ordenados.back(); // Armazena os dados do cliente com a menor demanda 
		int cliente_indice = cliente.indice;		
		
		
		/* Verifica se já foi realizado as entregas minimas, caso sim teremos que terceirizar */
		if(entregas_realizadas >= dados->minimo_entregas){
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente_indice];
			clientes_terceirizados.push_back(cliente_indice);
			this->funcao_objetivo += custo_terceirizacao_cliente;
			clientes_ordenados.pop_back();
			continue;
		}

		/* Verifica se o veiculo tem capacidade para suprir a demanda	*/
		int demanda_cliente = dados->demandas[cliente_indice]; 
		int capacidade = veiculo.get_capacidade_disp();
		if(demanda_cliente <= capacidade){

			int novaCapacidade = capacidade - demanda_cliente; // Cálculo da nova capacidade disponível do veículo
					
			funcao_objetivo = veiculo.get_objetivo() + dados->matriz_distancias[cliente_anterior][cliente_indice];
			veiculo.set_cliente(cliente_indice, cliente_anterior); // Seta o cliente como sendo o proximo na rota
			veiculo.set_objetivo(funcao_objetivo);     // Seta a nova funcao objetivo
			veiculo.set_capacidade_disp(novaCapacidade);  // Seta nova capacidade
			entregas_realizadas++; // Armaneza quantas entregas ja foi realizada
			clientes_ordenados.pop_back(); // Retira o cliente
			inseriu++;
			cliente_anterior = cliente_indice;
			veiculo.incrementa_clientes();		
		}
			
		
	}
	

	funcao_objetivo = veiculo.get_objetivo() + dados->matriz_distancias[cliente_anterior][0]; // Calculo da nova função funçao objetivo
	veiculo.set_cliente(0, cliente_anterior); // Seta o retorno do cliente para a garagem criando a solução inicial
	veiculo.set_objetivo(funcao_objetivo);
	
	return veiculo;
	
}


/* A inserção mais barata será realizada
	a partir de um vetor ordenado em ordem decrescente
	em relação a demanda. A intenção é utilizar a menor quantia de veiculos
	que supra a maior quantidade de clientes possiveis com um bom custo de rota.
	Após fazer a quantia de entregas mínimas os clientes serão terceirizados */

void Heuristica::insercaoMaisBarata(){
	
	int n = dados->demandas.size();
	/* Cria um vetor de clientes que possuem dado de demanda e indice */
	for(int i = 1; i < n; i++){
		Cliente cliente;
		cliente.indice = i;
		cliente.demanda = this->dados->demandas[i];
		clientes_ordenados.push_back(cliente);
	}

	/* Ordena em ordem decrescente*/
	sort(clientes_ordenados.begin(), clientes_ordenados.end(), [](const Cliente& a, const Cliente& b){
			return 	a.demanda >= b.demanda;
			});


	for(int i = 0; i < dados->q_veiculos; i++){
				
		/* Verificamos se ainda existe clientes para serem ordenados
		   caso não houver paramos o loop ou as entregas minimas serem realizadas*/
		if(clientes_ordenados.size() == 0 or entregas_realizadas == dados->minimo_entregas)
			break;


		Veiculo veiculo = solucaoInicial(i); //Constroi a solução inicial

		/* Quer dizer que nao foi construido uma solucao inicial para o veiculo
		 * seja porque a terceirizacao foi melhor */
		 if(veiculo.get_prox_cliente(0) == 0){
		 	continue;
		 }
		
		veiculos.push_back(veiculo);  
		veiculos[i].adiciona_custo_veiculo(); // Adiciona o custo do veiculo na função objetivo dele
 
	  
		/* O loop começa a partir do cliente com a menor demanda
			Se o veiculos não tiver capacidade devemos passar para o próximo
			além disso, se forem realizadas as entregas minimas, também devemos parar o codigo
				*/

		for(int v = clientes_ordenados.size() - 1; v >= 0; v--){
			
			/* Se o veiculo não tiver capacidade ou entregas minimas foram realizados paramos a distribuição */
			if( veiculos[i].get_capacidade_disp() <= 0 or entregas_realizadas >= dados->minimo_entregas)
				break;


			/* Armazena informações do cliente */
			Cliente cliente_data = clientes_ordenados[v];
			int cliente = cliente_data.indice;
	

			/* Se a demanda do cliente for maior do que a capacidade disponivel do veiculo
				O veiculo consegue suprir mais nenhum cliente, pois essa é a menor	*/
			int demanda_cliente = dados->demandas[cliente];
			if(demanda_cliente >  veiculos[i].get_capacidade_disp()){
				break;
			}
			
			
			/* O while é responsável por percorrer a arvore
			 * e armazenar o melhor vértice para inserir o cliente atual
			 * Começamos  o cliente A sendo zero que é a garagem
			 * o caminho está como uma árvore logo vetor[a] = b, então 
			 * o nó a possui o nó b como pai, paramos quando o pai for a garagem que vale zero*/
			vector < int> *caminho_total= veiculos[i].get_caminho_total();
			int melhor_cliente_a = 0;
			int melhor_cliente_b = 0;
			int custo = 0;
			int melhor_custo = INT_MAX; /* melhor custo da inserção */
			bool inseriu = false; /* Variavel de controle que indica se foi inserido */

			int cliente_a = 0;
			while(1){
				int cliente_b = (*caminho_total)[cliente_a];
	
				custo = dados->matriz_distancias[cliente_a][cliente] + dados->matriz_distancias[cliente_b][cliente] - dados->matriz_distancias[cliente_a][cliente_b];
		

				if (custo < melhor_custo){
					melhor_custo = custo;
					melhor_cliente_a = cliente_a;
					melhor_cliente_b = cliente_b;
					inseriu = true;
				}
				if (cliente_b == 0){
					break;
				}
				cliente_a = cliente_b;
			}
			
			/* Se o custo da terceirização for menor que o custo da inserção do cliente
			   devemos tercerizá-lo */
			int custo_terceirizacao_cliente = dados->custo_terceirizacao[cliente];
			if(custo_terceirizacao_cliente < melhor_custo and entregas_realizadas >= dados->minimo_entregas){

				clientes_ordenados.pop_back();
				this->funcao_objetivo += custo_terceirizacao_cliente;
				clientes_terceirizados.push_back(cliente);
				continue;

			}

			if(inseriu){
				veiculos[i].set_cliente(cliente, melhor_cliente_a); // Seta o pai de melhor_Cliente_a como cliente
				veiculos[i].set_cliente(melhor_cliente_b, cliente); // Seta o pai de cliente como melhorCliente_b

				int funcao_objetivo =  veiculos[i].get_objetivo() + melhor_custo;
				veiculos[i].set_objetivo(funcao_objetivo); // Seta a nova função objetivo do veiculo
		
				int novaCapacidade =  veiculos[i].get_capacidade_disp() - demanda_cliente;
				veiculos[i].set_capacidade_disp(novaCapacidade); // Seta a nova capacidade disponivel do veiculo
				clientes_ordenados.pop_back(); // Deleta o cliente
				entregas_realizadas++; // Indica que uma entrega foi realizada
				veiculos[i].incrementa_clientes(); // Indica que foi adicionado um cliente na rota

			}
		}

		this->funcao_objetivo +=  veiculos[i].get_objetivo();

	}
	

	/* Terceiriza todos os clientes restantes */
	for(int  i = 0; i < clientes_ordenados.size(); i++){
		Cliente cliente_data = clientes_ordenados[i]; // Armazena os dados do cliente 
		int indice_cliente = cliente_data.indice;
		clientes_terceirizados.push_back(indice_cliente); // Indica que foi terceirizado
		this->funcao_objetivo += dados->custo_terceirizacao[indice_cliente]; // Adiciona na função objetivo 
	}


}

void Heuristica::ILS(){
	
	cout << "Inicio guloso" << endl;
	auto inicio_guloso = std::chrono::high_resolution_clock::now();
	insercaoMaisBarata();	
	auto fim_guloso = std::chrono::high_resolution_clock::now() - inicio_guloso;
	long long millisecond_g = std::chrono::duration_cast<std::chrono::milliseconds>(fim_guloso).count();
	cout << "FO apos guloso: " << this->funcao_objetivo << endl;
	cout << "Tempo(s): " << millisecond_g / 1000 << endl;
	cout << "Fim Guloso"  << endl;
	

	cout <<  "Inicio primeiro VND " << endl;
	auto inicio_primeiro_vnd = std::chrono::high_resolution_clock::now();
	VND();
	auto fim_primeiro_vnd= std::chrono::high_resolution_clock::now() - inicio_primeiro_vnd;
	long long millisecond_vnd = std::chrono::duration_cast<std::chrono::milliseconds>(fim_primeiro_vnd).count();

	cout << "FO apos primeiro VND: " << this->funcao_objetivo << endl;
	cout << "Tempo(s): " << millisecond_vnd / 1000 << endl;
	cout << "Fim primeiro VND " << endl;


	/* Vamos executar o ILS 10 vezes	*/
	cout << "Inicio execuções ILS" << endl;
	auto inicio_ils = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < ILS_EXECUCOES; i++) {
		
		cout << "Inicio Perturbacao "  << endl;
		auto inicio_perturbacao = std::chrono::high_resolution_clock::now();
		perturbacao();
		auto fim_perturbacao= std::chrono::high_resolution_clock::now() - inicio_perturbacao;
		long long millisecond_perturbacao = std::chrono::duration_cast<std::chrono::milliseconds>(fim_perturbacao).count();
		cout << "FO apos primeiro perturbacao: " << this->funcao_objetivo << endl;
		cout << "Tempo(s): " << millisecond_perturbacao / 1000 << endl;
		cout << "Fim Perturbacao "  << endl;

		cout << "Inicio VND" <<  endl;
		auto inicio_segundo_vnd = std::chrono::high_resolution_clock::now();
		VND();
		auto fim_segundo_vnd= std::chrono::high_resolution_clock::now() - inicio_segundo_vnd;
		long long millisecond_segundo_vnd = std::chrono::duration_cast<std::chrono::milliseconds>(fim_segundo_vnd).count();
		cout << "FO apos VND: " << this->funcao_objetivo << endl;
		cout << "Tempo(s): " << millisecond_segundo_vnd / 1000 << endl;
		cout << "Fim VND "  << endl;

		if(this->funcao_objetivo < this->melhor_solucao.funcao_objetivo){
			this->melhor_solucao.veiculos = this->veiculos;
			this->melhor_solucao.clientes_terceirizados = this->clientes_terceirizados;
			this->melhor_solucao.funcao_objetivo = this->funcao_objetivo;
			cout << "Atualizou a melhor solucao" << endl;
		}
		
	
	}

	
	auto resultado = std::chrono::high_resolution_clock::now() - inicio_ils;
	long long millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(resultado).count();
	cout << "Solucao: " << this->melhor_solucao.funcao_objetivo << endl;
	cout << "Tempo(s): " << millisecond/1000.0 << endl;
	cout<< "Fim execuções ILS" << endl;
	printa_solucao_final();
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
				melhorou_solucao = swap_intrarota();
				break;
			case 2:
				melhorou_solucao = terceirizacao();
				break;
			case 3:
				melhorou_solucao = swap_interotas();
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

				}else if (i > j){        //em todos os casos iremos avaliar diminuir os custos que nao existiriam apos o mpvimento e somar os q existiram
				                         //oara saber se a troca é vantajosa

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
      	}

	}

	return houve_melhoria_rotas; //retorna se ouve melhoria para o VND
}

bool Heuristica::swap_interotas(){
	
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
	
	if(dados->minimo_entregas <= entregas_realizadas)     //se já está no minimo de entregas que devem ser feitas pelos veiculos sem terceirizar
		return 0;                                         //entao nao podemos testar o movimento de terceirizacao
	
	bool houve_melhoria_rotas = 0;
	vector<int> indice_v_retirados;       //vai guardar os indices dos veiculos a serem retirados se uma rota "deixar de existir" na terceirizacao

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

			if(rota.size() == 3){   //so existe 1 cliente no veiculo, entao se terceirizar o veiculo deixa de existir
				delta = -dados->matriz_distancias[rota[i-1]][rota[i]] -dados->matriz_distancias[rota[i]][rota[i+1]]
					    -dados->custo_veiculo + dados->custo_terceirizacao[rota[i]];
			}


			if (delta < melhor_delta){ // se delta for menor que o melhor delta atual significa que melhorou a solucao

				melhor_delta = delta;  //atualiza o delta pra uma nova comparacao
				melhor_i = i;          //guarda o indice i da melhor troca
			}
		}

		if (melhor_delta < 0){
			
			entregas_realizadas--;      //se terceirixa, menos uma entrega é feita  pelos veiculos proprios

			clientes_terceirizados.push_back(rota[melhor_i]);        //adicionando o cliente na lsita de terceirizados

			if(rota.size() == 3){
				indice_v_retirados.push_back(v);
			}else{

				veiculos[v].set_cliente(rota[melhor_i+1], rota[melhor_i-1]); 
				veiculos[v].set_cliente(-1, rota[melhor_i]);            	 	//o cliente que foi terceirizado, volta a apontar para -1
			
				veiculos[v].set_capacidade_disp(veiculos[v].get_capacidade_disp() + dados->demandas[rota[melhor_i]]);
				veiculos[v].set_objetivo( veiculos[v].get_objetivo() + melhor_delta - dados->custo_terceirizacao[rota[melhor_i]]);  //atualizando a funcao objetivo do veiculo              
			}

			funcao_objetivo = funcao_objetivo + melhor_delta;  //atualizando a funcao objetivo geral
			houve_melhoria_rotas = 1;
      	}
	}

	for(int i = indice_v_retirados.size() - 1; i >= 0; --i){            //retirando os veiculos com indices maiores primeiro no vetor veiculos
		veiculos.erase(veiculos.begin() + indice_v_retirados[i]);      
	}

	return houve_melhoria_rotas;
}

bool Heuristica::desterceirizacao(){

	bool houve_melhoria_rotas = 0;

	if(clientes_terceirizados.size() == 0){       //se nao tiver nenhum cliente a ser desterceirizado, nao pode aplicar o movimento
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
			
			entregas_realizadas++;     //se "desterceirizou", mais uma entrega é realizada pelos veiculos

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
	
bool Heuristica::swap_intrarota(){

	bool houve_melhoria_rotas = 0;
		
	for(int v = 0; v < veiculos.size(); v++){
		
		std::vector <int> rota;
		std::vector <int> *ptr_rota = veiculos[v].get_caminho_total();

		int ponto_rota = 0;
		rota.push_back(0);

		while(1){
			int cliente = (*ptr_rota)[ponto_rota];
			rota.push_back(cliente);
			if (cliente == 0){
				break;
			}else{
				ponto_rota = cliente;
			}
		}

		double delta,
		melhor_delta = 0;

		int melhor_i, melhor_j;

		for(int i = 1; i < rota.size() - 1; i++){

			for(int j = i+1; j < rota.size() - 1; j++){
					
				if(i == j-1){        //avalia o custo que o possivel novo movimento teria

					delta = -dados->matriz_distancias[rota[i-1]][rota[i]]  -dados->matriz_distancias[rota[j]][rota[j+1]]
							+dados->matriz_distancias[rota[i-1]][rota[j]] + dados->matriz_distancias[rota[i]][rota[j+1]];
				}
				else{

					delta = -dados->matriz_distancias[rota[i-1]][rota[i]] - dados->matriz_distancias[rota[i]][rota[i+1]]
						    -dados->matriz_distancias[rota[j-1]][rota[j]] - dados->matriz_distancias[rota[j]][rota[j+1]]
							+dados->matriz_distancias[rota[i-1]][rota[j]] + dados->matriz_distancias[rota[j]][rota[i+1]] 
							+dados->matriz_distancias[rota[j-1]][rota[i]] + dados->matriz_distancias[rota[i]][rota[j+1]];
				}

				if (delta < melhor_delta){    //se a solucao for melhor, isso significa um delta negativo e entra nesse if portanto
						
					melhor_delta = delta;
					melhor_i = i;
					melhor_j = j;
				}
					
			}
		}

		if(melhor_delta < 0){
			
			if(melhor_i == melhor_j-1){            //efetivando a mudanca na rota 
				veiculos[v].set_cliente(rota[melhor_j], rota[melhor_i-1]);
				veiculos[v].set_cliente(rota[melhor_i], rota[melhor_j]);
				veiculos[v].set_cliente(rota[melhor_j+1], rota[melhor_i]);

			}else{
				veiculos[v].set_cliente(rota[melhor_j], rota[melhor_i-1]);
				veiculos[v].set_cliente(rota[melhor_i+1], rota[melhor_j]);
				veiculos[v].set_cliente(rota[melhor_i], rota[melhor_j-1]);
				veiculos[v].set_cliente(rota[melhor_j+1], rota[melhor_i]);
			}

			veiculos[v].set_objetivo(veiculos[v].get_objetivo() + melhor_delta);      //atualizando o obejtivo do veiculo
			this->funcao_objetivo += melhor_delta;                                    //atualizando o objetivo geral 
			houve_melhoria_rotas = 1;

		}
	}
	return houve_melhoria_rotas;     //retorna que ouve melhoria
}

void Heuristica::perturbacao(){
	
	/* Vamos pegar 2 veiculos aleatorios enquanto fazemos shift entre rotas 1 a 1*/	
	srand((unsigned)time(NULL));
	
	int perturbacoes_realizadas = 0; /* Iremos fazer a perturbacao uma determinada quantia de vezes */

	while(perturbacoes_realizadas < QTD_PERTURBACAO){
		
		/* Se tiver apenas um veiculo não é possivel perturbar */
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

		/* Precisamos agora de duas posicoes aleatorias da rota para fazer o swap
		 * porem precisamos garantir que funcione por conta da capacidade de cada veiculo	*/
		int operacao_realizada = 0;

		while(operacao_realizada < 5){
			
			/* indices que terão seu proximo cliente alterado, caso o veiculo possua capacidade*/
			int i =  rand() % clientes_i.size();
			int j =  rand() % clientes_j.size();
			int prox_cliente_i = veiculos[indice_primeiro_veiculo].get_prox_cliente(clientes_i[i]);
			int prox_cliente_j = veiculos[indice_segundo_veiculo].get_prox_cliente(clientes_j[j]);
			
			
			int prox_final_i = veiculos[indice_primeiro_veiculo].get_prox_cliente(prox_cliente_i);
			int prox_final_j = veiculos[indice_segundo_veiculo].get_prox_cliente(prox_cliente_j);
			/* A nova capacidade dos veiculos será determinada por
			 * vamos retirar a capacidade do cliente anterior e adicionar a capacidade do novo cliente
			 * caso esse calculo seja maior ou igual a zero é porque podemos adicionar o cliente*/
			int nova_capacidade_i = veiculos[indice_primeiro_veiculo].get_capacidade_disp() + dados->demandas[prox_cliente_i]
				- dados->demandas[prox_cliente_j];
			int nova_capacidade_j = veiculos[indice_segundo_veiculo].get_capacidade_disp()
				+ dados->demandas[prox_cliente_j] - dados->demandas[prox_cliente_i];
			
		
			/* Vamos adicionar o prox_cliente_j em prox_cliente_i	*/

			if(nova_capacidade_i >= 0 and nova_capacidade_j >= 0){
				veiculos[indice_primeiro_veiculo].set_cliente(prox_cliente_j, clientes_i[i]); /* Seta o pai de cliente_i como sendo o proximo de j*/
				veiculos[indice_primeiro_veiculo].set_cliente(prox_final_i, prox_cliente_j); // O pai de proximo j será o proximo final de i

				// Cálculo da nova função objetivo do veiculo
				int funcao_objetivo = veiculos[indice_primeiro_veiculo].get_objetivo() - dados->matriz_distancias[clientes_i[i]][prox_cliente_i]
					- dados->matriz_distancias[prox_cliente_i][prox_final_i] + dados->matriz_distancias[clientes_i[i]][prox_cliente_j]
					+ dados->matriz_distancias[prox_cliente_j][prox_final_i];
			

				this->funcao_objetivo = this->funcao_objetivo - veiculos[indice_primeiro_veiculo].get_objetivo()
					+ funcao_objetivo;
				veiculos[indice_primeiro_veiculo].set_objetivo(funcao_objetivo);


				veiculos[indice_segundo_veiculo].set_cliente(prox_cliente_i, clientes_j[j]); // Seta o pai de cliente_j como o prox de i
				veiculos[indice_segundo_veiculo].set_cliente(prox_final_j, prox_cliente_i); // Seta o pai de prox_cliente_i como o prox_final de j

				/* Cálculo da nova função objetivo */
				funcao_objetivo = veiculos[indice_segundo_veiculo].get_objetivo() - dados->matriz_distancias[clientes_j[j]][prox_cliente_j]
					- dados->matriz_distancias[prox_cliente_j][prox_final_j] + dados->matriz_distancias[clientes_j[j]][prox_cliente_i]
					+ dados->matriz_distancias[prox_cliente_i][prox_final_j];

				this->funcao_objetivo = this->funcao_objetivo - veiculos[indice_segundo_veiculo].get_objetivo()
						+ funcao_objetivo;
				veiculos[indice_segundo_veiculo].set_objetivo(funcao_objetivo);

				veiculos[indice_primeiro_veiculo].set_capacidade_disp(nova_capacidade_i);
				veiculos[indice_segundo_veiculo].set_capacidade_disp(nova_capacidade_j);
				break;
			}

			operacao_realizada++;
		}

		perturbacoes_realizadas++;

	}
	
}

void Heuristica::printa_solucao_final(){
	

	string caminho = "resultados/saida-";
	string caminho_arq = caminho + dados->nome_instancia;

	std::ofstream arquivo(caminho_arq);


	int custo_terceiriz = 0;
	for(int i = 0; i < melhor_solucao.clientes_terceirizados.size(); i++){

		int terceirizado = melhor_solucao.clientes_terceirizados[i];
		custo_terceiriz += dados->custo_terceirizacao[terceirizado];
	}

	int custo_veiculos = (melhor_solucao.veiculos.size() * dados->custo_veiculo);

	if(arquivo.is_open()){
		arquivo << melhor_solucao.funcao_objetivo << endl;
		arquivo << melhor_solucao.funcao_objetivo - custo_terceiriz - custo_veiculos << endl;
		arquivo << custo_veiculos << endl;
		arquivo << custo_terceiriz << endl << endl;

 		for(int i = 0; i < melhor_solucao.clientes_terceirizados.size(); i++){         //printando os clientes terceirixados
			arquivo << clientes_terceirizados[i];
			arquivo << " ";
		}
		arquivo << endl << endl;

		arquivo << melhor_solucao.veiculos.size();

		for(int i = 0; i < veiculos.size(); i++){
			
			arquivo << endl;
			std::vector<int> rota;
			std::vector<int> *ptr_rota = veiculos[i].get_caminho_total();

			int ponto_rota = 0;
			while(1){

				int cliente = (*ptr_rota)[ponto_rota];
				
				if (cliente == 0){
					break;
				}else{
					ponto_rota = cliente;
					arquivo << cliente;
					arquivo << " ";
				}
			}
		}


		arquivo.close();
	}else{
		cout << "Erro ao criar arquivo!" << endl;
	}

	
}

Heuristica::~Heuristica(){
	
}
