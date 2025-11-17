#include "mux_cd4067.h"
#include "gpio.h"

/*
 * 模块说明：
 * 双片 CD4067 控制定时扫描模块，配合 TIM1 5ms Tick 实现行列/点位/全扫等模式。
 * 所有 GPIO 操作在 tick 中完成，避免主循环阻塞；可通过 mux_set_period_ms()
 * 设置全扫周期，通过 mux_enable() 控制 EN 引脚。
 */

static mux_scan_mode_t s_mode = MUX_SCAN_FULL;
static uint8_t s_row = 0U, s_col = 0U;
static uint8_t s_en = 0U;
static uint16_t s_period_ms = 200U;
static uint16_t s_elapsed_ms = 0U;
static uint8_t s_fullscan_active = 0U;

static void write_row_pins(uint8_t value)
{
	HAL_GPIO_WritePin(SY0_GPIO_Port, SY0_Pin, (value & 0x01U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY1_GPIO_Port, SY1_Pin, (value & 0x02U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY2_GPIO_Port, SY2_Pin, (value & 0x04U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY3_GPIO_Port, SY3_Pin, (value & 0x08U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void write_col_pins(uint8_t value)
{
	HAL_GPIO_WritePin(SX0_GPIO_Port, SX0_Pin, (value & 0x01U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SX1_GPIO_Port, SX1_Pin, (value & 0x02U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SX2_GPIO_Port, SX2_Pin, (value & 0x04U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SX3_GPIO_Port, SX3_Pin, (value & 0x08U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void apply_selection(uint8_t row, uint8_t col)
{
	write_row_pins(row & 0x0FU);
	write_col_pins(col & 0x0FU);
}

void mux_init(void)
{
	s_mode = MUX_SCAN_FULL;
	s_row = s_col = 0U;
	s_en = 0U;
	s_period_ms = 200U;
	s_elapsed_ms = 0U;
	s_fullscan_active = 0U;
	/* Disable both EN lines (active low) */
	HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, GPIO_PIN_SET);
}

void mux_set_mode(mux_scan_mode_t mode, uint8_t row, uint8_t col)
{
	s_mode = mode;
	s_row = row;
	s_col = col;
	if (mode != MUX_SCAN_FULL)
	{
		s_fullscan_active = 0U;
	}
	else
	{
		/* trigger a cycle immediately */
		s_elapsed_ms = s_period_ms;
	}
}

void mux_enable(uint8_t en)
{
	s_en = en ? 1U : 0U;
	GPIO_PinState level = s_en ? GPIO_PIN_RESET : GPIO_PIN_SET; /* active low enable */
	HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, level);
	HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, level);
}

void mux_set_period_ms(uint16_t period_ms)
{
	s_period_ms = (period_ms == 0U) ? 200U : period_ms;
	if (s_period_ms < 5U)
	{
		s_period_ms = 5U;
	}
}

void mux_tick_5ms(void)
{
	if (!s_en)
	{
		return;
	}

	if (s_elapsed_ms <= (uint16_t)(0xFFFFU - 5U))
	{
		s_elapsed_ms = (uint16_t)(s_elapsed_ms + 5U);
	}

	switch (s_mode)
	{
	case MUX_SCAN_ROW:
		apply_selection(s_row, 0U);
		break;
	case MUX_SCAN_COL:
		apply_selection(0U, s_col);
		break;
	case MUX_SCAN_POINT:
		apply_selection(s_row, s_col);
		break;
	case MUX_SCAN_FULL:
	default:
		if (!s_fullscan_active)
		{
			if (s_elapsed_ms >= s_period_ms)
			{
				s_elapsed_ms = 0U;
				s_fullscan_active = 1U;
				s_row = 0U;
				s_col = 0U;
			}
		}
		if (s_fullscan_active)
		{
			apply_selection(s_row, s_col);
			s_col++;
			if (s_col >= 16U)
			{
				s_col = 0U;
				s_row++;
				if (s_row >= 16U)
				{
					s_fullscan_active = 0U;
					s_row = 0U;
				}
			}
		}
		break;
	}
}

