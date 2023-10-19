#ifndef HEURISTICA_H
#define HEURISTICA_H
#include "instancia.h"
#include "veiculo.h"
#include <vector>
// using namespace std;

class Heuristica{
	public:
		Heuristica();
		~Heuristica();
		void alocarRecursos();
		void solve(Instancia *dados);
		void insercaoMaisBarata();	
		void solucaoInicial(int indiceSolucao, vector < bool >&clientesQueue);
		int calculaObjetivo(int cliente_anterior, int cliente_atual, int objetivo);
		void VND();

	private:
		vector < Veiculo* > *veiculos;
		Instancia *dados;
};




#endif
