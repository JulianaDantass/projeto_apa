#include "instancia.h"
#include <iostream>
#include <fstream>
#include <string>

     

Instancia::Instancia(){
    clientes = 0;
    veiculos = 0;
    capacidade = 0;
    minimo_entregas = 0;
    custo_veiculo = 0;
}


bool Instancia::lerInstancia (string caminho){
    
    // Tentando abrir o arrquivo da instancia
    ifstream arquivo_inst;
    arquivo_inst.open(caminho.c_str());

    if(!arquivo_inst){
        std::cout << "Não foi possivel abrir o arquivo da instancia"; 
        return false;
    }

    // Ler os primeiros 5 atributos da instancia
    arquivo_inst >> clientes
                 >> veiculos
                 >> capacidade
                 >> minimo_entregas
                 >> custo_veiculo;
    
    int aux;

    for(int i = 0; i < clientes; i++){        //lendo o vetor de demanda
        arquivo_inst >> aux; 
        demandas.push_back(aux);
    }

    for(int i = 0; i < clientes; i++){        //lendo o vetor de custos da terceirizacao
        arquivo_inst >> aux; 
        custo_terceirizacao.push_back(aux);
    }

    matriz_distancias.resize(clientes+1);     //definindo o tamanho da matriz de distancias com a quantidade de clientes + deposito

    for(int i = 0; i < matriz_distancias.size(); i++){  //lendo os valores da matriz de distancia
        for(int j = 0; j < matriz_distancias.size(); j++){
            arquivo_inst >> aux;
            matriz_distancias[i].push_back(aux);
        }
    }

    return true;
}

void Instancia::exibir(){

    std::cout << "Clientes: " << clientes << endl
              << "Veículos " << veiculos << endl
              << "Capacidade: " << capacidade << endl
              << "Mínimo de entregas: " << minimo_entregas << endl
              << "Custo dos veículos: " << custo_veiculo << endl << endl;

    for(int i = 0; i < clientes; i++){

        std::cout << "Demanda cliente " << i+1 << ": " << demandas[i] << endl;
    }
    
    cout << endl;
    for(int i = 0; i < clientes; i++){

        std::cout << "Custo tercerização do cliente " << i+1 << ": " << custo_terceirizacao[i] << endl;
    }

    std::cout << endl << "Matriz de custos: " << endl;

    for(int i = 0; i < matriz_distancias.size(); i++){ 
        for(int j = 0; j < matriz_distancias.size(); j++){
            std::cout << matriz_distancias[i][j] << "  ";
        }
        std::cout << endl;
    }
}
