#include <lvgl.h>
#include <DHT20.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include "ui.h"
#include "snake_logic.h"

#define TFT_BL 2


class LGFX : public lgfx::LGFX_Device
{
public:

  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_RGB   _panel_instance;

  LGFX(void)
  {


    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      
      cfg.pin_d0  = GPIO_NUM_8; // B0
      cfg.pin_d1  = GPIO_NUM_3;  // B1
      cfg.pin_d2  = GPIO_NUM_46;  // B2
      cfg.pin_d3  = GPIO_NUM_9;  // B3
      cfg.pin_d4  = GPIO_NUM_1;  // B4
      
      cfg.pin_d5  = GPIO_NUM_5;  // G0
      cfg.pin_d6  = GPIO_NUM_6; // G1
      cfg.pin_d7  = GPIO_NUM_7;  // G2
      cfg.pin_d8  = GPIO_NUM_15;  // G3
      cfg.pin_d9  = GPIO_NUM_16; // G4
      cfg.pin_d10 = GPIO_NUM_4;  // G5
      
      cfg.pin_d11 = GPIO_NUM_45; // R0
      cfg.pin_d12 = GPIO_NUM_48; // R1
      cfg.pin_d13 = GPIO_NUM_47; // R2
      cfg.pin_d14 = GPIO_NUM_21; // R3
      cfg.pin_d15 = GPIO_NUM_14; // R4

      cfg.pin_henable = GPIO_NUM_40;
      cfg.pin_vsync   = GPIO_NUM_41;
      cfg.pin_hsync   = GPIO_NUM_39;
      cfg.pin_pclk    = GPIO_NUM_0;
      cfg.freq_write  = 15000000;

      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch  = 43;
      
      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 4;
      cfg.vsync_back_porch  = 12;

      cfg.pclk_active_neg   = 1;
      cfg.de_idle_high      = 0;
      cfg.pclk_idle_high    = 0;

      _bus_instance.config(cfg);
    }
            {
      auto cfg = _panel_instance.config();
      cfg.memory_width  = 800;
      cfg.memory_height = 480;
      cfg.panel_width  = 800;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);
    setPanel(&_panel_instance);

  }
};


LGFX lcd;

//UI

int led;
DHT20 dht20;
SPIClass& spi = SPI;


/*******************************************************************************
   Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  

  //lcd.fillScreen(TFT_WHITE);
#if (LV_COLOR_16_SWAP != 0)
  lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
#else
  lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
#endif

  lv_disp_flush_ready(disp);

}



lv_obj_t* startScreen;
lv_obj_t* startBtn;
int gridSize = 40;

void drawStartScreen() {
    // Create a new fullscreen container
    startScreen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(startScreen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(startScreen, lv_color_hex(0x0000FF), LV_PART_MAIN);  // blue
    lv_obj_set_style_bg_opa(startScreen, LV_OPA_COVER, LV_PART_MAIN);

    // Title label
    lv_obj_t* title = lv_label_create(startScreen);
    lv_label_set_text(title, "SPEECH SNAKE");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);   // white text
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);  // center, offset down

    // Start button
    startBtn = lv_btn_create(startScreen);
    lv_obj_set_size(startBtn, 150, 50);
    lv_obj_align(startBtn, LV_ALIGN_CENTER, 0, 40);


    // Button text
    lv_obj_t* btnLabel = lv_label_create(startBtn);
    lv_label_set_text(btnLabel, "START GAME");
    lv_obj_center(btnLabel);
}

void drawBoard(){
  lcd.fillScreen(TFT_GREENYELLOW);
  for(int i = 1; i <= 24; i++){
    for(int j = 1; j <= 12; j++){
      if (i%2 == 0){
        if(j%2 != 0){
          lcd.fillRect(gridSize*i - gridSize, gridSize*j-gridSize, gridSize, gridSize, TFT_GREEN);
        }
      }
      else{
        if(j%2 == 0){
          lcd.fillRect(gridSize*i - gridSize, gridSize*j-gridSize, gridSize, gridSize, TFT_GREEN);
        }
      }
    }
  }
}

void drawSnake(Point snakeLoc[]) {
  for (int i = 0; i < snakeLength; i++) {
    int x = snakeLoc[i].x;
    int y = snakeLoc[i].y;

    int px = x * gridSize;
    int py = y * gridSize;

    lcd.fillRect(px, py, gridSize, gridSize, TFT_SKYBLUE);
  }
}

void drawFruit(Point fruitLoc[]) {
    int gx = fruitLoc[0].x;   // grid X
    int gy = fruitLoc[0].y;   // grid Y

    int px = gx * gridSize;
    int py = gy * gridSize;

    int cx = px + gridSize / 2;
    int cy = py + gridSize / 2;

    int radius = gridSize * 0.3;
    lcd.fillCircle(cx, cy, radius, TFT_RED);

    int stemLen = gridSize * 0.19;
    lcd.drawLine(cx, cy - radius, cx, cy - radius - stemLen, TFT_BROWN);
}

void setup(){
  
  Serial.begin(115200);
  // Serial.println("LVGL Widgets Demo");
  Wire.begin(19, 20);
  dht20.begin();
  //IO口引脚
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  
  // Init Display
  lcd.begin();
  lcd.fillScreen(TFT_BLACK);
  // lcd.setTextSize(2);
  delay(200);

  lv_init();

  delay(100);
  touch_init();

  screenWidth = lcd.width();
  screenHeight = lcd.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 10);
  //  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
  initGame();


  updateSnake();
  drawBoard();
  drawSnake(snake);
  drawFruit(&fruit);

  Serial.println( "Setup done" );

}

unsigned long prev = 0;
void loop()
{
  if (!snakeAlive){
    lv_timer_handler();
    lv_obj_clean(lv_scr_act());
    drawStartScreen();
  }
  else{
    if (millis() - prev > 1000) {
      prev = millis();
      updateSnake();
      drawBoard();
      drawSnake(snake);
      drawFruit(&fruit);
    }
  }
}
