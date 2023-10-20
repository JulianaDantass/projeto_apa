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
		void alocarRecursos();
		void solve();
		void insercaoMaisBarata();	
		void solucaoInicial(int indiceSolucao);
		int calculaObjetivoIda(int cliente_anterior, int cliente_atual, int objetivo);
		void VND();

	private:
		vector < Veiculo* > *veiculos;
		vector < int > clientesTerceirizados;

		vector < int > clientesOrdenados;
		int entregasRealizadas;
		Instancia *dados;
		int funcaoObjetiva;
};




#endif
