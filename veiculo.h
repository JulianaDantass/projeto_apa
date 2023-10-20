#ifndef VEICULO_H
#define VEICULO_H
#include <vector>
using namespace std;



class Veiculo{
	
	public:
		Veiculo(int veiculo, int demanda, int quantidadeCliente, int custoVeiculo);
		void insereCaminhoFim(int cliente);
		int getObjetivo();
		void setObjetivo(int valor);
		void setCapacidade(int valor);
		int getCapacidade();
		int getUltimoCliente();
		void setCliente(int cliente, int cliente_anterior); //Funcao responsável para dizer qual cliente foi colocado na rota
						
		vector<int>* getCaminhoInicial();
		int printaCaminhoTotal(int cliente);
		vector < int>* getCaminhoTotal(){return &this->caminhoTotal;};
		void setCustoVeiculo();
		void clearCaminhoInicial(){this->caminho.clear();};
		int getProxCliente(int clienteAnterior);

	private:
		vector < int > caminho; // Caminho percorrido por aquele veiculo
		vector < int > caminhoTotal; // Vetor de caminho total que será boleano para fazer a inserção em O(1)
		int veiculo; // Veiculo da solucao
		int funcao_objetivo;
		int capacidade;
		int custo;

};





#endif
