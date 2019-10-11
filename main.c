#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define TLMY "TITAraw_tlmy.bin"

// FUNCIÓN PARA INTERCAMBIAR VALORES ENTRE DOS PUNTEROS
void swap(int8_t* a, int8_t* b){
	int8_t aux = *a;
	*a = *b;
	*b = aux;
}

// FUNCIÓN PARA INVERTIR DE BIG ENDIAN A LITTLE ENDIAN
void reverse(int8_t* pi, int8_t* pf){
	while(pi<pf){
		swap(pi,pf);
		pi++;
		pf--;
	}
}

// FUNCIÓN DE COMPARACIÓN DE SEGUNDOS
int cmp_seconds(const void* a,const void* b){
	uint8_t pkt1[113];
	uint8_t pkt2[113];
	uint32_t *sec1;
	uint32_t *sec2;
	memcpy(pkt1,a,sizeof(pkt1));
	memcpy(pkt2,b,sizeof(pkt2));
	reverse(&pkt1[9],&pkt1[9+3]);
	reverse(&pkt2[9],&pkt2[9+3]);
	sec1 = &pkt1[9];
	sec2 = &pkt2[9];
	return (*sec1 - *sec2);
}

int main()
{
	FILE* pf;
	FILE* pfout;
	void* vec;
	int cant=0;
	uint8_t pkt[113];
	int16_t *sunX;
	int16_t *sunY;
	int16_t *sunZ;

	// SE ABRE ACHIVO DE TELEMETRÍA 
	pf = fopen(TLMY,"rb");

	if(!pf)
	{
		puts("Error al abrir el archivo");
		return 0;
	}
	
	// OBTENGO CANTIDAD DE PAQUETES EN EL ARCHIVO
	fread(pkt,sizeof(pkt),1,pf);
	cant++;
	while(!feof(pf))
	{
		fread(pkt,sizeof(pkt),1,pf);
		cant++;
	}

	vec = malloc(cant*sizeof(pkt));		// SE RESERVA ESPACIO PARA VECTOR DE PAQUETES
	void* ptr_vec = vec;


	// LEO ARCHIVO Y GUARDO LOS PAQUETES EN VEC
	fseek(pf,0L,SEEK_SET);
	fread(pkt,sizeof(pkt),1,pf);
	memcpy(ptr_vec,pkt,sizeof(pkt));
	ptr_vec = ptr_vec + sizeof(pkt);
	while(!feof(pf))
	{	
		fread(pkt,sizeof(pkt),1,pf);
		memcpy(ptr_vec,pkt,sizeof(pkt));
		ptr_vec = ptr_vec + sizeof(pkt);
	}
	
	// ORDENO PAQUETES SEGÚN EL TIEMPO
	qsort(vec,cant,sizeof(pkt),cmp_seconds);

	
	// ABRO ARCHIVO DE SALIDA
	pfout = fopen("SUN_SENSORS.csv","wb");
	if(!pfout)
	{
		puts("Error al abrir el archivo");
		return 0;
	}

	// EXTRAIGO INFO DE SENSORES SOLARES Y GENERO CSV
	ptr_vec = vec;
	fprintf(pfout,"Solar Sensor X,Solar Sensor Y,Solar Sensor Z\n");
	for(int i=0;i<cant;i++)
	{
		memcpy(pkt,ptr_vec,sizeof(pkt));
		reverse(&pkt[64],&pkt[64+1]);
		reverse(&pkt[66],&pkt[66+1]);
		reverse(&pkt[68],&pkt[68+1]);
		sunX = &pkt[64];
		sunY = &pkt[66];
		sunZ = &pkt[68];
		fprintf(pfout,"%f,%f,%f\n",*sunX/16384.0,*sunY/16384.0,*sunZ/16384.0);
		ptr_vec = ptr_vec + sizeof(pkt);
	}
	
	free(vec);
	fclose(pf);
	fclose(pfout);
	return 0;
}


