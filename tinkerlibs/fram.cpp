#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fram.h"

namespace Tinker_Fram {

	BP *bp;
		
	void error(const char * msg)
	{
		fprintf(stderr, "Error: %s\n", msg);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	int fram_open(char * device)
	{
		unsigned char version;

		//bp= bp_open(argv[1]);
		printf("El device a ser abierto es %s\n", device);
		bp= bp_open(device);
		if (bp == NULL) {
			error("No se puede conectar a Bus Pirate.");
			return BP_FAILURE;
		}

		printf("Version Firmware : %d.%d\n",
		bp_firmware_version_high(bp),
		bp_firmware_version_low(bp));

		// switch to binary mode
		if (bp_bin_init(bp, &version) != BP_SUCCESS) {
			error("no puedo cambiar a modo binario");
			return BP_FAILURE;
		}

		if (bp_bin_mode_spi(bp, &version) != BP_SUCCESS) {
			error("no puedo activar modo spi");
			return BP_FAILURE;
			//return -1;
		}

		if (bp_bin_spi_set_speed(bp, BP_BIN_SPI_SPEED_2M) != BP_SUCCESS) {
			error("no puedo activar velocidad");
			return BP_FAILURE;
			//return -1;
		}

		if (bp_bin_spi_set_config(bp, BP_BIN_SPI_CLK_IDLE_LOW |
                                BP_BIN_SPI_CLK_EDGE_HIGH |
                                BP_BIN_SPI_SMP_MIDDLE |
                                BP_BIN_SPI_LV_3V3) != BP_SUCCESS) {
			error("no puedo configurar parametros");
			return BP_FAILURE;
			//return -1;
		}
		usleep(1000);

		if (bp_bin_spi_set_periph(bp, (BP_BIN_SPI_PERIPH_POWER | BP_BIN_SPI_PERIPH_CS)) != BP_SUCCESS) {
			error("no puedo configurar perifericos");
			return BP_FAILURE;
			//return -1;
		}
		usleep(1000000);
		return BP_SUCCESS;	
	}


	
	int fram_close(void)
	{
		// quit binary mode

		if (bp_reset(bp) != BP_SUCCESS) {
			error("no puedo reset bus pirate a modo terminal");
			return BP_FAILURE;
		}
		printf("Cerrando conexion.\n");
		bp_close(bp);	
		return BP_SUCCESS;
	}
	
	int fram_is_present(void)
	{
		uint8_t status;
		
		//Leo el registro status y espero que los bits 0, 4, 5 y 6 esten en cero.
		//Eso asegura que al menos la lectura I2C es coherente
		fram_read_status(&status);
		if( (status & 0x71) == 0 ) {
			return BP_SUCCESS;
		} else {
			return BP_FAILURE;
		}
	
	}

	int fram_read_status(uint8_t *value)
	{
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_READ_STATUS, 0xFF};
		if (bp_bin_spi_bulk(bp, data, 2) != BP_SUCCESS) {
			error("Falla read status spi bulk");  
			return BP_FAILURE;
		}
		*value = data[1];
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_write_status(uint8_t value) 
	{
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_WRITE_STATUS, value};
	
		if (bp_bin_spi_bulk(bp, data, 2) != BP_SUCCESS) {
			error("Falla write status spi bulk");  
			return BP_FAILURE;
		}
	
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_write_enable(void) 
	{
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_WRITE_ENABLE};
	
		if (bp_bin_spi_bulk(bp, data, 1) != BP_SUCCESS) {
			error("Falla write enable spi bulk");  
			return BP_FAILURE;
		}
	
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_write_disable(void) 
	{
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_WRITE_DISABLE};
	
		if (bp_bin_spi_bulk(bp, data, 1) != BP_SUCCESS) {
			error("Falla write enable spi bulk");  
			return BP_FAILURE;
		}
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_read(unsigned offset, void * buffer, unsigned size)
	{
		uint16_t left;
		uint8_t *ptr;
		uint16_t num_blocks, diff;
		
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_READ_MEMORY, 0xFF, 0xFF};
		data[1] = (offset >> 8) & 0xFF;data[2] = (offset ) & 0xFF;
  
		if (bp_bin_spi_bulk(bp, data, 3) != BP_SUCCESS) {
			error("Falla read status");  
			return BP_FAILURE;
		}
  
		left = size;
		num_blocks = left / 16;
		diff = left - num_blocks * 16;
		//ptr = buffer;
		ptr = (uint8_t *)buffer;

		//printf("Left:%u Num_Blocks:%u Diff:%u\r\n", left, num_blocks, diff);
	
		if( left <= 16 ) {
			if (bp_bin_spi_bulk(bp, ptr, left) != BP_SUCCESS) {
				error("Falla read status");  
				return BP_FAILURE;
			}
		}
		else {
			while(num_blocks) {
				if (bp_bin_spi_bulk(bp, ptr, 16) != BP_SUCCESS) {
					error("Falla read status");  
					return BP_FAILURE;
				}
				ptr += 16;//ptr++;
				num_blocks--;//left--
			}
			
			if(diff != 0) {
				if (bp_bin_spi_bulk(bp, ptr, diff) != BP_SUCCESS) {
					error("Falla read status");  
					return BP_FAILURE;
				} 
			}
		}

		/*
		while(left) {
			if (bp_bin_spi_bulk(bp, ptr, 1) != BP_SUCCESS) {
				error("Falla read status");  
				return BP_FAILURE;
			}
			ptr++;
			left--;
		}
		*/
		//printf("\r\n");
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_write(unsigned offset, void const * buffer, unsigned size)
	{
		uint16_t left;
		uint8_t *ptr;
		uint16_t num_blocks, diff;
 
		if (bp_bin_spi_cs(bp, 0) != BP_SUCCESS) {
			error("Falla cs 0");  
			return BP_FAILURE;
		}

		unsigned char data[]= { _FRAM_WRITE_MEMORY, 0xFF, 0xFF};
		data[1] = (offset >> 8) & 0xFF;data[2] = (offset ) & 0xFF;
  
		if (bp_bin_spi_bulk(bp, data, 3) != BP_SUCCESS) {
			error("Falla read status");  
			return BP_FAILURE;
		}
  
		left = size;
		num_blocks = left / 16;
		diff = left - num_blocks * 16;
		//ptr = buffer;
		ptr = (uint8_t *)buffer;
		//printf("Left:%u Num_Blocks:%u Diff:%u\r\n", left, num_blocks, diff);
	
		if( left <= 16 ) {
			if (bp_bin_spi_bulk(bp, ptr, left) != BP_SUCCESS) {
				error("Falla read status");  
				return BP_FAILURE;
			}
		}
		else {
			while(num_blocks) {
				if (bp_bin_spi_bulk(bp, ptr, 16) != BP_SUCCESS) {
					error("Falla read status");  
					return BP_FAILURE;
				}
				ptr += 16;//ptr++;
				num_blocks--;//left--
			}
			
			if(diff != 0) {
				if (bp_bin_spi_bulk(bp, ptr, diff) != BP_SUCCESS) {
					error("Falla read status");  
					return BP_FAILURE;
				} 
			}
		}

		/*
		while(left) {
			if (bp_bin_spi_bulk(bp, ptr, 1) != BP_SUCCESS) {
				error("Falla read status");  
				return BP_FAILURE;
			}
			ptr++;
			left--;
		}
		*/
		//printf("\r\n");
  
		if (bp_bin_spi_cs(bp, 1) != BP_SUCCESS) {
			error("Falla cs 1");  
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}

	int fram_format(void) {
		unsigned char status_w;
		unsigned char status_r = 0;
		uint8_t memory[32768];
	  
		if(fram_write_enable() != BP_SUCCESS) {
			error("Falla write enable");
			return BP_FAILURE;
		}
		status_w = ( 0x80 | ( ((unsigned char ) _FRAM_BLOCK_NONE) << 2 ) );
	
		if(fram_write_status(status_w) != BP_SUCCESS) {
			error("Falla write status");
			return BP_FAILURE;
		}
	
		if(fram_write_disable() != BP_SUCCESS) {
			error("Falla write disable");
			return BP_FAILURE;
		}
	
		if(fram_read_status(&status_w) != BP_SUCCESS) {
			error("Falla read status");
			return BP_FAILURE;
		}
	
		printf("Status W:%X R:%X\r\n", status_w, status_r);
	
		if(fram_write_enable() != BP_SUCCESS) {
			error("Falla write enable");
			return BP_FAILURE;
		}
	
		memset(memory, 0x00, sizeof(memory));
	
		if(fram_write(0x00, &memory[0], sizeof(memory)) != BP_SUCCESS) {
			error("Falla fram write");
			return BP_FAILURE;
		}

		if(fram_write_disable() != BP_SUCCESS) {
			error("Falla write disable");
			return BP_FAILURE;
		}
		return BP_SUCCESS;
	}
	
	int fram_setBlockProtection( BlockProtection b )
	{
		unsigned char status_r;
		if(fram_write_enable() != BP_SUCCESS) {
			error("Falla write enable");
			return BP_FAILURE;
		}

		unsigned char status = ( 0x80 | ( ((unsigned char ) b) << 2 ) );
		
		if(fram_write_status(status) != BP_SUCCESS) {
			error("Falla write status");
			return BP_FAILURE;
		}

		if(fram_write_disable() != BP_SUCCESS) {
			error("Falla write disable");
			return BP_FAILURE;
		}

		if(fram_read_status(&status_r) != BP_SUCCESS) {
			error("Falla read status");
			return BP_FAILURE;
		}
		
		if ( ( status_r & (0x80|(3<<2)) ) != status )
		{
			// printf("return 0\n");
			//return 0;
			return BP_FAILURE;
		}
		else
		{
			//  printf("return 1\n");
			//return 1;
			return BP_SUCCESS;
		}
    
	}
}
