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
		vector < Cliente > clientes_ordenados; /* Clientes ordenados em ordem decrescente em relação a demanda */
		vector <int> clientes_terceirizados; /* Vetor que armazena os clientes que serão terceirizados */
		int entregas_realizadas; /* Variavel que indica a quantia de entregas realizadas por veiculos */
		Instancia *dados;
		int funcao_objetivo;
		Solucao melhor_solucao;
};




#endif
