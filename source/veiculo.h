#ifndef VEICULO_H
#define VEICULO_H
#include <vector>
using namespace std;



class Veiculo{
	
	public:
		Veiculo();
		void edita_parametros(int veiculo, int demanda, int q_cliente, int custo_veiculo);
		int get_objetivo();
		void set_objetivo(int valor);
		void set_capacidade_disp(int valor);
		int get_capacidade_disp();
		void set_cliente(int cliente, int cliente_anterior); //Funcao responsável para dizer qual cliente foi colocado na rota
						
		int get_quantia_clientes();
		void incrementa_clientes();
		void set_quantia_clientes(int qtd_clientes);
		void decrementa_clientes();

		int printa_caminho_total(int cliente);
		vector <int> *get_caminho_total(){return &this->caminho_total;};
		void adiciona_custo_veiculo();
		int get_prox_cliente(int clienteAnterior);

	private:
		vector <int> caminho_total; // Vetor de caminho total que será boleano para fazer a inserção em O(1)
		int veiculo; // Veiculo da solucao
		int funcao_objetivo;
		int capacidade_disponivel;
		int custo;
		int qtd_clientes;

};





#endif
