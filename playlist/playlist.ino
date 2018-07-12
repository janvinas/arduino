/**
 * Playlist con Arduino - Parte 3.2 
 * Selector de melodías:
 *  - Con un botón podremos ir pasando la canción, cuyo título de mostrará en la pantalla LCD. Al llegar a la última canción y pulsar el botón, la lista se reiniciará.
 *  - Con otro botón podremos reproducir la canción que esté seleccionada, la cual sonará hasta que termine. 
 *  - En caso de mostrar un título de canción en modo carrusel, no se podrá reproducir o cambiar de canción hasta que se haya mostrado todo el texto.
 *  - Una vez termine la canción, podremos volver a usar los botones de selección y reproducción.
 */
 
// Incluir librerías necesarias
#include <LiquidCrystal.h>
#include <Wire.h> 

// Inicialización de PINs para las conexiones con la pantalla LCD 
LiquidCrystal lcd( 8,9,4,5,6,7 ); 

#define OCTAVE_OFFSET 0

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951

// Puertos de entrada y salida
#define PIN_BOTON_PASAR 13
#define PIN_BOTON_SELECCIONAR 12
#define TONE_PIN 11
// PIN para determinar el brillo de la pantalla
#define LCD_Backlight 10
// Número de filas y columnas disponibles en la pantalla
#define N_FILAS 2
#define N_COLUMNAS 16
// Velocidad a la que queremos que se mueva el texto
#define VELOCIDAD 200 

// Array de notas (Brett Hagman)
const int notes[] = { 0,
NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

const char END_CHAR = 'E';

// Textos auxiliares y de bienvenida
const String texto_bienvenida_1 = "Campus Junior 18";
const String texto_bienvenida_2 = "### Playlist ###";
const String texto_aux = "################";
// Número de canciones disponibles
const int N_CANCIONES = 5;
// Array of songs to select them easily
char* notas_canciones[N_CANCIONES];
char* nombres_canciones[N_CANCIONES];

int indice;
int cancion_actual;
bool pantalla_bienvenida;

void setup()
{
  // Configuración monitor serie
  Serial.begin(9600); 
  // Inicializamos el array con las notas de cada canción
  //notas_canciones[0] = "The Simpsons:d=4,o=5,b=160:c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g,8p,8p,8f#,8f#,8f#,8g,a#.,8c6,8c6,8c6,c6";
  //notas_canciones[1] = "Indiana Jones:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
  notas_canciones[0] = "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6";
  notas_canciones[1] = "Looney Tunes:d=4,o=5,b=140:32p,c6,8f6,8e6,8d6,8c6,a.,8c6,8f6,8e6,8d6,8d#6,e.6,8e6,8e6,8c6,8d6,8c6,8e6,8c6,8d6,8a,8c6,8g,8a#,8a,8f";
  notas_canciones[2] = "Star Wars:d=4,o=5,b=45:32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#.6,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#6";
  notas_canciones[3] = "The Good, the Bad and the Ugly:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#";
  notas_canciones[4] = "Impossible Mission:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
  // Inicializamos el array con los nombres de cada canción
  //nombres_canciones[0] = "The Simpsons";
  //nombres_canciones[1] = "Indiana Jones";
  nombres_canciones[0] = "Entertainer";
  nombres_canciones[1] = "Looney Tunes";
  nombres_canciones[2] = "Star Wars";
  nombres_canciones[3] = "The Good, the Bad and the Ugly";
  nombres_canciones[4] = "Impossible Mission";

  // Índice de la canción a reproducir - Inicializado a 0 para mostrar la pantalla de bienvenida
  indice = 0; 
  pantalla_bienvenida = true;
  // Inicializamos la pantalla y mostramos el texto de bienvenida
  lcd.begin(N_COLUMNAS, N_FILAS);
  lcd.clear();
  
  // Instrucciones para modificar el brillo de la pantalla (SÓLO PARA LCD KEYPAD SHIELD)
  pinMode(LCD_Backlight, OUTPUT); 
  analogWrite(LCD_Backlight, 128); // Set the brightness of the backlight
  
  lcd.setCursor(0, 0);     
  lcd.print(texto_bienvenida_1);     
  lcd.setCursor(0, 1);     
  lcd.print(texto_bienvenida_2);   
}

void loop() {
  // Comprobar el valor del PIN donde está conectado el botón para ver si lo hemos pulsado
  int boton_pasar_pulsado = digitalRead(PIN_BOTON_PASAR);

  bool selector_pulsado = false;
  
  // Si el botón ha sido pulsado, realizar lo siguiente
  if (boton_pasar_pulsado == 1) 
  {    
    pantalla_bienvenida = false;
    // Si hemos llegado al final de la lista, volver a empezar
    if (indice > N_CANCIONES) {
      indice = 0;
    }
    // Borrar información en la pantalla LCD
    lcd.clear();  
    // Mostrar el título de la canción por pantalla
    pintarTextoLCD(nombres_canciones[indice]); 
    // Comprobar el valor del PIN donde está conectado el botón para ver si lo hemos pulsado
    // Incrementar el índice de la canción
    selector_pulsado = true;
    cancion_actual = indice;
  }

  int boton_seleccionar_pulsado = digitalRead(PIN_BOTON_SELECCIONAR);
  //Serial.println(boton_seleccionar_pulsado);
  // Si el botón ha sido pulsado, realizar lo siguiente
  if (boton_seleccionar_pulsado == 1 && !pantalla_bienvenida) 
  {
    // Reproducir la canción
    play_rtttl(notas_canciones[cancion_actual]); 
  }

  if (selector_pulsado == true) {
    indice ++;   
  }

  delay(100);
}

// Método para pintar en la pantalla LCD un texto
void pintarTextoLCD(String texto){
  // Obtenemos el tamaño del texto
  int tam_texto=texto.length();
  // En caso de que el texto sea mayor al espacio en la pantalla, realizamos el carrusel
  if (tam_texto > N_COLUMNAS) {
    // Mostramos el texto y desplazamos hacia la derecha
    for(int i = 1; i <= tam_texto+1 ; i ++)
    {        
      String texto_desplazado = texto.substring(i-1);     
      // Limpiamos pantalla
      lcd.clear();     
      //Situamos el cursor
      lcd.setCursor(0, 0);     
      // Pintamos el texto desplazado por pantalla
      lcd.print(texto_desplazado);
      lcd.setCursor(0, 1);     
      lcd.print(texto_aux);       
      // Esperamos a realizar el siguiente desplazamiento
      delay(VELOCIDAD);         
    } 
    lcd.clear();     
    //Situamos el cursor
    lcd.setCursor(0, 0); 
    lcd.print(texto);       
  // En caso de que el texto quepa en la pantalla, simplemente lo mostramos  
  } else {
    //Situamos el cursor
    lcd.setCursor(0, 0);
    // Pintamos el texto por pantalla
    lcd.print(texto);
    lcd.setCursor(0, 1);     
    lcd.print(texto_aux);  
  }
}

// Método para reproducir melodía (basado en el código de Brett Hagman)
void play_rtttl(char *p)
{
  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while(*p != ':') p++;    // ignore name
  p++;                     // skip ':'

  // get default duration
  if(*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while(isDigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) default_dur = num;
    p++;                   // skip comma
  }

  //Serial.print("ddur: "); Serial.println(default_dur, 10);

  // get default octave
  if(*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) default_oct = num;
    p++;                   // skip comma
  }

  //Serial.print("doct: "); Serial.println(default_oct, 10);

  // get BPM
  if(*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while(isDigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }

  //Serial.print("bpm: "); Serial.println(bpm, 10);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  //Serial.print("wn: "); Serial.println(wholenote, 10);

  // now begin note loop
  while(*p)
  {
    // first, get note duration, if available
    num = 0;
    while(isDigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    
    if(num) duration = wholenote / num;
    else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch(*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if(*p == '#')
    {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if(*p == '.')
    {
      duration += duration/2;
      p++;
    }
  
    // now, get scale
    if(isDigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if (*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // From line 248 to 256
 // Added to code so end of the song signal is transmitted
    //  It is a very dirty way to send the END_CHAR but it was
    //  impossible to make it work by sending just the END_CHAR
    char songPlaying = *p;
    if (songPlaying)
    {
      Serial.print(" ");
    }
    else
    {
      Serial.print(END_CHAR);
    }

    // now play the note
    if(note)
    {
      //Serial.print("Playing: ");
      //Serial.print(scale, 10); Serial.print(' ');
      //Serial.print(note, 10); Serial.print(" (");
      //Serial.print(notes[(scale - 4) * 12 + note], 10);
      //Serial.print(") ");
      //Serial.println(duration, 10);
      tone(TONE_PIN, notes[(scale - 4) * 12 + note]);
      delay(duration);
      noTone(TONE_PIN);
    }
    else
    {
      //Serial.print("Pausing: ");
      //Serial.println(duration, 10);
      delay(duration);
    }
  }
}
