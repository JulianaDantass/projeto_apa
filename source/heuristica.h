#ifndef HEURISTICA_H
#define HEURISTICA_H
#include "instancia.h"
#include "veiculo.h"
#include <vector>

using namespace std;

typedef struct{
	vector < Veiculo > veiculos;
	vector < int > clientes_terceirizados;
	int funcao_objetivo;

}Solucao;

typedef struct{
	int demanda;
	int indice;
}Cliente;

class Heuristica{
	public:
		Heuristica(Instancia* dados);
		~Heuristica();
		void ILS();
		void insercaoMaisBarata();	
		Veiculo solucaoInicial(int indiceSolucao);
		void VND();
		void perturbacao();
		void demanda_mais_barata();	
		//metodos para as estruturas de vizinhanca
		bool reinsertion();
		bool swap_interotas();
		bool crossover();
		bool terceirizacao();
		bool desterceirizacao();
		bool swap_intrarota();

		void printa_solucao_final();


	private:
		vector <Veiculo> veiculos;
		vector < Cliente > clientes_ordenados;
		vector <int> clientes_terceirizados;
		vector <int> clientes_disponiveis;
		int entregas_realizadas;
		Instancia *dados;
		int funcao_objetivo;
		Solucao melhor_solucao;
};




#endif
