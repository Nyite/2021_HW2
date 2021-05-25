#include "mem.h"

static void	UnlockFlash(void)
{
	//Write KEY1 = 0x45670123
	//Write KEY2 = 0xCDEF89AB
	if ((FLASH->CR & FLASH_CR_LOCK) != 0)
	{
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;
	}
}

static void	LockFlash(void)
{
	while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	FLASH->CR = FLASH_CR_LOCK;
}

void EraseSector(void)
{
	UnlockFlash();
	while(FLASH->SR & FLASH_SR_BSY);

	FLASH->CR |= FLASH_CR_SER;
	FLASH->CR |= FLASH_CR_SNB_0 | FLASH_CR_SNB_1;
	FLASH->CR |= FLASH_CR_STRT;

	while(FLASH->SR & FLASH_SR_BSY);
	FLASH->CR &= ~FLASH_CR_SER;
	LockFlash();
}

//Sector 3 0x0800C000 - 0x0800FFFF 16 Kbytes

void WriteToFlash(uint8_t *data, int size_data)
{
	EraseSector();
	UnlockFlash();

	FLASH->CR |= FLASH_CR_PG;

	for (int  i = 0; i < size_data; i++)
	{
		*(__IO uint8_t*)(0x0800C000 + i) = data[i];
		while ((FLASH->SR & FLASH_SR_BSY) != 0);
		if((FLASH->SR & FLASH_SR_EOP) == 0)
			FLASH->SR |= FLASH_SR_EOP;
	}

	FLASH->CR &= ~FLASH_CR_PG;

	LockFlash();
}

void HWReadFromFlash(uint8_t *data, uint32_t mem_address)
{
	*data = *(__IO uint8_t*)(mem_address);
}
