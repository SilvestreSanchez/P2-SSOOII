/*
---- BUSCADOR DE PALABRAS SOOIIGLE ----
Usage: <./prac2 file_name word n_threads>
AUTHOR: Silvestre Sanchez-Bermejo Sanchez
DATE: 16/04/2021
DESCRIPTION: Search word's ocurrences on a .txt file 
DESCRIPCION: Busca ocurrencias de una palabra en un archivo .txt
*/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <algorithm>
#include <bits/stdc++.h>
#include <queue>
#include <mutex>
#include "ocurrencia.cpp"
#include "wordcleaner.cpp"
#include "colores.h"

std::vector <std::string> v_Lines;
std::mutex semaforo;
std::priority_queue<Ocurrencia, std::vector<Ocurrencia>, cmpFunction> pq;


void tokenizar(std::vector<std::string> &v_tokens,int line){
  std::stringstream check1(v_Lines[line]);
  std::string token;
    
  while(getline(check1, token, ' ')){
    token=cleanWord(token);
    v_tokens.push_back(token);
  }
}


void addPQ(Ocurrencia oc){
  std::lock_guard<std::mutex> lock(semaforo);
  pq.push(oc);
}

void encontrar(int inicio, int final,std::string word,int ID){
  int linea = inicio;
  /*Itera linea a linea*/
  for(linea;linea<=final;linea++){
    
    std::vector<std::string> v_tokens;
    tokenizar(v_tokens,linea);
    
    int last_index=0;
    int n_ocurrencias = std::count(v_tokens.begin(),v_tokens.end(), word);
    /*Itera por cada ocurrencia de la linea*/
    for(int i=0;i<n_ocurrencias;i++){
      auto it= std::find(v_tokens.begin()+last_index,v_tokens.end(), word);
      if (it != v_tokens.end()){
	int index= it -v_tokens.begin();
	last_index=index+1;
	if (index==0 && v_tokens.size()==1){ /*Unica palabra en la linea*/
	  Ocurrencia x(ID,inicio,final,linea,word,"","");
	  addPQ(x);
	}else if (index== v_tokens.size() -1){ /*Ultima palabra de la linea*/
	  Ocurrencia y(ID,inicio,final,linea,word,v_tokens[index-1],""); 
	  addPQ(y);
	}else if(index == 0){ /*Primera palabra de la linea*/
	  Ocurrencia z(ID,inicio,final,linea,word,"",v_tokens[index+1]); 
	  addPQ(z);
	}else{ /*Encontrado entre dos palabras*/
	  Ocurrencia o(ID,inicio,final,linea,word,v_tokens[index-1],v_tokens[index+1]);
	  addPQ(o);
	}
      }
    }
  }
}

std::ifstream get_Args(int &hilos,std::string &w,int argc, char *argv[]){
  if (argc!=4){
    std::cout << "Error en los argumentos. Uso: ./prac2 nombre_archivo palabra n_hilos" << std::endl;
    exit(-1);
  }
  std::string aux= argv[3];
  if (aux.find_first_not_of("0123456789") != std::string::npos){
    std::cout << "Introduzca un numero de hilos correcto" << std::endl;
    exit(-1);
  }
  hilos=std::stoi(argv[3]);
  w= argv[2];
  transform(w.begin(),w.end(),w.begin(),::tolower);
  std::ifstream file(argv[1]);
  return file;
}

void leerArchivo(std::ifstream &file){
  std::string line;
  getline(file,line);
  while(!file.eof()){
    transform(line.begin(),line.end(),line.begin(),::tolower);
    v_Lines.push_back(line);
    getline(file,line);   
  }
}

void crearHilos(std::vector<std::thread> &v_Hilos,int N_HILOS,std::string word){
  int n_lineas= v_Lines.size();
  int n_lineas_hilo= n_lineas/N_HILOS;
  
  int inicio=0;
  
  for(int i=1;i<=N_HILOS;i++){
    int final;
    final=(n_lineas_hilo*i)-1;
    /* Si es el ultimo hilo , debe acabar con la ultima linea*/
    if(i == N_HILOS)
      final=n_lineas-1;
    v_Hilos.push_back(std::thread(encontrar,inicio,final,word,i));
    inicio=final+1;
  }
}


void mostrarResultados(){
  int n=0;
  while(!pq.empty()){
    Ocurrencia o = pq.top();
    
    std::cout<< COLOR_CYAN <<"[HILO "<<o.ID_Hilo<< " inicio: "<<o.inicio<<" - final: "<<o.fin<< "] :: "<< COLOR_RESET <<"linea "<<o.linea<< " :: ... " << o.prev_word <<" " <<COLOR_RED <<o.word<<" " <<COLOR_RESET <<o.next_word << " ..."<<std::endl;
    
    pq.pop();
    n++;
  }
  std::cout << "Numero de ocurrencias: " << n << std::endl;
}


int main(int argc, char *argv[]){

  std::vector<std::thread> v_Hilos;
  int N_HILOS;
  std::string word;
  std::ifstream file;
  
  file=get_Args(N_HILOS, word,argc,argv);
 
  if(file.is_open()){
    leerArchivo(file);
  }else{
    std::cout << "ERROR: Archivo no encontrado" << std::endl;
    exit(-1);
  }
  crearHilos(v_Hilos,N_HILOS,word);
  
  std::for_each(v_Hilos.begin(),v_Hilos.end(),std::mem_fn(&std::thread::join));
  mostrarResultados();
  return EXIT_SUCCESS;
}
