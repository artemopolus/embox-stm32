/**
 * @file
 * @brief
 *
 * @date 16.07.15
 * @author
 */

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>
#include <util/log.h>
#include <kernel/irq.h>

#if defined STM32F746xx
#include "stm32746g_discovery_lcd.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery_lcd.h"
/* This macro is not defined for STM32F769I, but it still
 * works just fine with the same numeric constatnt */
void     BSP_LCD_LayerRgb565Init(uint16_t LayerIndex, uint32_t FB_Address);
#else
#error Unsupported platform
#endif

/* Initialize layer 0 as active for all F7 boards. */
#undef LTDC_ACTIVE_LAYER
#define LTDC_ACTIVE_LAYER 0

#include <embox/unit.h>
EMBOX_UNIT_INIT(stm32f7_lcd_init);

#define LTDC_IRQ      OPTION_GET(NUMBER,ltdc_irq)
static_assert(LTDC_IRQ == LTDC_IRQn);

#define USE_FB_SECTION OPTION_GET(BOOLEAN,use_fb_section)

#if USE_FB_SECTION
#define STM32_FB_SECTION_START OPTION_GET(STRING,fb_section_start)
extern char STM32_FB_SECTION_START;
#else
#define STM32_FB_START  OPTION_GET(NUMBER, fb_base)
#endif

#define STM32_LCD_HEIGHT  OPTION_GET(NUMBER, height)
#define STM32_LCD_WIDTH   OPTION_GET(NUMBER, width)
#define STM32_LCD_BPP     OPTION_GET(NUMBER, bpp)

static int stm32f7_lcd_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int stm32f7_lcd_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(*var));

	var->xres_virtual = var->xres = BSP_LCD_GetXSize();
	var->yres_virtual = var->yres = BSP_LCD_GetYSize();
	var->bits_per_pixel = STM32_LCD_BPP;

	switch (STM32_LCD_BPP) {
	case 16:
		var->fmt = BGR565;
		break;
	case 32:
		var->fmt = BGRA8888;
		break;
	default:
		log_error("stm32f7_lcd_get_var unknown BPP = %d\n", STM32_LCD_BPP);
		return -1;
	}

	return 0;
}

static void stm32f7_lcd_fillrect(struct fb_info *info,
		const struct fb_fillrect *rect) {
	BSP_LCD_SetTextColor(rect->color | 0xff000000);
	BSP_LCD_FillRect(rect->dx, rect->dy, rect->width, rect->height);
}

static uint32_t stm32f7_get_image_color(const struct fb_image *image, int num) {
	switch (image->depth) {
	case 1:
		if (image->data[num / 8] & (1 << (8 - num % 8))) {
			return image->fg_color;
		} else {
			return image->bg_color;
		}
	case 16:
		return ((uint16_t *) image->data)[num];
	case 32:
		return ((uint32_t *) image->data)[num];
	default:
		log_error("Unsupported color depth: %d\n", image->depth);
		return image->bg_color;
	}
}

static void stm32f7_lcd_imageblit(struct fb_info *info,
		const struct fb_image *image) {
	int dy = image->dy, dx = image->dx;
	int height = image->height, width = image->width;
	int n = 0;

	for (int j = dy; j < dy + height; j++) {
		for (int i = dx; i < dx + width; i++) {
			BSP_LCD_DrawPixel(i, j, stm32f7_get_image_color(image, n));
		}
	}
}

static irq_return_t ltdc_irq_handler(unsigned int irq_num, void *dev_id) {
#if defined STM32F746xx
extern LTDC_HandleTypeDef hLtdcHandler;
#define hltdc_handler hLtdcHandler
#elif defined STM32F769xx
extern LTDC_HandleTypeDef  hltdc_discovery;
#define hltdc_handler hltdc_discovery
#else
#error Unsupported platform
#endif

	HAL_LTDC_IRQHandler(&hltdc_handler);

	return IRQ_HANDLED;
}

STATIC_IRQ_ATTACH(LTDC_IRQ, ltdc_irq_handler, NULL);

static struct fb_ops stm32f7_lcd_ops = {
	.fb_set_var   = stm32f7_lcd_set_var,
	.fb_get_var   = stm32f7_lcd_get_var,
	.fb_fillrect  = stm32f7_lcd_fillrect,
	.fb_imageblit = stm32f7_lcd_imageblit,
};

static int stm32f7_lcd_init(void) {
	char *mmap_base;

	if (BSP_LCD_Init() != LCD_OK) {
		log_error("Failed to init LCD!");
		return -1;
	}

	if (0 > irq_attach(LTDC_IRQ, ltdc_irq_handler, 0, NULL, "LTDC")) {
		log_error("irq_attach failed");
		return -1;
	}

#if USE_FB_SECTION
	mmap_base = (char *) &STM32_FB_SECTION_START;
#else
	mmap_base = (char *) STM32_FB_START;
#endif

	BSP_LCD_SetXSize(STM32_LCD_WIDTH);
	BSP_LCD_SetYSize(STM32_LCD_HEIGHT);

	switch (STM32_LCD_BPP) {
	case 16:
		BSP_LCD_LayerRgb565Init(LTDC_ACTIVE_LAYER, (unsigned int) mmap_base);
		break;
	case 32:
		BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, (unsigned int) mmap_base);
		break;
	default:
		log_error("Failed to init LCD Layer!");
		return -1;
	}

	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

#ifdef STM32F769xx
	/* STM32746 doesn't support brightness change */
	BSP_LCD_SetBrightness(100);
#endif

	BSP_LCD_Clear(LCD_COLOR_BLACK);

	fb_create(&stm32f7_lcd_ops,
			mmap_base,
			STM32_LCD_WIDTH *
			STM32_LCD_HEIGHT /
			8 * STM32_LCD_BPP);

	return 0;
}
