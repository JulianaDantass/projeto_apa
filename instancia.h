#ifndef INSTANCIA_H_INCLUDED
#define INSTANCIA_H_INCLUDED

    #include <iomanip>
    #include <fstream>
    #include <vector>
	#include "heuristica.h"
    using namespace std;

    class Instancia{
        public: 
            int clientes,
                veiculos,
                capacidade,
                minimo_entregas,
                custo_veiculo;

            vector<int> demandas, 
                        custo_terceirizacao;

            vector<vector<int>> matriz_distancias;
			Heuristica* heuristica;
        public: 
            Instancia();
			~Instancia();
            Heuristica* lerInstancia(string caminho_instancia);
            void exibir();
    };


#endif // INSTANCE_H_INCLUDED
