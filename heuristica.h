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
		void printVector(vector<int> vetor);
		void insercaoMaisBarata();	
		Veiculo solucaoInicial(int indiceSolucao);
		int custoIda(int cliente_anterior, int cliente_atual, int objetivo);
		void VND();

		//metodos para as estruturas de vizinhanca
		bool reinsertion();
		bool swapEntreRotas();

	private:
		vector <Veiculo> veiculos;
		vector <int> clientesTerceirizados;
		int clientesAtendidos;
		vector <int> clientesOrdenados;
		int entregasRealizadas;
		Instancia *dados;
		int funcaoObjetivo;
};




#endif
