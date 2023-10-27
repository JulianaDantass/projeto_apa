#ifndef HEURISTICA_H
#define HEURISTICA_H
#include "instancia.h"
#include "veiculo.h"
#include <vector>

using namespace std;



class Heuristica{
	public:
		Heuristica(Instancia* dados);
		~Heuristica();
		void resolve();
		void insercaoMaisBarata();	
		Veiculo solucaoInicial(int indiceSolucao);
		void VND();

		//metodos para as estruturas de vizinhanca
		bool reinsertion();
		bool swapEntreRotas();
		bool crossover();
		bool terceirizacao();
		bool desterceirizacao();

		void printVector(vector<int> vetor); //deve ser eliminado

	private:
		vector <Veiculo> veiculos;
		vector <int> clientesTerceirizados;
		int clientesAtendidos;
		vector <int> clientesDisponiveis;
		int entregasRealizadas;
		Instancia *dados;
		int funcaoObjetivo;
};




#endif
