#include <SPFD5408_Adafruit_TFTLCD.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Definición de colores.
#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_CYAN    0x07FF
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW  0xFFE0
#define TFT_WHITE   0xFFFF
#define TFT_GREY    0x5AEB
#define TFT_ORANGE  0xFD20

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

float    sx = 0,
         sy = 1,
         mx = 1,
         my = 0,
         hx = -1,
         hy = 0;            // Almacenar H, M, S x & y multiplicadores
float    sdeg = 0,
         mdeg = 0,
         hdeg = 0;
uint16_t osx = 120, 
         osy = 120, 
         omx = 120, 
         omy = 120, 
         ohx = 120, 
         ohy = 120;         // Almacenar H, M, S x & y coordenadas
int16_t  x0 = 0,
         x1 = 0,
         yy0 = 0,
         yy1 = 0,
         x00 = 0,
         yy00 = 0;
uint32_t targetTime = 0;    // Para la siguiente espera de 1 segundo

uint16_t xpos; // Posición de las
uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Se toma la hora del reloj al momento de compilar y subir el código
boolean initial = 1;
char d;

String getCurDateTime() {
  String date = String(__DATE__);
  String month = getCurMonth();
  String day = getCurDay();
  String year = date.substring(9, 11);
  return day + "-" + month + "-" + year;
}

String getCurDay() {
  String dy = String(__DATE__).substring(4, 6);
  if (dy == " 1")
  return "01";
  else if (dy == " 2")
  return "02";
  else if (dy == " 3")
  return "03";
  else if (dy == " 4")
  return "04";
  else if (dy == " 5")
  return "05";
  else if (dy == " 6")
  return "06";
  else if (dy == " 7")
  return "07";
  else if (dy == " 8")
  return "08";
  else if (dy == " 9")
  return "09";
  else
  return dy;
}

String getCurMonth() {
  String dt = String(__DATE__).substring(0, 3);
  if (dt == "Jan")
  return "01";
  else if (dt == "Feb")
  return "02";
  else if (dt == "Mar")
  return "03";
  else if (dt == "Apr")
  return "04";
  else if (dt == "May")
  return "05";
  else if (dt == "Jun")
  return "06";
  else if (dt == "Jul")
  return "07";
  else if (dt == "Aug")
  return "08";
  else if (dt == "Sep")
  return "09";
  else if (dt == "Oct")
  return "10";
  else if (dt == "Nov")
  return "11";
  else if (dt == "Dec")
  return "12";
  else
  return "¿?";
}

void setup(void) {
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1);

  tft.setTextColor(TFT_WHITE);  // Color texto
  tft.fillScreen(TFT_BLACK);    // Color fondo

  // Dibujar reloj principal
  xpos = tft.width() / 2;       // Obtener el centro de la pantalla a lo ancho
  tft.drawCircle(xpos, 120, 125, TFT_YELLOW); // Primer circulo exterior, el mas delgado
  tft.fillCircle(xpos, 120, 118, TFT_BLUE);   // Segundo circulo exterior
  tft.fillCircle(xpos, 120, 110, TFT_BLACK);  // Color interno del reloj 
  for (int a=95; a<104; a++){
  tft.drawCircle(xpos, 120, a, TFT_WHITE);} // Tercer circulo, donde se pondrán las líneas que identificaran los segundos

  // Pintar 12 líneas; cada una representará una hora distinta
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 114 + xpos;
    yy0 = sy * 114 + 120;
    x1 = sx * 100 + xpos;
    yy1 = sy * 100 + 120;
    
    tft.drawLine(x0, yy0, x1, yy1, TFT_YELLOW);
  }

  // Dibujar las 60 líneas que representarán los segundos
  for (int i = 0; i < 360; i += 6) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 102 + xpos;
    yy0 = sy * 102 + 120;
    x00 = sx * 92 + xpos;
    yy00 = sy * 92 + 120;
    
    // Trazar línea de minutos
    tft.drawLine(x0, yy0, x00, yy00, TFT_BLACK);
    tft.drawLine(x0+1, yy0+1, x00+1, yy00+1, TFT_BLACK);
    
    // Las líneas de los 4 cuadrantes serán más cortas
    if (i == 0 || i == 180 || i == 90 || i == 270) tft.fillCircle(x0, yy0, 2, TFT_WHITE);
  }

  //Dibujar círculo central, solo se usó para ver el centro de la pantalla
  //tft.fillCircle(xpos, 121, 3, TFT_WHITE);
  targetTime = millis() + 1000; // Se actualiza el tiempo de espera
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;
    if (ss == 60) {
      ss = 0;
      mm++;
      if (mm > 59) {
        mm = 0;
        hh++;
        if (hh > 23) {
          hh = 0;
        }
      }
    }
    
    // Precalculo de los grados para las coordenadas
    sdeg = ss * 6;                     // 0-59 -> 0-354
    mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - Incluye segundos, pero estos incrementos no se utilizan
    hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - Incluye minutos y segundos, pero estos incrementos no se utilizan
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial) {
      initial = 0;
      // Borrar las posiciones de las manecillas del reloj de las horas y los minutos cada minuto
      tft.drawLine(ohx, ohy, xpos, 121, TFT_BLACK);
      ohx = hx * 62 + xpos + 1;
      ohy = hy * 62 + 121;
      tft.drawLine(omx, omy, xpos, 121, TFT_BLACK);
      omx = mx * 84 + xpos;
      omy = my * 84 + 121;
    }

    // Pintar las nuevas posiciones de las manecillas de las horas y los minutos
    tft.drawLine(osx, osy, xpos, 121, TFT_BLACK);
    osx = sx * 90 + xpos + 1;
    osy = sy * 90 + 121;
    tft.drawLine(osx, osy, xpos, 121, TFT_RED);
    tft.drawLine(ohx, ohy, xpos, 121, TFT_CYAN);
    tft.drawLine(omx, omy, xpos, 121, TFT_WHITE);
    tft.drawLine(osx, osy, xpos, 121, TFT_RED);
    tft.fillCircle(xpos, 121, 3, TFT_RED);

  tft.setCursor(xpos-45, 45);
  tft.setTextSize(2);
  tft.print(getCurDateTime());

  tft.setCursor(xpos-50, 180);
  tft.setTextSize(2);
  tft.print(" VIERNES "); //Hasta 9 letras
  
  // Dibujar el mini reloj de los segundos
  tft.drawCircle(xpos, 155, 20, TFT_YELLOW);
  tft.drawCircle(xpos, 155, 18, TFT_BLUE);
  tft.drawCircle(xpos, 155, 17, TFT_CYAN);
  tft.drawCircle(xpos, 155, 16, TFT_CYAN);
  tft.fillRect(xpos-10, 149,22,15,TFT_BLACK);
  if(ss<10){tft.setCursor(xpos-10, 149); tft.setTextSize(2);
  tft.print('0'); tft.setCursor(xpos+2, 149);}
  else{
  tft.setCursor(xpos-10, 149);}
  tft.setTextSize(2);
  tft.print(ss);
  
  // Dibujar el mini reloj de los minutos
  tft.drawCircle(xpos+35, 117, 20, TFT_YELLOW);
  tft.drawCircle(xpos+35, 117, 18, TFT_BLUE);
  tft.drawCircle(xpos+35, 117, 17, TFT_CYAN);
  tft.drawCircle(xpos+35, 117, 16, TFT_CYAN);
 tft.fillRect(xpos+25, 111,22,15,TFT_BLACK);
  if(mm<10){tft.setCursor(xpos+25, 111); tft.setTextSize(2);
  tft.print('0'); tft.setCursor(xpos+37, 111);}
  else{
  tft.setCursor(xpos+25, 111);}
  tft.println(mm);
  
  // Dibujar el mini reloj de las horas
  tft.drawCircle(xpos-35, 117, 20, TFT_YELLOW);
  tft.drawCircle(xpos-35, 117, 18, TFT_BLUE);
  tft.drawCircle(xpos-35, 117, 17, TFT_CYAN);
  tft.drawCircle(xpos-35, 117, 16, TFT_CYAN);
  tft.fillRect(xpos-45, 111,22,15,TFT_BLACK); //erase
  if(hh<10){tft.setCursor(xpos-45, 111); tft.setTextSize(2);
  tft.print('0'); tft.setCursor(xpos-33, 111);}
  else{
  tft.setCursor(xpos-45, 111);}
  tft.setTextSize(2);
  tft.print(hh);
  //tft.setCursor(xpos-65, 111);
  //tft.println(':');

  if (hh>=0 && hh<12) d='A'; else {d='P';}
  tft.drawRoundRect(xpos-14,72,29,21,5,TFT_CYAN);
  tft.fillRect(xpos-11, 75,23,15,TFT_BLACK);
  tft.setCursor(xpos-11, 75);
  tft.setTextSize(2);
  tft.print(d);
  tft.println('M');
  }
}
