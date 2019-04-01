/*
    microshell.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>

#include "parser.h"


int mipwd();
int mils(char *opcional, char *carpeta);
int mimkdir(const char *carpeta);
int mirmdir(const char *carpeta);
int micd(const char *carpeta, char *pt);
int micat(char *fichero);
int micp(char *original, char *copia);
int mirm(const char *fichero);
int miexit ();

int mils1();
int mils2(char *carpeta);
int mils3();
int mils4(char *carpeta);



///////////////////////////// PROGRAMA PRINCIPAL /////////////////////////////////

int main () {

    orden O;
    int r;

	// Obtenemos la ruta desde la que se inicia la shell, porque la necesitamos luego
	char rut[2048];				// Aqui guardamos la ruta
	char *puntero;				// Puntero que apunta a la direccion donde se guarda la ruta
	puntero = getcwd(rut,2048);	// Obtenemos la ruta
	int correcto;				// Para poner si la orden se ha ejecurado correctamente

    printf ("Introduzca órdenes (pulse Ctrl-D para terminar)\n");

    do {		// Leer ordenes (y mostrarlas)

		correcto = 0; 
		inicializar_orden (&O);
        printf ("$ ");
        r = leer_orden (&O, stdin);


		// Evitamos que el programa falle si el usuario pulsa INTRO sin haver escrito nada
		while (O.argc == 0) {
			inicializar_orden (&O);
	        printf ("$ ");
	        r = leer_orden (&O, stdin);
		}


        if (r < 0) {fprintf (stderr, "\nError %d: %s\n", -r, mensajes_err[-r]);}
        else {	// Seleccionamos que funcion utilizar dependiendo de la orden introducida
			char *orden = O.argv[0];		// Guarda que funcion se va a usar
			if (strcmp("mipwd",orden) == 0)	{
				correcto = mipwd();}					
			else{
			if (strcmp("mils",orden) == 0) {
				correcto = mils(O.argv[1],O.argv[2]);}
			else{
			if (strcmp("mimkdir",orden) == 0) {
				correcto = mimkdir(O.argv[1]);}
			else{
			if (strcmp("mirmdir",orden) == 0) {
				correcto = mirmdir(O.argv[1]);}		
			else{
			if (strcmp("micd",orden) == 0) {
				correcto = micd(O.argv[1],puntero);}	
			else{
			if (strcmp("micat",orden) == 0) {
				correcto = micat(O.argv[1]);}
			else{
			if (strcmp("micp",orden) == 0) {
				correcto = micp(O.argv[1],O.argv[2]);}
			else{
			if (strcmp("mirm",orden) == 0) {
				correcto = mirm(O.argv[1]);}
			else{
			if (strcmp("exit",orden) == 0) {
				correcto = miexit();}
			else{
				printf ("\"%s\": no se ha encontrado la orden\n", orden);}}}}}}}}}

			if (correcto == -1) {
				printf ("\"%s\": no se ha podido ejecutar la orden\n", orden);
			}
		}
		
        liberar_orden (&O);
    } while (r==0);   // Repetir hasta error o EOF

    return 0;
}





///////////////////////////// FUNCIONES DE LAS ORDENES //////////////////////////////

// Muestra la ruta del directorio actual
int mipwd() {
	char ruta[2048];			// Aqui guardamos la ruta
	char *ptr;					// Puntero que apunta a la direccion donde se guarda la ruta
	ptr = getcwd(ruta,2048);	// Obtenemos la ruta
	printf("%s\n",ptr);			// Imprimimos el resultado
	return 0;
}



// Muestra un listado de las entradas de la carpeta (la actual si no se pasa ninguna por parametro), ademas se le puede anadir el parametro "-l" para detallar mas datos de cada entrada
int mils(char *opcional, char *carpeta) {
	int correct = 0;
	// Vemos en que situacion estamos segun los parametros introducidos
	if(opcional == NULL) {			// No se ha introducido ningun parametro
		correct = mils1();
	}
	else {	// Se ha introducido al menos un parametro, comprobamos si es la opcion o la carpeta
		if((strcmp (opcional,"-l")) == 0) {
			// Tenemos el parametro opcion, comprobamos si tambien el parametro carpeta
			if(carpeta == NULL) {	// Solo esta el parametro opcion
				correct = mils3();
			} else {				// Esta el parametro opcion y la carpeta
				correct = mils4(carpeta);
			}
		}
		else {	// Como hay un parametro y no es la opcion, debe ser la carpeta
			correct = mils2(opcional);	// Aunque le llame opcional es la carpeta
		}
	}
	return correct;
}



// "mils" sin parametros
int mils1() {
	char ruta[2048];			// Aqui guardamos la ruta
	getcwd(ruta,2048);			// Obtenemos la ruta
	DIR *openDr;				// Direccion donde se va a trabajar
	openDr = opendir(ruta);		// Abrimos la ruta actual
	if(openDr == NULL) {return -1;}

	struct dirent *fichero;		// Elemento del directorio

	// Recorremos el directorio mostrando lo que contiene
	while((fichero=readdir(openDr)) != 0) {
		printf("%s\n",fichero->d_name);
	}
	if(closedir(openDr) == -1) {return -1;}

	return 0;
}



// "mils" con el parametro carpeta
int mils2(char *carpeta) {

	DIR *openDr;				// Direccion donde se va a trabajar
	openDr = opendir(carpeta);	// Abrimos la ruta actual
	if(openDr == NULL) {
		printf("ATENCION: No existe el directorio especificado.\n");
		return -1;
	}
	struct dirent *fichero;		// Elemento del directorio

	// Recorremos el directorio mostrando lo que contiene
	while((fichero=readdir(openDr)) != 0) {
		printf("%s\n",fichero->d_name);
	}
	if(closedir(openDr) == -1) {return -1;}
	return 0;
}



// "mils" con el parametro -l
int mils3() {

	char ruta[2048];			// Aqui guardamos la ruta
	getcwd(ruta,2048);			// Obtenemos la ruta
	DIR *openDr;				// Direccion donde se va a trabajar
	openDr = opendir(ruta);		// Abrimos la ruta actual
	if(openDr == NULL) {return -1;}

	struct tm *fecha;			// Fecha de modificacion del fichero
	struct passwd *usuario;		// Usuario al que pertenece un fichero
	struct group *grupo;		// Grupo al que pertenece un fichero
	struct dirent *fichero;		// Elemento del directorio
	struct stat info;			// Informacion del estado actual de un fichero

	// Recorremos el directorio mostrando lo que contiene detalladamente
	while ((fichero=readdir(openDr)) != 0) {

		// Comprobamos que el estado del contenido del directorio sea correcto
		if ((stat (fichero->d_name,&info)) == -1) {return -1;}
	
		// Imprimimos los permisos del fichero
		printf ((S_IFDIR & info.st_mode) ? "d" : "-");		//Directorio.
		printf ((S_IRUSR & info.st_mode) ? "r" : "-");		//Usuario lectura.
		printf ((S_IWUSR & info.st_mode) ? "w" : "-");		//Usuario escritura.
		printf ((S_IXUSR & info.st_mode) ? "x" : "-");		//Usuario ejecucion.
		printf ((S_IRGRP & info.st_mode) ? "r" : "-");		//Grupo lectura.
		printf ((S_IWGRP & info.st_mode) ? "w" : "-");		//Grupo escritura.
		printf ((S_IXGRP & info.st_mode) ? "x" : "-");		//Grupo ejecucion.
		printf ((S_IROTH & info.st_mode) ? "r" : "-");		//Otros lectura.
		printf ((S_IWOTH & info.st_mode) ? "w" : "-");		//Otros escritura.
		printf ((S_IXOTH & info.st_mode) ? "x   " : "-   ");//Otros ejecucion.

		usuario = getpwuid(info.st_uid);	// Usuario propietario del fichero
		grupo = getgrgid(info.st_gid);		// Grupo propietario del fichero
		printf ("%s  %s   ",usuario->pw_name,grupo->gr_name);// Imprimimos usuario y grupo

		// Vemos que tipo de fichero es y lo imprimimos
		if (S_ISREG (info.st_mode) != 0) {printf ("Fichero sencillo\t");} else { 
		if (S_ISLNK (info.st_mode) != 0) {printf ("Enlace simbolico\t");} else { 
		if (S_ISFIFO(info.st_mode) != 0) {printf ("Tuberia\t");} else {
		if (S_ISDIR (info.st_mode) != 0) {printf ("Directorio\t\t");} else {
		if (S_ISCHR (info.st_mode) != 0) {printf ("Dispositivo de caracter\t");} else {
		if (S_ISBLK (info.st_mode) != 0) {printf ("Dispositivo de bloque\t");} else {
		if (S_ISSOCK(info.st_mode) != 0) {printf ("Socket\t");}}}}}}}

		printf ("%li\t",info.st_size);		// Imprimimos tamano

		fecha = localtime (&info.st_mtime);	// Obtenemos la fecha modificacion y la imprimimos
		printf ("%d/%d/%d  ",fecha->tm_year+1900,fecha->tm_mon+1,fecha->tm_mday);

		printf ("%s\n",fichero->d_name);	// Imprimimos el nombre del fichero

	}

	if(closedir(openDr) == -1) {return -1;}
	return 0;
}



// "mils" con el parametro -l y el parametro carpeta
int mils4(char *carpeta) {

	// Guardamos la ruta actual
	char ruta[2048];			// Aqui guardamos la ruta
	char *ptr;					// Puntero que apunta a la direccion donde se guarda la ruta
	ptr = getcwd(ruta,2048);	// Obtenemos la ruta

	// Vamos a la ruta de la carpeta introducida
	if(chdir(carpeta) == -1) {
		printf("ATENCION: No existe el directorio especificado.\n");
		return -1;
	}

	// Guardamos la ruta de la carpeta en la estamos ahora
	char rut[2048];				// Aqui guardamos la ruta
	getcwd(rut,2048);			// Obtenemos la ruta

	DIR *openDr;				// Direccion donde se va a trabajar
	openDr = opendir(rut);		// Abrimos la ruta actual
	if(openDr == NULL) {return -1;}

	struct dirent *fichero;	// Elemento del directorio
	struct tm *fecha;		// Fecha de modificacion del fichero
	struct passwd *usuario;	// Usuario al que pertenece un fichero
	struct group *grupo;	// Grupo al que pertenece un fichero
	struct stat info;		// Informacion del estado actual de un fichero

	// Recorremos el directorio mostrando lo que contiene detalladamente
	while ((fichero=readdir(openDr)) != 0) {

		// Comprobamos que el estado del contenido del directorio sea correcto
		if ((stat (fichero->d_name,&info)) == -1) {return -1;}
	
		// Imprimimos los permisos del fichero
		printf ((S_IFDIR & info.st_mode) ? "d" : "-");		//Directorio.
		printf ((S_IRUSR & info.st_mode) ? "r" : "-");		//Usuario lectura.
		printf ((S_IWUSR & info.st_mode) ? "w" : "-");		//Usuario escritura.
		printf ((S_IXUSR & info.st_mode) ? "x" : "-");		//Usuario ejecucion.
		printf ((S_IRGRP & info.st_mode) ? "r" : "-");		//Grupo lectura.
		printf ((S_IWGRP & info.st_mode) ? "w" : "-");		//Grupo escritura.
		printf ((S_IXGRP & info.st_mode) ? "x" : "-");		//Grupo ejecucion.
		printf ((S_IROTH & info.st_mode) ? "r" : "-");		//Otros lectura.
		printf ((S_IWOTH & info.st_mode) ? "w" : "-");		//Otros escritura.
		printf ((S_IXOTH & info.st_mode) ? "x   " : "-   ");//Otros ejecucion.

		usuario = getpwuid(info.st_uid);	// Usuario propietario del fichero
		grupo = getgrgid(info.st_gid);		// Grupo propietario del fichero
		printf ("%s  %s   ",usuario->pw_name,grupo->gr_name);	// Imprimimos usuario y grupo

		// Vemos que tipo de fichero es y lo imprimimos
		if (S_ISREG (info.st_mode) != 0) {printf ("Fichero sencillo\t");} else { 
		if (S_ISLNK (info.st_mode) != 0) {printf ("Enlace simbolico\t");} else { 
		if (S_ISFIFO(info.st_mode) != 0) {printf ("Tuberia\t");} else {
		if (S_ISDIR (info.st_mode) != 0) {printf ("Directorio\t\t");} else {
		if (S_ISCHR (info.st_mode) != 0) {printf ("Dispositivo de caracter\t");} else {
		if (S_ISBLK (info.st_mode) != 0) {printf ("Dispositivo de bloque\t");} else {
		if (S_ISSOCK(info.st_mode) != 0) {printf ("Socket\t");}}}}}}}

		printf ("%li\t",info.st_size);		// Imprimimos tamano

		fecha = localtime (&info.st_mtime);	// Obtenemos la fecha modificacion y la imprimimos
		printf ("%d/%d/%d  ",fecha->tm_year+1900,fecha->tm_mon+1,fecha->tm_mday);

		printf ("%s\n",fichero->d_name);	// Imprimimos el nombre del fichero

	}

	if(closedir(openDr) == -1) {return -1;}

	// Volvemos a la ruta inicial
	if(chdir(ptr) == -1) {return -1;}
	return 0;
}



// Crea una nueva carpeta con el nombre que se le pasa por parametro
int mimkdir(const char *carpeta) {
	if (carpeta == NULL) {printf("ATENCION: Debe elegir un nombre para la carpeta.\n");return -1;}
	// Creamos la carpeta. Si surge un error salimos del programa
	if(mkdir(carpeta,0777) == -1) {return -1;}
	return 0;
}



// Elimina una carpeta con el nombre que se le pasa por parametro
int mirmdir(const char *carpeta) {
	if (carpeta == NULL) {printf("ATENCION: Debe elegir un nombre para la carpeta.\n");return -1;}
	// Eliminamos la carpeta. Si surge un error salimos del programa
	if(rmdir(carpeta) == -1) {
		printf("ATENCION: Se ha equivocado el nombre de la carpeta, intenta borrar un archivo o una carpeta que contiene archivos.\n");
		return -1;
	}
	return 0;
}



// Cambia de carpeta.
int micd(const char *carpeta, char *pt) {
	// Si hay un parametro, vamos a dicha carpeta. Ante un error sale del programa
	if(carpeta != NULL) {
		if(chdir(carpeta) == -1) {
			printf("ATENCION: No existe la carpeta especificada.\n");
			return -1;
		}
	// Si el parametro esta vacio, va a la carpeta desde la que se inicio la shell
	} else {
		if(chdir(pt) == -1) {return -1;}
	}
	return 0;
}



// Muestra el contenido del archivo que se le pasa como parametro. Usa proyeccion en memoria
int micat(char *fichero) {

	if (fichero == NULL) {printf("ATENCION: Debe elegir el nombre del fichero.\n");return -1;}

	// Abrimos el archivo pasado como argumento
	int fd = open(fichero,O_RDONLY);
	if(fd == -1) {
		printf("ATENCION: No se existe el archivo especificado.\n");
		return -1;
	}

	struct stat texto;	// Para obtener las caracteristicas del texto

	// Comprobamos que el estado del texto es correcto
	if(fstat(fd,&texto) == -1) {return -1;}

	// Comprobamos si el texto esta vacio (no contiene numeros ni letras ni espacios ni nada)
	if((int)texto.st_size == 0) {
		// El texto esta vacio, por lo que nom impromimos nada
		return 0;		
	}

	// Proyectamos el texto en memoria
	char *proyeccion = mmap(NULL,texto.st_size,PROT_READ,MAP_PRIVATE,fd,(off_t)0);
	if(proyeccion == MAP_FAILED) {return -1;}

	// Imprimimos el texto
	printf("%s\n",proyeccion);

	// Liberamos la memoria proyectada
	if(munmap(proyeccion,texto.st_size) == -1) {return -1;}

	// Cerramos el archivo de texto
	int clos = close(fd);
	if(clos == -1) {return -1;}
	return 0;
}



// Copia (usando read() y write()) el contenido del primer fichero en el segundo
int micp(char *original, char *copia) {

	if (original == NULL) 	{printf("ATENCION: Debe elegir el fichero que quiere copiar.\n");return -1;}
	if (copia == NULL) 		{printf("ATENCION: Debe elegir el fichero en el que quiere copiar.\n");return -1;}

	// Nos aseguramos de que no se intente copiar "desde" y "en" el mismo archivo
	if(strcmp(original,copia) == 0) {
		printf("ATENCION: No puede realizar la copia en el mismo archivo que quiere copiar.\n");
		return -1;		
	}

	// Abrimos en modo lectura el primer archivo pasado como argumento
	int fd1 = open(original,O_RDONLY);
	if(fd1 == -1) {
		printf("ATENCION: No se existe el archivo que quiere copiar.\n");
		return -1;
	}

	// Abrimos en modo escritura el segundo archivo pasado como argumento
	int fd2 = open(copia,O_WRONLY | O_TRUNC);
	if(fd2 == -1) {
		printf("ATENCION: No se existe el archivo en el que quiere copiar.\n");
		return -1;
	}

	struct stat texto;	// Para obtener las caracteristicas del texto

	// Comprobamos que el estado del texto es correcto
	if(fstat(fd1,&texto) == -1) {return -1;}

	int posicion = 0;
	char letra;

	// Recorremos todo el texto copiando cada caracter
	for(posicion = 0; posicion < texto.st_size; posicion++) {

		// Posicionamos el puntero en la posicion correcta del texto 1
		if(lseek(fd1,posicion,SEEK_SET) == -1) {
			printf("ATENCION: No se ha podido reposicionar el puntero de L/E en el texto 1.\n");
			return -1;		
		}

		// Leemos un caracter desde la posicion del puntero en el texto 1
		if(read(fd1,&letra,1) == -1) {
			printf("ATENCION: No se ha podido leer (read) el fichero 1.\n");
			return -1;		
		}

		// Posicionamos el puntero en la posicion correcta del texto 2
		if(lseek(fd2,posicion,SEEK_SET) == -1) {
			printf("ATENCION: No se ha podido reposicionar el puntero de L/E en el texto 2.\n");
			return -1;		
		}

		// Leemos un caracter desde la posicion del puntero en el texto 2
		if(write(fd2,&letra,1) == -1) {
			printf("ATENCION: No se ha podido escribir (write) en el fichero 2.\n");
			return -1;		
		}
	}

	// Cerramos el archivo de texto
	int clos = close(fd1);
	if(clos == -1) {return -1;}
	clos = close(fd2);
	if(clos == -1) {return -1;}
	return 0;
}



// Elimina un fichero con el nombre que se le pasa por parametro
int mirm(const char *fichero) {
	if (fichero == NULL) 	{printf("ATENCION: Debe elegir el fichero que quiere eliminar.\n");return -1;}
	// Eliminamos el fichero. Si surge un error salimos del programa
	if(unlink(fichero) == -1) {
			printf("ATENCION: No existe el fichero especificado, o dicho nombre no se corresponde con un fichero.\n");
		return -1;
	}
	return 0;
}



// Termina la ejecucion de la microshell (finaliza el programa)
int miexit() {
	exit(0);
}

