#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector> 
#include "instancia.h"
 
using namespace std;    


int main(int argc, char** argv){
    
    
    if(argc < 2){
        printf("Comando: ./'nome do executavel' instancias/'instancia escolhida'\n");

        return 0;

    }else if(argc >= 2){
        
        Instancia current_instance;

        string filename = string(argv[1]);

        current_instance.lerInstancia(filename);  
        current_instance.exibir();            
        
    }
    

    return 0;
}