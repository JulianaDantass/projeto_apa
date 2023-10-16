#ifndef SOLUCAO_H
#define SOLUCAO_H
#include <vector>
using namespace std;



class Solucao{
	
	public:
		Solucao(int veiculo, int demanda);
		void insereCaminhoFim(int cliente);
		int getObjetivo();
		void setObjetivo(int valor);
		void setCapacidade(int valor);
		int getCapacidade();
		vector<int>* getCaminho();

	private:
		vector < int > caminho; // Caminho percorrido por aquele veiculo
		int veiculo; // Veiculo da solucao
		int funcao_objetivo;
		int capacidade;

};





#endif
