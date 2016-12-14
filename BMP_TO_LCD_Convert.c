/****************************************
 * To convert image from bitmap file	*             
 * Author:Ming-Shu Wu                   *
 * Date:2016                        	*
 ****************************************/
#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include <string.h>

#define VERTICAL_BYTE(BIT_7,BIT_6,BIT_5,BIT_4,BIT_3,BIT_2,BIT_1,BIT_0) \
    (((BIT_7) << 7) | ((BIT_6) << 6) | ((BIT_5) << 5) | ((BIT_4) << 4) | ((BIT_3) << 3) | ((BIT_2) << 2) | ((BIT_1) << 1) | (BIT_0))

#pragma pack(push) 
#pragma pack(1)
struct info_header{ 
	uint32_t	size;/* Info Header size in bytes */ 
	int32_t		width;/* Width and height of image */ 
	int32_t		height;
	uint16_t	planes;/* Number of colour planes */ 
	uint16_t	bits; /* Bits per pixel */ 
	uint32_t	compression; /* Compression type */ 
	uint32_t	imagesize; /* Image size in bytes */ 
	int32_t		xresolution;/* Pixels per meter */ 
	int32_t		yresolution; 
	uint32_t	ncolours; /* Number of colours */ 
	uint32_t	importantcolours; /* Important colours */ 
};

struct file_header{ 
	uint16_t	type; /* type : Magic identifier,default is BM(0x42,0x4D)*/
	uint32_t	size;/* File size in bytes*/ 
	uint32_t	reserved; /*reserved */ 
	uint32_t	offset;/* Offset to image data, bytes */ 
	struct info_header info[0];/* List of properties. */
};
#pragma pack(pop)

int main (void)
{
	char file_name[1024],new_file[1024];
	FILE *read_fp,*write_fp;
	struct file_header *fileP;
	struct info_header *infoP;
	struct data_field *dataP;
	struct pixel_field *pixelP;
	char *dataAllP,*dataCovert_P,*dataTmpe_P,*dataMap_Covert;
	int padding=0,padbyte=0;
	unsigned int imageByte,pad_lock=0;
	int widthPixel,heightPixel,widthByte,heightByte,arraySize;	
	int a;
	unsigned int first_num,right_offset=0,bit_offset=7,heightLevel= 0;
	uint8_t BIT_7,BIT_6,BIT_5,BIT_4,BIT_3,BIT_2,BIT_1,BIT_0;
	
	size_t header_size = sizeof(struct file_header)+sizeof(struct info_header);
	fileP = (struct file_header*)malloc(sizeof(struct file_header)+sizeof(struct info_header));
	infoP = fileP->info;
	//printf("%d\n,%d\n",sizeof(struct file_header),sizeof(struct info_header));
	printf("Please loading picture: ");
	gets(file_name);
	
	if((read_fp = fopen(file_name,"rb"))==NULL)
	{
		printf("Cannot open read data file\n");
		exit(1);
	}
	sprintf(new_file,"%s.bin",file_name);
	if((write_fp = fopen(new_file,"wb"))==NULL)
	{
		printf("Cannot open write file\n");
		exit(1);
	}
		
	fread(fileP,1,header_size,read_fp);
	
	imageByte = infoP->imagesize;
	widthPixel = infoP->width;
	heightPixel = infoP->height;
	if(heightPixel%8!=0)
		arraySize = widthPixel * heightPixel/8+ widthPixel*8;
	else
		arraySize = widthPixel *heightPixel/8;
		
	if((widthPixel *heightPixel)%8 != 0)
		arraySize = arraySize+1;		
	//heightByte = heightPixel/8;
	if(widthPixel%8 != 0){
		widthByte = widthPixel/8 + 1;
		padding = widthPixel%8;
	}else{
		widthByte = widthPixel/8;
		padding=0;
	}
	if(widthByte%4 != 0){
		padbyte = 4- (widthByte % 4);
		widthByte =widthByte+padbyte;	
	}
	//printf("widthByte:%d,padbyte:%d,padding:%d\n\n",widthByte,padbyte,padding);		
	printf( "File size is %d byte, Imagesize size id %d byte\n",fileP->size,imageByte);
	printf( "Bitmap Data Offset is %d byte\n",fileP->offset);
	printf( "File Width is %d pixel, Height is %d pixel\n",widthPixel,heightPixel);
	
	dataCovert_P = dataAllP = (char*)malloc(sizeof(char)*imageByte);
	fseek(read_fp,fileP->offset,SEEK_SET);
	fread(dataAllP,1,imageByte,read_fp);
	free(fileP);
	
	dataMap_Covert = (char*)malloc(sizeof(char)*imageByte);
	memset(dataMap_Covert,0,sizeof(char)*imageByte);
	for(a=0;a<arraySize;a++){
		first_num = imageByte- widthByte-widthByte*8*heightLevel;/*new array of first point and big bit*/
		BIT_0= ~(dataCovert_P[(first_num+right_offset)-widthByte*0]>>bit_offset+(7-padding+1)*pad_lock)&0x01;/*if scan right of boundary that eable pad_lcok,and padding bit shift */
		BIT_1= ~(dataCovert_P[(first_num+right_offset)-widthByte*1]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_2= ~(dataCovert_P[(first_num+right_offset)-widthByte*2]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_3= ~(dataCovert_P[(first_num+right_offset)-widthByte*3]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_4= ~(dataCovert_P[(first_num+right_offset)-widthByte*4]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_5= ~(dataCovert_P[(first_num+right_offset)-widthByte*5]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_6= ~(dataCovert_P[(first_num+right_offset)-widthByte*6]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
		BIT_7= ~(dataCovert_P[(first_num+right_offset)-widthByte*7]>>bit_offset+(7-padding+1)*pad_lock)&0x01;
	
		dataMap_Covert[a] = VERTICAL_BYTE(BIT_7,BIT_6,BIT_5,BIT_4,BIT_3,BIT_2,BIT_1,BIT_0);
		//printf("dataMap_Covert[%d]:%x\n",a,dataMap_Covert[a]);
		if(bit_offset==0){
			bit_offset=7;
			if(padding){/*if scan before boundary*/
					if(right_offset==(widthByte-1-padbyte-1)){
						bit_offset = padding-1;
						pad_lock=1;
					}//if				
			}//if			
			if(right_offset==(widthByte-1-padbyte)){
				right_offset = 0;
				pad_lock=0;
				heightLevel=heightLevel+1;
			}else{
					right_offset++;
			}//else						
		}else{
			bit_offset--;				
		}//else								
	}//for
	fwrite(dataMap_Covert,1,arraySize,write_fp);
	free(dataCovert_P);
	free(dataMap_Covert);
	fclose(read_fp);
	fclose(write_fp);
	system("pause");
	
	return 0;
}
