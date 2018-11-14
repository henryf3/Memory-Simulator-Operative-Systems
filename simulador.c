//Name: Caraguay, H.  && Seraquive, J.
#include <stdio.h>
#include <stdlib.h> // For exit() function
#include <string.h>
#include <stdbool.h>

int debug;

int redwri=0, faults=0;

/*-----------------------------VERSION 2--------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/


//Values: --ID--, --Pagina--, --Referencia--, --Dirty--, --Clock
int pagetablev2[32][4];


void setinitialvaluestwo(void){
  for (int i=0; i<32; i++){
    pagetablev2[i][0]=-1; // id process to -1
    pagetablev2[i][1]=-1; // page to -1
    pagetablev2[i][2]=0; // dirty bit process to 0
    pagetablev2[i][3]=0; // clock(number_instruction)
  }
}

//To search if there is a freepage
bool searchfreepage2(int pagetable[32][4], int * indexfree){
	bool isfree = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][1]==-1){
			isfree = true;
			*indexfree = i;
			break;
		}
	}
	return isfree;
}
//This funciton search by ID process and page
bool searchbypair2(int idproc, int page, int * index){
	bool ispresent=false;
	for (int i=0; i<32; i++){
		if (pagetablev2[i][0]==idproc && pagetablev2[i][1]==page){
			ispresent = true;
			*index = i;
			break;
		}
	}
	return ispresent;
}

bool searchfirstcase2(int pagetable[32][4], int * index){
  bool firstcase = true;
  int minvalue = pagetable[0][3];
  int minindex = 0;
  int counter = 0;

  for (int i=1; i<32; i++){
    if (pagetable[i][3] <= minvalue){ //= porque se revisa si hay dos valores
      minvalue = pagetable[i][3];
      minindex = i;
    }
  }


  for (int j=0;j<32;j++){
    if (pagetable[j][3] == minvalue){
      counter++;
    }
  }


  if (counter == 2){
    firstcase = false;
  }else if (counter == 1) {
    *index = minindex;
    if(pagetable[minindex][2] == 1){ redwri++;} //aumentamos en uno la referencia de disco si la página que remplazamos se encuentra en dirty
  }
  return firstcase;
}

bool searchsecondcase2(int pagetable[32][4], int * index){
  bool secondcase = false;
  int minvalue = pagetable[0][3];

  int minindex1 = 0;
  int minindex2 = 0;

  for (int i=1; i<32; i++){
    if (pagetable[i][3] < minvalue){
      minvalue = pagetable[i][3];
      minindex1 = i;
    }
  }

  for (int j = 0; j<32; j++){
    if ((pagetable[j][3] == minvalue) && (j != minindex1)){
      minindex2 = j;
    }
  }

  if (pagetable[minindex1][2] != pagetable[minindex2][2]){ //comprobamos que una este limpia y una sucia
    secondcase = true;
    if (pagetable[minindex1][2] == 0){ //buscamos la pagina que no esta sucia
      *index = minindex1;
    }else if(pagetable[minindex2][2] == 0){
      *index = minindex2;
    }
  }
  return secondcase;
}

bool searchthirdcase2(int pagetable[32][4], int * index){
  bool thirdcase = false;
  int minvalue = pagetable[0][3];

  int minindex1 = 0;
  int minindex2 = 0;

  for (int i=1; i<32; i++){
    if (pagetable[i][3] < minvalue){
      minvalue = pagetable[i][3];
      minindex1 = i;
    }
  }

  for (int j = 0; j<32; j++){
    if ((pagetable[j][3] == minvalue) && (j != minindex1)){
      minindex2 = j;
    }
  }

  if (pagetable[minindex1][2] == pagetable[minindex2][2]){ //comprobamos que una este limpia y una sucia
    thirdcase = true;

     if (pagetable[minindex1][1] < pagetable[minindex2][1]){ //buscamos la pagina que no esta sucia
       if(pagetable[minindex1][2] == 1){ //aumentamos en dos la referencia de disco si la página que remplazamos se encuentra en dirty
          redwri++;
       }
       *index = minindex1;

    }else if (pagetable[minindex2][1] < pagetable[minindex1][1]){
       if(pagetable[minindex2][2] == 1){//aumentamos en dos la referencia de disco si la página que remplazamos se encuentra en dirty
         redwri++;
       }
      *index = minindex2;
    }
  }
  return thirdcase;
}


int searchind_two(int idproc, int page, bool * is_in_table ){
  int index;

  //Search by process and page
  *is_in_table = searchbypair2(idproc, page, &index);

  if(!*is_in_table){
    faults++; //Add a fault because the page is not in the simulator


    if (searchfreepage2(pagetablev2, &index)){
      return index;

    }else if (searchfirstcase2(pagetablev2, &index)){  //seguimiento de la pagina que se hizo referencia pro ultima vez */
      //redwri -> se añade dependiendo si la pagina a remplazar se encuentra limpia o no
      return index;

     }else if (searchsecondcase2(pagetablev2, &index)){ //si hay dos paginas deveolvemos la limpia
       return index;

     }else if (searchthirdcase2(pagetablev2, &index)) {//si ambas estan sucias o limpias devolvemos la pagina con menor número */
      //redwri -> se añade dependiendo si la pagina a remplazar se encuentra limpia o no
      return index;
    }
  }else{
    return index;
   }
}



void replace_two(int id_process, int instruction, int memoryreference, char * caracter, int number_instruction){
  int index;
  int page = instruction/512;
  bool is_in_table;

  index = searchind_two(id_process, page, &is_in_table);

  if (!is_in_table && debug==1) {
    printf("%d %d %d %d %d \n", number_instruction, index, pagetablev2[index][0], pagetablev2[index][1], pagetablev2[index][3]);
  }

  pagetablev2[index][0]=id_process;
  pagetablev2[index][1]=page;
  pagetablev2[index][3]=number_instruction;
  pagetablev2[index][2]=0;

  page = memoryreference/512;

  index = searchind_two(id_process, page, &is_in_table);
  if (!is_in_table && debug==1) {
    printf("%d %d %d %d %d \n", number_instruction, index, pagetablev2[index][0], pagetablev2[index][1], pagetablev2[index][3]);
  }


  pagetablev2[index][0]=id_process;
  pagetablev2[index][1]=page;
  pagetablev2[index][3]=number_instruction;
  if (*caracter == 'W'){
    pagetablev2[index][2]=1;//dirty
  }

  if (*caracter == 'R' && !is_in_table){
		pagetablev2[index][2] = 0;   //dirty
	}


}
/*-----------------------------VERSION 1--------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
//Values: --ID--, --Pagina--, --Referencia--, --Dirty--     version 1
int pagetable[32][4];

//To search if there is a freepage
bool searchfreepage(int * index){
	bool isfree = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][0]==0){
			isfree = true;
			*index = i;
			break;
		}
	}
	return isfree;
}
// Reference and dirty bits
bool searchfirstcase(int * index){
	bool firstcase = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][2]==0 && pagetable[i][3]==0){
			firstcase = true;
			*index = i;
			break;
		}
	}
	return firstcase;
}
bool searchsecondcase(int * index){
	bool secondcase = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][2]== 0 && pagetable[i][3]==1){
			secondcase = true;
			*index = i;
			break;
		}
	}
	return secondcase;
}
bool searchthirdcase(int * index){
	bool thirdcase = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][2]==1 && pagetable[i][3]==0){
			thirdcase = true;
			*index = i;
			break;
		}
	}
	return thirdcase;
}
bool searchforthcase(int * index){
	bool forthcase = false;
	for (int i=0; i<32; i++){
		if (pagetable[i][2]==1 && pagetable[i][3]==1){
			forthcase = true;
			*index = i;
			break;
		}
	}
	return forthcase;
}
//Esta funcion busca si el processo y su pagina respectiva ya estan en pagetable
bool searchbypair(int idproc, int page, int * index){
	bool ispresent=false;
	for (int i=0; i<32; i++){
		if (pagetable[i][0]==idproc && pagetable[i][1]==page){
			ispresent = true;
			*index = i;
			break;
		}
	}
	return ispresent;
}

int searchind_one(int idproc, int page, bool * is_in_table){
	int index;
	//Buscamos por id de proceso y pagina.
	*is_in_table = searchbypair(idproc,page,&index);

	if (!*is_in_table){
		faults++; //Contamos un fallo cada vez que no esta en la tabla una pagina especifica.
		if (searchfreepage(&index)){
			return index;
		}else if(searchfirstcase(&index)){//00
			return index;
		}else if(searchsecondcase(&index)){//01
			redwri++;
			return index;
		}else if(searchthirdcase(&index)){//10
			return index;
		}else if(searchforthcase(&index)){//11
			redwri++;
			return index;
		}
	}else if(*is_in_table){
		return index;
	}

}

//Values: --ID--, --Pagina--, --Referencia--, --Dirty--     version 1
//code for algorithm replacement version 1
void replace_one(int id_process, int instruction, int memoryreference, char * caracter, int number_instruction){
	int index;
	int page = instruction/512;

	bool is_in_table;

	index = searchind_one(id_process, page, &is_in_table);

	if (!is_in_table && debug==1) {
    printf("%d %d %d %d %d \n ", number_instruction, index, pagetable[index][0], pagetable[index][1], pagetable[index][3]);
	}


	pagetable[index][0]=id_process;
	pagetable[index][1]=page;
	pagetable[index][2]=1;
	pagetable[index][3]=0;


	page = memoryreference/512;
	index = searchind_one(id_process, page, &is_in_table);

	if (!is_in_table && debug==1) {
    printf("%d %d %d %d %d \n ", number_instruction, index, pagetable[index][0], pagetable[index][1], pagetable[index][3]);
	}

	pagetable[index][0] = id_process;
	pagetable[index][1] = page;
	pagetable[index][2] = 1;

	if (*caracter == 'W'){
		pagetable[index][3] = 1;   //dirty
		//~ printf("Haciendo escritura \n");
	}

	if (*caracter == 'R' && !is_in_table){
		pagetable[index][3] = 0;   //dirty
	}

}

//In this function we set -1 to all values inside pagetable.
void setinitialvaluesone(void){
    for (int i=0; i<32; i++){
        pagetable[i][0]=0; // id process to 0
        pagetable[i][1]=0; // page to 0
        pagetable[i][2]=0; // reference bit to 0
        pagetable[i][3]=0; // dirty bit process to 0
      }
}

void setreferencetozero(void){
	for (int i=0; i<32; i++){ //all id process to -1
       pagetable[i][2]=0;
	}
}

/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/




/*
El simulador debe recibir algo como:
simulator datafile.txt version [1]
* simulator ----> nombre del programa  0
* datafile.txt----> archivo con data   1
* version ----> numero 1-2 para indicar algoritmo de reemplazo  2
* [1] ----> debug parametro opcional   3
*/
int main(int argc, char *argv[])
{
	//Verifying if the number of arguments is correct
	if (argc<3 || argc>4){
		printf("Invalid number of arguments \nClosing simulator.\n"); exit(1);
    }

	//Setting debug variable
	if (argc==4) debug=1;
	else debug=0;

	//~ --------------------------------------------------------------------------
	//Opening file with data, and storing each line in four distinct variables.
	char * name_file = argv[1]; //Name of file
	int version = atoi(argv[2]); //Number of version

	FILE *fptr;

  //variable used to storing the values in the file
	int id_process, instruction, memoryreference;
	char * caracter;

	setinitialvaluesone();
	setinitialvaluestwo();

	//Opening file
	fptr = fopen(name_file, "r");
	if (fptr == NULL)
	{
		printf("Error! opening file");
		// Program exits if file pointer returns NULL.
		exit(1);
	}

	int contador=1;
	while (1){

		fscanf(fptr, "%d %d %d %c", &id_process, &instruction, &memoryreference, caracter);
		if (feof(fptr)) break;

		if (version==1){
			//Setting initial values
			replace_one(id_process, instruction, memoryreference, caracter, contador);
		}else if (version ==2){
			//Setting initial values
			replace_two(id_process, instruction, memoryreference, caracter, contador);
		}

		if(contador%200==0){ //To establish every 200 instructions the reference bit to zero
			setreferencetozero();
		}
		contador++;
	}
	fclose(fptr);


	printf("REPORTE FINAL VERSION %d \n", version);
	printf("Total de fallas %d \n", faults );
	printf("Total de escrituras %d \n", redwri );
	
	
	if(debug == 1){
      printf("Debug activated \n");
	 }

    return 0;
}
