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
		bool swapEntreRotas();
		bool crossover();
		bool terceirizacao();
		bool desterceirizacao();

		void printVector(vector<int> vetor); //deve ser eliminado

	private:
		vector <Veiculo> veiculos;
		vector <int> clientes_terceirizados;
		int clientesAtendidos;
		vector <int> clientes_disponiveis;
		int entregas_realizadas;
		Instancia *dados;
		int funcao_objetivo;
		Solucao melhor_solucao;
};




#endif
