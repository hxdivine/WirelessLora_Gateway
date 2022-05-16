
#include "gd32e10x.h"
#include "spi.h"
#include "systick.h"

/*
	SPI1初始化
*/
spi_parameter_struct spi_init_struct;
void Spi1Init(void)
{
	rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_5 | GPIO_PIN_7);  //推挽复用
	gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_10MHZ,GPIO_PIN_6); //上拉输入
	gpio_init(GPIOA,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4);//推挽
	
	spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode = SPI_MASTER;
	spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;//时钟极性 相位
	spi_init_struct.nss = SPI_NSS_SOFT;
	spi_init_struct.prescale = SPI_PSC_2;
	spi_init_struct.endian = SPI_ENDIAN_MSB;
	spi_init(SPI0, &spi_init_struct);
	
	/* set crc polynomial */
	spi_crc_polynomial_set(SPI0,7);
	/* enable SPI0 */
	spi_enable(SPI0);
	Spi1ReadWriteByte(0xff);
}

/*
	SPI1读写寄存器
*/
u8 Spi1ReadWriteByte(u8 data)
{
	u8 ret = 0;
	while(spi_i2s_flag_get(SPI0,SPI_FLAG_TBE) == RESET)
	{
		ret++;
		if(ret > 200)
		{
			return 0;
		}
	}
	spi_i2s_data_transmit(SPI0,data);
	ret = 0;
	while(spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE) == RESET)
	{
		ret++;
		if(ret > 200)
		{
			return 0;
		}
	}
	
	return spi_i2s_data_receive(SPI0);
}

/*
	使用在 ENC28J60模块 以及 LoRa模块
*/
void SpiAsNet(void)
{
	spi_disable(SPI0);
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init(SPI0,&spi_init_struct);
	spi_enable(SPI0);
}

/*
	使用在 Flash
*/
void SpiAsFlash(void)
{
	spi_disable(SPI0);
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init(SPI0,&spi_init_struct);
	spi_enable(SPI0);
	delayMs(1);
}

/*
	关闭SPI
*/
void SpiClose(void)
{
	spi_disable(SPI0);
}
