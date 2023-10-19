#ifndef HEURISTICA_H
#define HEURISTICA_H

#include "veiculo.h"
#include <vector>
using namespace std;

class Heuristica{
	public:
		Heuristica(int clientes, int qVeiculos, int capacidade, int minimo_entregas, int custo_veiculo, 
				vector < int> *demanda, vector <int> * custo_terceirizacao, vector <vector<int>>* matriz_distancia);
		~Heuristica();
		void solve();
		void insercaoMaisBarata();	
		void solucaoInicial(int indiceSolucao, vector < bool >&clientesQueue);
		int calculaObjetivo(int cliente_anterior, int cliente_atual, int objetivo);
		void VND();

	private:
		int clientes;
		int qVeiculos;
		int capacidade;
		int minimo_entregas;
		int custo_veiculo;
		vector <int> * demanda;
		vector <int> * custo_terceirizacao;
		vector <vector<int>>* matriz_distancia;
		vector < Veiculo* > *veiculos;
		vector < int > clientesOrdenados;
};




#endif
