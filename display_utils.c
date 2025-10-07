#include "display_utils.h"
#include "config.h"
#include "LCD_Display.h"
#include "function_gen.h"
#include "adc.h"
#include "waveforms.h"
void drawWaveformSample(void)
{
    clearScreen();

    const int centerY = ILI9341_TFTHEIGHT / 2;
    const int startX  = 20;
    const int width   = 200;
    const int scaleY  = 50;
    uint16_t color    = COLOR_SINE;
    const char *label = "";

    for (int i = 0; i < TABLE_SIZE; i++) {
        int16_t normalized = 0;
        int16_t x = startX + (i * width) / TABLE_SIZE;   // integer x

        switch (waveform_mode) {
            case WAVE_SINE:
                // convert 0–65535 sine_table to -32768–+32767, scale by 50/32768
                normalized = ((int32_t)sine_table[i] - 32768) * scaleY / 32768;
                color = COLOR_SINE;
                label = "SINE";
                break;

            case WAVE_SAW:
                normalized = ((i * 2 * scaleY) / TABLE_SIZE) - scaleY;
                color = COLOR_SAW;
                label = "SAW";
                break;

            case WAVE_TRI:
                if (i < TABLE_SIZE / 2)
                    normalized = ((i * 4 * scaleY) / TABLE_SIZE) - scaleY;
                else
                    normalized = scaleY - ((i - TABLE_SIZE / 2) * 4 * scaleY) / TABLE_SIZE;
                color = COLOR_TRI;
                label = "TRI";
                break;

            case WAVE_SQUARE:
                normalized = (i < TABLE_SIZE / 2) ? scaleY : -scaleY;
                color = COLOR_SQUARE;
                label = "SQUARE";
                break;

            default:
                normalized = 0;
                break;
        }

        int y = centerY - normalized;
        drawPixel(x, y, color);
    }

    setCharConfig(0xFFFF, 2, 1, 0x0000, 2);
    moveCursor(70, ILI9341_TFTHEIGHT - 30);
    drawString((char*)label, strlen(label));
}
void displayADCValues()
{
    char buf[32];

    // draw background rectangle on right side (to overwrite old text)
    fillRect(180, 40, 60, 80, 0x0000); // x,y,w,h,black

    setCharConfig(0xFFFF, 1, 1, 0x0000, 1); // white text
    moveCursor(185, 50);

    snprintf(buf, sizeof(buf), "Attack:%4u", Attack_ms);
    drawString(buf, strlen(buf));

    moveCursor(185, 70);
    snprintf(buf, sizeof(buf), "A6:%4u", Decay_ms);
    drawString(buf, strlen(buf));

    moveCursor(185, 90);
    snprintf(buf, sizeof(buf), "A8:%4u", Sustain_lv);
    drawString(buf, strlen(buf));
}
