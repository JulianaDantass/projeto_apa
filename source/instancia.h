#ifndef INSTANCIA_H_INCLUDED
#define INSTANCIA_H_INCLUDED

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

class Instancia{
    public: 
        int q_clientes,
            q_veiculos,
            capacidade,
            minimo_entregas,
            custo_veiculo;

        vector<int> demandas, 
                    custo_terceirizacao;

        vector<vector<int>> matriz_distancias;

    public: 
        Instancia();
        void lerInstancia(string caminho_instancia);
        void exibir();
};


#endif // INSTANCE_H_INCLUDED
