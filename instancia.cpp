#include "instancia.h"
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <vector>

     
Instancia::Instancia(){
    q_clientes = 0;
    q_veiculos = 0;
    capacidade = 0;
    minimo_entregas = 0;
    custo_veiculo = 0;
}


void Instancia::lerInstancia (string caminho){
    
    // Tentando abrir o arrquivo da instancia
    ifstream arquivo_inst;
    arquivo_inst.open(caminho.c_str());

    if(!arquivo_inst){
        cout << "Não foi possivel abrir o arquivo da instancia"; 
    }

    // Ler os primeiros 5 atributos da instancia
    arquivo_inst >> q_clientes
                 >> q_veiculos
                 >> capacidade
                 >> minimo_entregas
                 >> custo_veiculo;
    
    int aux;
    
    demandas.push_back(INT_MAX);      //descartando o indice 0 do vetor para que a indexação fique direta                   
    for(int i = 0; i < q_clientes; i++){        //lendo o vetor de demanda de cada cliente
        arquivo_inst >> aux; 
        demandas.push_back(aux);
    }

    custo_terceirizacao.push_back(INT_MAX);      //descartando o indice 0 do vetor para que a indexação fique direta    
    for(int i = 0; i < q_clientes; i++){        //lendo o vetor de custos da terceirizacao de cada cliente
        arquivo_inst >> aux; 
        custo_terceirizacao.push_back(aux);
    }

    matriz_distancias.resize(q_clientes+1);     //definindo o tamanho da matriz de distancias com a quantidade de clientes + deposito

    for(int i = 0; i < matriz_distancias.size(); i++){  //lendo os valores da matriz de distancia
        for(int j = 0; j < matriz_distancias.size(); j++){
            arquivo_inst >> aux;
            matriz_distancias[i].push_back(aux);
        }
    }
	
}

void Instancia::exibir(){

    cout << "Clientes: " << q_clientes << endl
              << "Veículos " << q_veiculos << endl
              << "Capacidade: " << capacidade << endl
              << "Mínimo de entregas: " << minimo_entregas << endl
              << "Custo dos veículos: " << custo_veiculo << endl << endl;

    for(int i = 0; i < q_clientes; i++){

        cout << "Demanda cliente " << i+1 << ": " << demandas[i] << endl;
    }
    
    cout << endl;
    for(int i = 0; i < q_clientes; i++){

        cout << "Custo tercerização do cliente " << i+1 << ": " << custo_terceirizacao[i] << endl;
    }

    cout << endl << "Matriz de custos: " << endl;

    for(int i = 0; i < matriz_distancias.size(); i++){ 
        for(int j = 0; j < matriz_distancias.size(); j++){
            cout << matriz_distancias[i][j] << "  ";
        }
        cout << endl;
    }
}
